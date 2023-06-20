#include "cons.h"
#include "suite.h"
#include <ctype.h>
#include <string.h>

static struct cute_report * cute_the_cons_report;

static char *
cute_cons_report_create_hrule(char sym, int len)
{
	cute_assert_intern(isprint((int)sym));
	cute_assert_intern(len > 0);

	char * hrule;

	hrule = cute_malloc((size_t)len + 1);
	memset(hrule, sym, (size_t)len);
	hrule[len] = '\0';

	return hrule;
}

void
cute_cons_report_head(const struct cute_cons_report * report)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(report->ncols >= CUTE_CONS_NAME_COLS_MIN);
	cute_assert_intern(report->fill);
	cute_assert_intern((report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH) <=
	                   report->colnr);
	cute_assert_intern(cute_report_title);
	cute_assert_intern(cute_report_title[0]);

	char *       hrule;
	size_t       ttl_len;
	size_t       prt_len;
	int          pre_len;
	int          post_len;
	const char * bold = report->term.bold;
	const char * reg = report->term.regular;

	hrule = cute_cons_report_create_hrule(CUTE_CONS_REPORT_HRULE_SYM,
	                                      report->colnr / 2);

	ttl_len = strnlen(cute_report_title,
	                  (size_t)report->colnr -
	                  (sizeof("###  Running ") - 1) -
	                  (sizeof(" test(s)  ###") - 1));
	prt_len = (sizeof("  Running ") - 1) +
	          ttl_len +
	          (sizeof(" test(s)  ") - 1);
	pre_len = (report->colnr - (int)prt_len) / 2;
	post_len = report->colnr - (pre_len + (int)prt_len);

	fprintf(report->stdio,
	        "\n"
	        "%.*s"
	        "  Running %s%.*s%s test(s)  "
	        "%.*s"
	        "\n\n",
	        pre_len, hrule,
	        bold, (int)ttl_len, cute_report_title, reg,
	        post_len, hrule);

	cute_free(hrule);
}

void
cute_cons_report_test_done(const struct cute_cons_report * report,
                           const struct cute_run *         run)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(report->ncols >= CUTE_CONS_NAME_COLS_MIN);
	cute_assert_intern(report->fill);
	cute_assert_intern((report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH) <=
	                   report->colnr);
	cute_run_assert_intern(run);

	fprintf(report->stdio,
	        "%s%*s%s\n",
	        cute_term_issue_color(&report->term, run->issue),
	        CUTE_CONS_REPORT_STAT_WIDTH,
	        cute_issue_label(run->issue),
	        report->term.regular);

	if (run->issue == CUTE_FAIL_ISSUE) {
		cute_assert_intern(run->what);
		cute_assert_intern(run->why);

		fprintf(report->stdio,
		        "%s@ %s:%d%s\n",
		        report->term.gray,
		        run->file,
		        run->line,
		        report->term.fore);

		fprintf(report->stdio,
		        "%s%s: %s%s%s\n",
		        report->term.gray,
		        run->what,
		        report->term.bold,
		        run->why,
		        report->term.regular);
	}
}

static void
cute_cons_report_suite_sumup(const struct cute_cons_report * report,
                             const struct cute_suite_run *   suite,
                             bool                            total)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(report->ncols >= CUTE_CONS_NAME_COLS_MIN);
	cute_assert_intern(report->fill);
	cute_assert_intern((report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH) <=
	                   report->colnr);
	cute_run_assert_intern(&suite->super);

	const struct cute_term *  term = &report->term;
	enum cute_issue           issue = suite->super.issue;
	int                       depth = suite->super.depth;
	const char *              color;
	struct timespec           diff;
	const char *              bold = total ? term->bold : "";
	const char *              blue = !total ? term->blue : "";
	const char *              green = total ? term->green : "";
	const char *              yellow = total ? term->yellow : "";
	const char *              red = total ? term->red : "";
	const char *              fore = term->fore;
	const char *              name = total ? "Total" :
	                                         suite->super.base->name;
	int                       width = report->colnr -
	                                  cute_term_indent_width(depth) -
	                                  CUTE_CONS_REPORT_SUMUP_WIDTH;
        const struct cute_stats * stats = total ? &suite->sums : &suite->stats;

	color = cute_term_issue_color(term, issue);
	cute_diff_tspec(&diff, &suite->super.begin, &suite->super.end);

	cute_term_depth_printf(term,
	                       report->stdio,
	                       depth,
	                       "%s"
	                       "%s" CUTE_CONS_REPORT_NAME_FMT
	                       " %s" CUTE_CONS_REPORT_STAT_FMT "%s"
	                       " " CUTE_CONS_REPORT_TIME_FMT
	                       " %s" CUTE_CONS_REPORT_PASS_FMT
	                       " %s" CUTE_CONS_REPORT_SKIP_FMT
	                       " %s" CUTE_CONS_REPORT_FAIL_FMT
	                       " %s" CUTE_CONS_REPORT_EXEC_FMT
	                       " " CUTE_CONS_REPORT_TOTAL_FMT
	                       "%s"
	                       "\n",
	                       bold,
	                       blue, width, width, name,
	                       color, cute_issue_label(issue), term->fore,
	                       diff.tv_sec, diff.tv_nsec / 1000L,
	                       green, stats->pass,
	                       yellow, stats->skip,
	                       red, stats->fail,
	                       fore, stats->exec,
	                       stats->total,
	                       term->regular);

	if (issue != CUTE_OFF_ISSUE)
		fprintf(report->stdio,
		        "%s"
		        CUTE_CONS_REPORT_NAME_FMT
		        " " CUTE_CONS_REPORT_STAT_PAD
		        " " CUTE_CONS_REPORT_TIME_PAD
		        "%s" CUTE_CONS_REPORT_PASS_FMT "%%"
		        "%s" CUTE_CONS_REPORT_SKIP_FMT "%%"
		        "%s" CUTE_CONS_REPORT_FAIL_FMT "%%"
		        "%s" CUTE_CONS_REPORT_EXEC_FMT "%%"
		        "%s"
		        "\n",
		        bold,
		        report->ncols, report->ncols, "",
		        green, (stats->pass * 100) / stats->exec,
		        yellow, (stats->skip * 100) / stats->exec,
		        red, (stats->fail * 100) / stats->exec,
		        fore, (stats->exec * 100) / stats->total,
		        term->regular);
	else
		fprintf(report->stdio,
		        "%s"
		        CUTE_CONS_REPORT_NAME_FMT
		        " " CUTE_CONS_REPORT_STAT_PAD
		        " " CUTE_CONS_REPORT_TIME_PAD
		        "%s" CUTE_CONS_REPORT_PASS_FMT "%%"
		        "%s" CUTE_CONS_REPORT_SKIP_FMT "%%"
		        "%s" CUTE_CONS_REPORT_FAIL_FMT "%%"
		        "%s" CUTE_CONS_REPORT_EXEC_FMT "%%"
		        "%s"
		        "\n",
		        bold,
		        report->ncols, report->ncols, "",
		        green, 0,
		        yellow, 0,
		        red, 0,
		        fore, 0,
		        term->regular);
}

static void
cute_cons_report_process_sumup(struct cute_run * run,
                               enum cute_visit   visit,
                               void *            data)
{
	switch (visit) {
	case CUTE_BEGIN_VISIT:
		cute_cons_report_suite_sumup(
			(const struct cute_cons_report *)data,
			(const struct cute_suite_run *)run,
			false);
		break;

	case CUTE_END_VISIT:
	case CUTE_ONCE_VISIT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_cons_report_results(const struct cute_cons_report * report,
                         const struct cute_suite_run *   suite)
{
	size_t          sz = (size_t)report->colnr + 1;
	enum cute_issue issue = suite->super.issue;
	char const      title[] = "SUMMARY";
	int             tlen = sizeof(title) - 1;
	char *          body;
	int             blen;
	int             len = tlen;
	int             tpad;
	int             bpad;
	int             indent;
	const char *    color;
	const char *    bold = report->term.bold;
	const char *    regular = report->term.regular;

	body = cute_malloc(sz);

	switch (issue) {
	case CUTE_PASS_ISSUE:
		blen = snprintf(body,
		                sz,
		                "%u test(s) PASSED",
		                suite->sums.pass);
		break;

	case CUTE_SKIP_ISSUE:
		blen = snprintf(body,
		                sz,
		                "%u test(s) SKIPPED",
		                suite->sums.skip);
		break;

	case CUTE_FAIL_ISSUE:
		blen = snprintf(body,
		                sz,
		                "%u/%u (%u%%) test(s) FAILED",
		                suite->sums.fail,
		                suite->sums.exec,
		                suite->sums.fail * 100 / suite->sums.exec);
		break;

	case CUTE_OFF_ISSUE:
		blen = snprintf(body,
		                sz,
		                "%u test(s) DISABLED",
		                suite->sums.total);
		break;

	default:
		__cute_unreachable();
	}

	if (blen > len)
		len = blen;

	tpad = (len - tlen) / 2;
	bpad = (len - blen) / 2;
	indent = (report->colnr - (3 + 2 + len + 2 + 3)) / 2;

	color = cute_term_issue_color(&report->term, issue);

	fprintf(report->stdio,
	        "\n"
	        "%*s###  %*s"
	        "%s%s%s"
	        "%*s  ###\n"
	        "%*s###  %*s"
	        "%s%s%s%s"
	        "%*s  ###\n"
	        "\n",
	        indent, "", tpad, "",
	        bold, title, regular,
	        len - tlen - tpad, "",
	        indent, "", bpad, "",
	        bold, color, body, regular,
	        len - blen - bpad, "");

	cute_free(body);
}

void
cute_cons_report_sumup(const struct cute_cons_report * report,
                       const struct cute_suite_run *   suite)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(report->ncols >= CUTE_CONS_NAME_COLS_MIN);
	cute_assert_intern(report->fill);
	cute_assert_intern((report->ncols + CUTE_CONS_REPORT_SUMUP_WIDTH) <=
	                   report->colnr);
	cute_run_assert_intern(&suite->super);

	int    ncols = report->colnr - CUTE_CONS_REPORT_SUMUP_WIDTH;
	char * hrule;

	fprintf(report->stdio,
	        "%s"
	        "%-*s"
	        " " CUTE_CONS_REPORT_STAT_HEAD
	        " " CUTE_CONS_REPORT_TIME_HEAD
	        " " CUTE_CONS_REPORT_PASS_HEAD
	        " " CUTE_CONS_REPORT_SKIP_HEAD
	        " " CUTE_CONS_REPORT_FAIL_HEAD
	        " " CUTE_CONS_REPORT_EXEC_HEAD
	        " " CUTE_CONS_REPORT_TOTAL_HEAD
	        "%s"
	        "\n",
	        report->term.bold,
	        ncols, CUTE_CONS_REPORT_NAME_HEAD,
	        report->term.regular);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	cute_run_foreach((struct cute_run *)suite,
	                 cute_cons_report_process_sumup,
	                 (void *)report);
#pragma GCC diagnostic pop

	hrule = cute_cons_report_create_hrule(CUTE_CONS_REPORT_TABLE_SYM,
	                                      report->colnr);
	fprintf(report->stdio, "%s\n", hrule);
	cute_free(hrule);

	cute_cons_report_suite_sumup(report, suite, true);


	cute_cons_report_results(report, suite);
}

void
cute_cons_report_on_init(struct cute_cons_report * report, int name_cols)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(name_cols);

	if (name_cols > report->ncols)
		report->ncols = name_cols;
}

void
cute_cons_report_on_head(struct cute_cons_report * report,
                         int                       colnr,
                         int                       fill_cols)
{
	cute_cons_report_assert_intern(report);
	cute_assert_intern(report->ncols >= CUTE_CONS_NAME_COLS_MIN);
	cute_assert_intern(fill_cols);
	cute_assert_intern(fill_cols < colnr);

	report->colnr = colnr;

	report->fill = cute_cons_report_create_hrule(CUTE_CONS_REPORT_FILL_SYM,
	                                             fill_cols);
}

static int
_cute_cons_report_release(struct cute_report * report)
{
	cute_cons_report_assert_intern((struct cute_cons_report *)report);

	struct cute_cons_report * rprt = (struct cute_cons_report *)report;
	FILE *                    stdio = rprt->stdio;

	cute_free(rprt->fill);
	cute_free(report);

	return cute_close_stdio(stdio);
}

int
cute_cons_report_setup(struct cute_cons_report *  report,
                       const struct cute_config * config,
                       cute_report_handle_fn *    handle)
{
	cute_assert_intern(report);
	cute_config_assert_intern(config);
	cute_assert_intern(handle);

	cute_term_setup(&report->term, stdout, config->tty);

	report->super.handle = handle;
	report->super.release = _cute_cons_report_release;
	report->stdio = stdout;
	report->colnr = 0;
	report->ncols = CUTE_CONS_NAME_COLS_MIN;
	report->fill = NULL;

	cute_cons_report_register(&report->super);

	return 0;
}

void
cute_cons_report_handle(enum cute_event         event,
                        enum cute_kind          kind,
                        const struct cute_run * run)
{
	if (cute_the_cons_report) {
		cute_report_assert_intern(cute_the_cons_report);

		cute_the_cons_report->handle(cute_the_cons_report,
		                             event,
		                             kind,
		                             run);
	}
}

void
cute_cons_report_register(struct cute_report * report)
{
	cute_assert_intern(!cute_the_cons_report);
	cute_report_assert_intern(report);

	cute_the_cons_report = report;
}

int
cute_cons_report_release(void)
{
	if (cute_the_cons_report) {
		cute_report_assert_intern(cute_the_cons_report);

		int ret;

		ret = cute_the_cons_report->release(cute_the_cons_report);

		cute_the_cons_report = NULL;

		return ret;
	}

	return 0;
}
