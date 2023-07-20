#include "cons.h"
#include "run.h"
#include "suite.h"
#include <string.h>

static void
cute_terse_report_progress(const struct cute_cons_report * report)
{
	unsigned int prog;
	int          total = report->colnr - (CUTE_CONS_REPORT_PROG_WIDTH + 2);
	int          fill;

	prog = cute_report_progress();
	cute_assert_intern(prog <= 100U);
	fill = (total * (int)prog) / 100;

	fprintf(cute_iodir_stderr,
	        "\r" CUTE_CONS_REPORT_PROG_FMT "[%.*s%*s]",
	        prog,
	        fill,
	        report->fill,
	        total - fill,
	        "");

	fflush(cute_iodir_stderr);
}

static void
cute_terse_report_on_init(struct cute_cons_report * report,
                          const struct cute_run *   run)
{
	int len;
	int cols;

	len = (int)strnlen(run->name, CUTE_FULL_NAME_MAX_LEN);
	cols = cute_term_depth_width(run->base->name, run->depth);

	if ((len + 1 + 3 + 1 + CUTE_CONS_REPORT_STAT_WIDTH) >
	    (cols + CUTE_CONS_REPORT_SUMUP_WIDTH))
		cols = (int)len;

	cute_cons_report_on_init(report, cols);
}

static int
cute_terse_report_fill_len(const struct cute_cons_report * report,
                           const char *                    name)
{
	int max = report->colnr - (1 + 3 + 1 + CUTE_CONS_REPORT_STAT_WIDTH);

	return report->colnr -
	       (int)strnlen(name, (size_t)max) -
	       1 -
	       (1 + CUTE_CONS_REPORT_STAT_WIDTH);
}

static void
cute_terse_report_test_done(const struct cute_cons_report * report,
                            const struct cute_run *         run)
{
	if ((run->issue == CUTE_FAIL_ISSUE) ||
	    (run->issue == CUTE_EXCP_ISSUE)) {
		const char * name = run->name;
		int          fill;
		sigset_t     set;

		fill = cute_terse_report_fill_len(report, name);

		fputc('\r', cute_iodir_stderr);
		fflush(cute_iodir_stderr);

		fprintf(report->stdio,
		        "%s %*.*s ",
		        name,
		        fill,
		        fill,
		        report->fill);

		cute_cons_report_mask(report, &set);
		cute_cons_report_test_done(report, run);
		cute_cons_report_unmask(report, &set);

		fputc('\n', report->stdio);
	}

	cute_terse_report_progress(report);
}

static void
cute_terse_report_test(struct cute_cons_report * report,
                       enum cute_event           event,
                       const struct cute_run *   run)
{
	switch (event) {
	case CUTE_INIT_EVT:
		cute_terse_report_on_init(report, run);
		break;

	case CUTE_DONE_EVT:
		cute_terse_report_test_done(report, run);
		break;

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
cute_terse_report_suite_done(const struct cute_cons_report * report,
                             const struct cute_suite_run *   suite)
{
	if ((suite->super.issue == CUTE_FAIL_ISSUE) ||
	    (suite->super.issue == CUTE_EXCP_ISSUE)) {
		const char * name = suite->super.name;
		int          fill;
		sigset_t     set;

		fill = cute_terse_report_fill_len(report, name);

		fputc('\r', cute_iodir_stderr);
		fflush(cute_iodir_stderr);

		cute_cons_report_mask(report, &set);
		fprintf(report->stdio,
		        "%s%s%s %*.*s ",
		        report->term.blue,
		        name,
		        report->term.regular,
		        fill,
		        fill,
		        report->fill);
		cute_cons_report_test_done(report, &suite->super);
		cute_cons_report_unmask(report, &set);

		fputc('\n', report->stdio);
	}

	cute_terse_report_progress(report);
}

static void
cute_terse_report_on_head(struct cute_cons_report * report)
{
	sigset_t set;

	int colnr = report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH;

	cute_cons_report_on_head(report,
	                         colnr,
	                         colnr - (CUTE_CONS_REPORT_PROG_WIDTH + 2));

	cute_cons_report_mask(report, &set);
	cute_cons_report_head(report);
	cute_cons_report_unmask(report, &set);
}

static void
cute_terse_report_on_foot(const struct cute_cons_report * report,
                          const struct cute_suite_run *   suite)
{
	sigset_t set;

	fputc('\r', cute_iodir_stderr);
	fflush(cute_iodir_stderr);

	cute_cons_report_mask(report, &set);
	cute_cons_report_sumup(report, suite);
	cute_cons_report_unmask(report, &set);
}

static void
cute_terse_report_on_show(const struct cute_cons_report * report,
                          const struct cute_suite_run *   suite)
{
	sigset_t set;

	cute_cons_report_mask(report, &set);
	cute_report_on_show(&suite->super,
	                    report->stdio,
	                    report->term.blue,
	                    report->term.regular);
	cute_cons_report_unmask(report, &set);
}

static void
cute_terse_report_suite(struct cute_cons_report *     report,
                        enum cute_event               event,
                        const struct cute_suite_run * suite)
{
	switch (event) {
	case CUTE_INIT_EVT:
		cute_terse_report_on_init(report, &suite->super);
		break;

	case CUTE_HEAD_EVT:
		cute_terse_report_on_head(report);
		break;

	case CUTE_DONE_EVT:
		cute_terse_report_suite_done(report, suite);
		break;

	case CUTE_FOOT_EVT:
		cute_terse_report_on_foot(report, suite);
		break;

	case CUTE_SHOW_EVT:
		cute_terse_report_on_show(report, suite);
		break;

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
cute_terse_report_handle(struct cute_report *    report,
                         enum cute_event         event,
                         enum cute_kind          kind,
                         const struct cute_run * run)
{
	cute_cons_report_assert_intern((struct cute_cons_report *)report);
	cute_run_assert_intern(run);

	switch (kind) {
	case CUTE_TEST_KIND:
		cute_terse_report_test((struct cute_cons_report *)report,
		                       event,
		                       run);
		break;

	case CUTE_SUITE_KIND:
		cute_terse_report_suite((struct cute_cons_report *)report,
		                        event,
		                        (const struct cute_suite_run *)run);
		break;

	default:
		__cute_unreachable();
	}
}

int
cute_report_setup_terse(const struct cute_config * config)
{
	cute_config_assert_intern(config);

	struct cute_cons_report * rprt;
	int                       ret;

	rprt = cute_malloc(sizeof(*rprt));

	ret = cute_cons_report_setup(rprt, config, cute_terse_report_handle);
	if (!ret)
		return 0;

	cute_free(rprt);

	return ret;
}
