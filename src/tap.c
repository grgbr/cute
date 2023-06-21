#include "cons.h"
#include "suite.h"
#include <string.h>
#include <errno.h>

struct cute_tap_report {
	struct cute_report super;
	FILE *             stdio;
};

#define cute_tap_report_assert(_report) \
	cute_assert(_report); \
	cute_report_assert(&(_report)->super); \
	cute_assert((_report)->stdio)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_tap_report_assert_intern(_report) \
	cute_tap_report_assert(_report)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_tap_report_assert_intern(_report)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static void
cute_tap_report_test_done(const struct cute_tap_report * report,
                          const struct cute_run *        run)
{
	int depth = (run->depth - 1) * 4;

	switch (run->issue) {
	case CUTE_PASS_ISSUE:
		fprintf(report->stdio,
		        "%*sok %d - %s\n",
		        depth, "",
		        run->id + 1,
		        run->base->name);
		break;

	case CUTE_SKIP_ISSUE:
		cute_assert_intern(run->what);
		cute_assert_intern(run->why);

		fprintf(report->stdio,
		        "%2$*1$sok %3$d - %4$s # SKIP %5$s: %6$s\n"
		        "%2$*1$s  ---\n"
		        "%2$*1$s  at:\n"
		        "%2$*1$s    file: %7$s\n"
		        "%2$*1$s    line: %8$d\n"
		        "%2$*1$s  ...\n",
		        depth, "",
		        run->id + 1, run->base->name, run->what, run->why,
		        run->file,
		        run->line);
		break;

	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		cute_assert_intern(run->what);
		cute_assert_intern(run->why);

		fprintf(report->stdio,
		        "%2$*1$snot ok %3$d - %4$s\n"
		        "%2$*1$s  ---\n"
		        "%2$*1$s  severity: fail\n"
		        "%2$*1$s  message: \"%5$s\"\n"
		        "%2$*1$s  reason: \"%6$s\"\n"
		        "%2$*1$s  at:\n"
		        "%2$*1$s    file: %7$s\n"
		        "%2$*1$s    line: %8$d\n"
		        "%2$*1$s  ...\n",
		        depth, "",
		        run->id + 1, run->base->name,
		        run->what,
		        run->why,
		        run->file,
		        run->line);
		break;

	case CUTE_OFF_ISSUE:
		fprintf(report->stdio,
		        "%*sok %d - %s # SKIP disabled\n",
		        depth, "",
		        run->id + 1,
		        run->base->name);
		break;

	default:
		__cute_unreachable();
	}

}

static void
cute_tap_report_test(struct cute_tap_report * report,
                      enum cute_event         event,
                      const struct cute_run * run)
{
	switch (event) {

	case CUTE_DONE_EVT:
		cute_tap_report_test_done(report, run);
		break;

	case CUTE_INIT_EVT:
	case CUTE_SETUP_EVT:
	case CUTE_EXEC_EVT:
	case CUTE_TEARDOWN_EVT:
	case CUTE_FINI_EVT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_tap_report_on_head(struct cute_tap_report * report)
{
	fprintf(report->stdio, "TAP version 14\n");
}

static void
cute_tap_report_suite_setup(const struct cute_tap_report * report,
                            const struct cute_suite_run *  suite)
{
	if (suite->super.parent) {
		int depth = (suite->super.depth - 1) * 4;

		fprintf(report->stdio,
		        "%2$*1$s# Subtest: %3$s\n"
		        "%2$*1$s    1..%4$u\n",
		        depth,
		        "",
		        suite->super.base->name,
		        suite->stats.total);
	}
	else
		fprintf(report->stdio, "1..%u\n", suite->stats.total);
}

static void
cute_tap_report_suite(struct cute_tap_report *       report,
                       enum cute_event               event,
                       const struct cute_suite_run * suite)
{
	switch (event) {

	case CUTE_HEAD_EVT:
		cute_tap_report_on_head(report);
		break;

	case CUTE_SETUP_EVT:
		cute_tap_report_suite_setup(report, suite);
		break;

	case CUTE_DONE_EVT:
		if (suite->super.parent)
			cute_tap_report_test_done(report, &suite->super);
		break;

	case CUTE_SHOW_EVT:
		/* IMPLEMENT ME ! */
		cute_assert(0);
		break;

	case CUTE_INIT_EVT:
	case CUTE_EXEC_EVT:
	case CUTE_TEARDOWN_EVT:
	case CUTE_FINI_EVT:
	case CUTE_FOOT_EVT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_tap_report_handle(struct cute_report *    report,
                       enum cute_event         event,
                       enum cute_kind          kind,
                       const struct cute_run * run)
{
	cute_tap_report_assert_intern((struct cute_tap_report *)report);
	cute_run_assert_intern(run);

	switch (kind) {
	case CUTE_TEST_KIND:
		cute_tap_report_test((struct cute_tap_report *)report,
		                      event,
		                      run);
		break;

	case CUTE_SUITE_KIND:
		cute_tap_report_suite((struct cute_tap_report *)report,
		                       event,
		                       (const struct cute_suite_run *)run);
		break;

	default:
		__cute_unreachable();
	}
}

static int
cute_tap_report_release(struct cute_report * report)
{
	cute_tap_report_assert_intern((struct cute_tap_report *)report);

	struct cute_tap_report * rprt = (struct cute_tap_report *)report;
	FILE *                   stdio = rprt->stdio;

	cute_free(report);

	return cute_close_stdio(stdio);
}

int
cute_report_setup_tap(const struct cute_config * config)
{
	cute_config_assert_intern(config);

	struct cute_tap_report * rprt;
	int                      ret;

	rprt = cute_malloc(sizeof(*rprt));

	rprt->super.handle = cute_tap_report_handle;
	rprt->super.release = cute_tap_report_release;

	if (config->tap_path) {
		rprt->stdio = fopen(config->tap_path, "w");
		if (!rprt->stdio) {
			ret = -errno;
			cute_error("'%s': cannot open output file: %s (%d).\n",
			           config->tap_path,
			           strerror(errno),
			           errno);
			goto free;
		}

		cute_report_register(&rprt->super);
	}
	else {
		rprt->stdio = stdout;
		cute_cons_report_register(&rprt->super);
	}

	return 0;

free:
	cute_free(rprt);

	return ret;
}
