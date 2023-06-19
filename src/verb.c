#include "cons.h"
#include "run.h"
#include "suite.h"

#define CUTE_VERB_REPORT_OVER_LABEL "OVERALL"
#define CUTE_VERB_REPORT_OVER_WIDTH ((int) \
                                     sizeof(CUTE_VERB_REPORT_OVER_LABEL) - 1)

static void
cute_verb_report_on_init(struct cute_cons_report * report,
                         const struct cute_run *   run)
{
	cute_cons_report_on_init(report,
	                         cute_term_depth_width(run->base->name,
	                                               run->depth));
}

static void
cute_verb_report_test_setup(const struct cute_cons_report * report,
                            const struct cute_run *         run)
{
	const char * name = run->base->name;
	int          fill = report->colnr -
	                    CUTE_CONS_REPORT_PROG_WIDTH -
	                    (1 + cute_term_depth_width(name,
	                                               run->depth)) -
	                    1 -
	                    (1 + CUTE_CONS_REPORT_TIME_WIDTH) -
	                    (1 + CUTE_CONS_REPORT_STAT_WIDTH);

	fprintf(report->stdio,
	        CUTE_CONS_REPORT_PROG_FMT " ",
	        cute_report_progress());

	cute_term_depth_printf(&report->term,
	                       report->stdio,
	                       run->depth,
	                       "%s %*.*s ",
	                       name,
	                       fill, fill, report->fill);

	fflush(report->stdio);
}

static void
cute_verb_report_test_done(struct cute_cons_report * report,
                           const struct cute_run *   run)
{
	struct timespec diff;

	cute_diff_tspec(&diff, &run->begin, &run->end);

	fprintf(report->stdio,
	        CUTE_CONS_REPORT_TIME_FMT " ",
	        diff.tv_sec, diff.tv_nsec / 1000L);

	cute_cons_report_test_done(report, run);
}

static void
cute_verb_report_test(struct cute_cons_report * report,
                      enum cute_event           event,
                      const struct cute_run *   run)
{
	switch (event) {
	case CUTE_INIT_EVT:
		cute_verb_report_on_init(report, run);
		break;

	case CUTE_SETUP_EVT:
		cute_verb_report_test_setup(report, run);
		break;

	case CUTE_DONE_EVT:
		cute_verb_report_test_done(report, run);
		break;

	case CUTE_EXEC_EVT:
	case CUTE_TEARDOWN_EVT:
	case CUTE_FINI_EVT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_verb_report_on_head(struct cute_cons_report * report)
{
	int colnr = CUTE_CONS_REPORT_PROG_WIDTH +
	            (1 + report->ncols) +
	            1 + 3 +
	            (1 + CUTE_CONS_REPORT_TIME_WIDTH) +
	            (1 + CUTE_CONS_REPORT_STAT_WIDTH);

	if (colnr < (report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH))
		colnr = report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH;

	cute_cons_report_on_head(report,
	                         colnr,
	                         colnr - (CUTE_CONS_REPORT_PROG_WIDTH +
	                                  1 +
	                                  1 +
	                                  (1 + CUTE_CONS_REPORT_STAT_WIDTH)));

	cute_cons_report_head(report);

	fprintf(report->stdio,
	        "%s"
	        CUTE_CONS_REPORT_PROG_PAD
	        " " CUTE_CONS_REPORT_NAME_HEAD
	        "%*s"
	        " " CUTE_CONS_REPORT_TIME_HEAD
	        " " CUTE_CONS_REPORT_STAT_HEAD
	        "%s"
	        "\n",
	        report->term.bold,
	        colnr - (CUTE_CONS_REPORT_PROG_WIDTH +
	                 (1 + (int)sizeof(CUTE_CONS_REPORT_NAME_HEAD) - 1) +
	                 (1 + CUTE_CONS_REPORT_TIME_WIDTH) +
	                 (1 + CUTE_CONS_REPORT_STAT_WIDTH)),
	        "",
	        report->term.regular);
}

static void
cute_verb_report_suite_setup(const struct cute_cons_report * report,
                             const struct cute_suite_run *   run)
{
	fprintf(report->stdio,
	        CUTE_CONS_REPORT_PROG_FMT " ",
	        cute_report_progress());

	cute_term_depth_printf(&report->term,
	                       report->stdio,
	                       run->super.depth,
	                       "%s%s%s\n",
	                       report->term.blue,
	                       run->super.base->name,
	                       report->term.regular);
}

static void
cute_verb_report_on_foot(const struct cute_cons_report * report,
                         const struct cute_suite_run *   suite)
{
	enum cute_issue issue = suite->super.issue;
	const char *    color;
	const char *    label;
	struct timespec diff;
	int             fill = report->colnr -
	                       CUTE_CONS_REPORT_PROG_WIDTH -
	                       (1 + CUTE_VERB_REPORT_OVER_WIDTH) -
	                       1 -
	                       (1 + CUTE_CONS_REPORT_TIME_WIDTH) -
	                       (1 + CUTE_CONS_REPORT_STAT_WIDTH);

	color = cute_term_issue_color(&report->term, issue);
	label = cute_issue_label(issue);

	cute_diff_tspec(&diff, &suite->super.begin, &suite->super.end);

	fprintf(report->stdio,
	        CUTE_CONS_REPORT_PROG_FMT
	        " %s" CUTE_VERB_REPORT_OVER_LABEL "%s"
	        " %*.*s"
	        " " CUTE_CONS_REPORT_TIME_FMT
	        " %s%*s"
	        "%s\n\n\n",
	        cute_report_progress(),
	        report->term.blue, report->term.regular,
	        fill, fill, report->fill,
	        diff.tv_sec, diff.tv_nsec / 1000L,
	        color, CUTE_CONS_REPORT_STAT_WIDTH, label,
	        report->term.regular);

	cute_cons_report_sumup(report, suite);
}

static void
cute_verb_report_test_show(const struct cute_cons_report * report,
                           const struct cute_run *         run)
{
	const char * name = run->base->name;
	const char * color;
	const char * active;
	int          cols;

	if (run->state == CUTE_OFF_STATE) {
		color = report->term.gray;
		active = "off";
	}
	else {
		color = report->term.fore;
		active = "on";
	}

	cols = report->ncols - cute_term_indent_width(run->depth);
	cute_term_depth_printf(&report->term,
	                       report->stdio,
	                       run->depth,
	                       "%s"
	                       "%-*s"
	                       "  %3s  "
	                       " %s:%d"
	                       "%s\n",
	                       color,
	                       cols, name,
	                       active,
	                       run->base->file, run->base->line,
	                       report->term.regular);
}

static void
cute_verb_report_suite_show(const struct cute_cons_report * report,
                            const struct cute_run *         run)
{
	const char * name = run->base->name;
	const char * color;
	const char * active;
	int          cols;

	if (run->state == CUTE_OFF_STATE) {
		color = report->term.gray;
		active = "off";
	}
	else {
		color = report->term.fore;
		active = "on";
	}

	cols = report->ncols - cute_term_indent_width(run->depth);
	cute_term_depth_printf(&report->term,
	                       report->stdio,
	                       run->depth,
	                       "%s%-*s"
	                       "  %s%3s  "
	                       " %s:%d"
	                       "%s\n",
	                       report->term.blue, cols, name,
	                       color, active,
	                       run->base->file, run->base->line,
	                       report->term.regular);
}

static void
cute_verb_report_show_run(struct cute_run * run,
                          enum cute_visit   visit,
                          void *            data)
{
	cute_run_assert_intern(run);
	cute_cons_report_assert_intern((struct cute_cons_report *)data);

	const struct cute_cons_report * report =
		(const struct cute_cons_report *)data;

	switch (visit) {
	case CUTE_BEGIN_VISIT:
		cute_verb_report_suite_show(report, run);
		break;

	case CUTE_ONCE_VISIT:
		cute_verb_report_test_show(report, run);
		break;

	case CUTE_END_VISIT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_verb_report_show(const struct cute_cons_report * report,
                      const struct cute_suite_run *   run)
{
	fprintf(report->stdio,
	        "%s"
	        "%-*s"
	        " ACTIVE FILE:LINE"
	        "%s\n",
	        report->term.bold,
	        report->ncols, CUTE_CONS_REPORT_NAME_HEAD,
	        report->term.regular);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	cute_run_foreach((struct cute_run *)run,
	                 cute_verb_report_show_run,
	                 (struct cute_cons_report *)report);
#pragma GCC diagnostic pop
}

static void
cute_verb_report_suite(struct cute_cons_report *     report,
                       enum cute_event               event,
                       const struct cute_suite_run * suite)
{
	switch (event) {
	case CUTE_INIT_EVT:
		cute_verb_report_on_init(report, &suite->super);
		break;

	case CUTE_HEAD_EVT:
		cute_verb_report_on_head(report);
		break;

	case CUTE_SETUP_EVT:
		cute_verb_report_suite_setup(report, suite);
		break;

	case CUTE_FOOT_EVT:
		cute_verb_report_on_foot(report, suite);
		break;

	case CUTE_SHOW_EVT:
		cute_verb_report_show(report, suite);
		break;

	case CUTE_EXEC_EVT:
	case CUTE_TEARDOWN_EVT:
	case CUTE_DONE_EVT:
	case CUTE_FINI_EVT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_verb_report_handle(struct cute_report *    report,
                        enum cute_event         event,
                        enum cute_kind          kind,
                        const struct cute_run * run)
{
	cute_cons_report_assert_intern((struct cute_cons_report *)report);
	cute_run_assert_intern(run);

	switch (kind) {
	case CUTE_TEST_KIND:
		cute_verb_report_test((struct cute_cons_report *)report,
		                      event,
		                      run);
		break;

	case CUTE_SUITE_KIND:
		cute_verb_report_suite((struct cute_cons_report *)report,
		                       event,
		                       (const struct cute_suite_run *)run);
		break;

	default:
		__cute_unreachable();
	}
}

int
cute_report_setup_verb(const struct cute_config * config)
{
	cute_config_assert_intern(config);

	struct cute_cons_report * rprt;
	int                       ret;

	rprt = cute_malloc(sizeof(*rprt));

	ret = cute_cons_report_setup(rprt, config, cute_verb_report_handle);
	if (!ret)
		return 0;

	cute_free(rprt);

	return ret;
}
