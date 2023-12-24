/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cons.h"
#include "suite.h"
#include "iodir.h"
#include <ctype.h>
#include <string.h>

static struct cute_report * cute_the_cons_report;

void
cute_cons_report_mask(const struct cute_cons_report * report, sigset_t * old)
{
	if (!sigisemptyset(&report->mask)) {
		int err __cute_unused;

		err = pthread_sigmask(SIG_BLOCK, &report->mask, old);
		cute_assert_intern(!err);
	}
}

void
cute_cons_report_unmask(const struct cute_cons_report * report,
                        const sigset_t *                old)
{
	if (!sigisemptyset(&report->mask)) {
		int err __cute_unused;

		err = pthread_sigmask(SIG_SETMASK, old, NULL);
		cute_assert_intern(!err);
	}
}

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

	char *       hrule;
	size_t       max;
	char *       title;
	size_t       ttl_len;
	size_t       prt_len;
	int          pre_len;
	int          post_len;
	const char * bold = report->term.bold;
	const char * reg = report->term.regular;

	hrule = cute_cons_report_create_hrule(CUTE_CONS_REPORT_HRULE_SYM,
	                                      report->colnr / 2);

	max = (size_t)report->colnr -
	      (sizeof("###  Running ") - 1) -
	      (sizeof(" test(s)  ###") - 1);
	title = cute_malloc(max + 1);
	snprintf(title,
	         max,
	         "%s%s%s",
	         cute_package,
	         cute_package_version ? " " : "",
	         cute_package_version ? cute_package_version : "");

	ttl_len = strnlen(title, max);
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
	        bold, (int)ttl_len, title, reg,
	        post_len, hrule);

	cute_free(title);
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

	if ((run->issue == CUTE_FAIL_ISSUE) ||
	    (run->issue == CUTE_EXCP_ISSUE)) {
		cute_assert_intern(run->what);
		cute_assert_intern(run->why);
		struct cute_text_block * blk;

		fprintf(report->stdio,
		        "%s"
		        "issue:  %s\n"
		        "reason: %s\n",
		        report->term.gray,
		        run->what,
		        run->why);
		blk = cute_assess_desc(&run->assess);
		if (blk) {
			cute_report_printf_block(blk,
			                         0,
			                         report->stdio,
			                         cute_report_format_text);
			cute_text_destroy(blk);
		}

		if (cute_iodir_is_block_busy(&run->ioout)) {
			fprintf(report->stdio,
			        ">>> stdout >>>%s\n",
			        report->term.italic);
			cute_iodir_print_block(report->stdio,
			                       0,
			                       &run->ioout,
			                       cute_iodir_format_text);
			fprintf(report->stdio,
			        "%s%s<<< stdout <<<\n",
			        report->term.regular,
			        report->term.gray);
		}

		if (cute_iodir_is_block_busy(&run->ioerr)) {
			fprintf(report->stdio,
			        ">>> stderr >>>%s\n",
			        report->term.italic);
			cute_iodir_print_block(report->stdio,
			                       0,
			                       &run->ioerr,
			                       cute_iodir_format_text);
			fprintf(report->stdio,
			        "%s%s<<< stderr <<<\n",
			        report->term.regular,
			        report->term.gray);
		}

		fputs(report->term.regular, report->stdio);
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
	                       " %s" CUTE_CONS_REPORT_EXCP_FMT
	                       " %s" CUTE_CONS_REPORT_EXEC_FMT
	                       " " CUTE_CONS_REPORT_TOTAL_FMT
	                       "%s"
	                       "\n",
	                       bold,
	                       blue, width, width, name,
	                       color, cute_issue_label(issue), term->fore,
	                       diff.tv_sec, diff.tv_nsec / 1000L,
	                       green, stats->pass,
	                       stats->skip ? yellow : fore, stats->skip,
	                       stats->fail ? red : fore, stats->fail,
	                       stats->excp ? red : fore, stats->excp,
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
		        "%s" CUTE_CONS_REPORT_EXCP_FMT "%%"
		        "%s" CUTE_CONS_REPORT_EXEC_FMT "%%"
		        "%s"
		        "\n",
		        bold,
		        report->ncols, report->ncols, "",
		        green,
		        (stats->pass * 100) / stats->exec,
		        stats->skip ? yellow : fore,
		        (stats->skip * 100) / stats->exec,
		        stats->fail ? red : fore,
		        (stats->fail * 100) / stats->exec,
		        stats->excp ? red : fore,
		        (stats->excp * 100) / stats->exec,
		        fore, (stats->exec * 100) / stats->total,
		        term->regular);
	else
		fprintf(report->stdio,
		        "%s"
		        CUTE_CONS_REPORT_NAME_FMT
		        " " CUTE_CONS_REPORT_STAT_PAD
		        " " CUTE_CONS_REPORT_TIME_PAD
		        "%s" CUTE_CONS_REPORT_PASS_FMT "%%%s"
		        CUTE_CONS_REPORT_SKIP_FMT "%%"
		        CUTE_CONS_REPORT_FAIL_FMT "%%"
		        CUTE_CONS_REPORT_EXCP_FMT "%%"
		        CUTE_CONS_REPORT_EXEC_FMT "%%"
		        "%s"
		        "\n",
		        bold,
		        report->ncols, report->ncols, "",
		        green, 0, fore,
		        0,
		        0,
		        0,
		        0,
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
		blen = snprintf(body, sz, "All tests PASSED");
		break;

	case CUTE_SKIP_ISSUE:
		blen = snprintf(body, sz, "All tests SKIPPED");
		break;

	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		blen = snprintf(body,
		                sz,
		                "%u/%u (%u%%) test(s) FAILED",
		                suite->sums.fail + suite->sums.excp,
		                suite->sums.exec,
		                (suite->sums.fail + suite->sums.excp) * 100 /
		                suite->sums.exec);
		break;

	case CUTE_OFF_ISSUE:
		blen = snprintf(body, sz, "All tests DISABLED");
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
	        " " CUTE_CONS_REPORT_EXCP_HEAD
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

/*****************************************************************************/

struct cute_report_info {
	unsigned int suites;
	unsigned int tests;
};

static void
cute_cons_report_show_info(struct cute_run * run,
                           enum cute_visit   visit,
                           void *            data)
{
	cute_run_assert_intern(run);

	struct cute_report_info * info = data;

	switch (visit) {
	case CUTE_BEGIN_VISIT:
		info->suites++;
		break;

	case CUTE_ONCE_VISIT:
		info->tests++;
		break;

	case CUTE_END_VISIT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_cons_report_show_prop(const struct cute_prop * prop, FILE * stdio)
{
	size_t sz = prop->size;

	if (sz) {
		const char * str;

		cute_foreach_string(str, prop->data, sz)
			fprintf(stdio, "%s\n", str);
	}
}

void
cute_cons_report_on_info(const struct cute_cons_report * report,
                         const struct cute_run *         run,
                         enum cute_kind                  kind)
{
	cute_cons_report_assert_intern(report);
	cute_run_assert_intern(run);

	const char *            type = type;
	const char *            bold = report->term.bold;
	const char *            blue = report->term.blue;
	const char *            reg = report->term.regular;
	const char *            high;
	const char *            under;
	unsigned int            tmout = run->base->tmout;
	struct cute_report_info info = {
		.suites  = 0,
		.tests   = 0
	};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
	switch (kind) {
	case CUTE_TEST_KIND:
		type = "test";
		high = "";
		under = "";
		info.tests = 1;
		break;

	case CUTE_SUITE_KIND:
		type = "suite";
		high = blue;
		under = reg;
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_run_foreach((struct cute_run *)run,
		                 cute_cons_report_show_info,
		                 (void *)&info);
		break;
	}
#pragma GCC diagnostic pop

	fprintf(report->stdio,
	        "### %1$sIdentification%2$s ###\n\n"
	        "%1$sName%2$s:      %3$s%4$s%5$s\n"
	        "%1$sPackage%2$s:   %6$s\n"
	        "%1$sVersion%2$s:   %7$s\n"
	        "%1$sType%2$s:      %8$s\n",
	        bold, reg,
	        high, run->base->name, under,
	        cute_package,
	        cute_package_version,
	        type);
	if (run->parent)
		fprintf(report->stdio,
		        "%sParent%s:    %s%s%s\n",
		        bold, reg, blue, run->parent->base->name, reg);
	else
		fprintf(report->stdio, "%sParent%s:    none\n", bold, reg);
	fprintf(report->stdio,
	        "%1$sDepth%2$s:     %3$d\n"
	        "%1$sFull name%2$s: %4$s%5$s%6$s\n"
	        "%1$sSource%2$s:    %7$s:%8$d\n",
	        bold, reg, run->depth,
	        high, run->name, under,
	        run->base->file, run->base->line);

	fprintf(report->stdio,
	        "\n### %1$sProperties%2$s ###\n\n"
	        "%1$sSetup%2$s:     %3$s\n"
	        "%1$sTeardown%2$s:  %4$s\n",
	        bold, reg,
	        run->base->setup ? "yes" : "no",
	        run->base->teardown ? "yes" : "no");
	switch (tmout) {
	case CUTE_NONE_TMOUT:
		fprintf(report->stdio, "%sTimeout%s:   none\n", bold, reg);
		break;
	case CUTE_INHR_TMOUT:
		fprintf(report->stdio, "%sTimeout%s:   inherited\n", bold, reg);
		break;
	default:
		fprintf(report->stdio, "%sTimeout%s:   %u\n", bold, reg, tmout);
	}
	fprintf(report->stdio,
	        "%1$s#Suites%2$s:   %3$u\n"
	        "%1$s#Tests%2$s:    %4$u\n",
	        bold, reg, info.suites,
	        info.tests);

	fprintf(report->stdio,
	        "\n### %1$sRuntime%2$s ###\n\n"
	        "%1$sHostname%2$s:  %3$s\n"
	        "%1$sBuild ID%2$s:  %4$s\n",
	        bold, reg,
	        cute_hostname,
	        cute_build_id);

	fprintf(report->stdio,
	        "\n### %sBuild configuration%s ###\n\n",
	        bold, reg);
	cute_cons_report_show_prop(&cute_build_conf, report->stdio);

	fprintf(report->stdio, "\n### %sBuild flags%s ###\n\n", bold, reg);
	cute_cons_report_show_prop(&cute_build_tool, report->stdio);
	fputc('\n', report->stdio);
	cute_cons_report_show_prop(&cute_build_flags, report->stdio);
}

static void
cute_cons_report_show_run(struct cute_run * run,
                          enum cute_visit   visit,
                          void *            data)
{
	const struct cute_cons_report * rprt = (const struct cute_cons_report *)
	                                       data;

	switch (visit) {
	case CUTE_BEGIN_VISIT:
		if (run->state != CUTE_OFF_STATE)
			fprintf(rprt->stdio,
			        "%s%s%s\n",
			        rprt->term.blue,
			        run->name,
			        rprt->term.regular);
		break;

	case CUTE_ONCE_VISIT:
		if (run->state != CUTE_OFF_STATE)
			fprintf(rprt->stdio, "%s\n", run->name);
		break;

	case CUTE_END_VISIT:
		break;

	default:
		__cute_unreachable();
	}
}

void
cute_cons_report_on_show(const struct cute_cons_report * report,
                         const struct cute_run *         run)
{
	cute_cons_report_assert_intern(report);
	cute_run_assert_intern(run);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	cute_run_foreach((struct cute_run *)run,
	                 cute_cons_report_show_run,
	                 (void *)report);
#pragma GCC diagnostic pop
}

/*****************************************************************************/

static int
_cute_cons_report_release(struct cute_report * report)
{
	cute_cons_report_assert_intern((struct cute_cons_report *)report);

	struct cute_cons_report * rprt = (struct cute_cons_report *)report;
	FILE *                    stdio = rprt->stdio;

	cute_free(rprt->fill);
	cute_free(report);

	fflush(stdio);

	return 0;
}

#define CUTE_TERM_RED_COLOR     "\e[91m"
#define CUTE_TERM_GREEN_COLOR   "\e[32m"
#define CUTE_TERM_YELLOW_COLOR  "\e[33m"
#define CUTE_TERM_BLUE_COLOR    "\e[34m"
#define CUTE_TERM_GRAY_COLOR    "\e[37m"
#define CUTE_TERM_FORE_COLOR    "\e[39m"

#define CUTE_TERM_REGULAR       "\e[0m"
#define CUTE_TERM_BOLD_GRAPH    "\e[1m"
#define CUTE_TERM_ITALIC_GRAPH  "\e[3m"

static void
cute_cons_setup_term(struct cute_cons_report * report,
                     FILE *                    stdio,
                     enum cute_config_tty      config)
{
	cute_assert_intern(report);
	cute_assert_intern(stdio);

	bool               tty;
	int                ret;
	struct cute_term * term = &report->term;
	sigset_t *         msk = &report->mask;

	switch (config) {
	case CUTE_CONFIG_TRUE_TTY:
		tty = true;
		break;

	case CUTE_CONFIG_FALSE_TTY:
		tty = false;
		break;

	case CUTE_CONFIG_PROBE_TTY:
		ret = isatty(fileno(stdio));
		if (ret >= 0)
			tty = !!ret;
		else
			tty = false;
		break;

	default:
		__cute_unreachable();
	}

	ret = sigemptyset(msk);
	cute_assert_intern(!ret);

	if (tty) {
		term->regular = CUTE_TERM_REGULAR;
		term->bold = CUTE_TERM_BOLD_GRAPH;
		term->italic = CUTE_TERM_ITALIC_GRAPH;
		term->red = CUTE_TERM_RED_COLOR;
		term->green = CUTE_TERM_GREEN_COLOR;
		term->yellow = CUTE_TERM_YELLOW_COLOR;
		term->blue = CUTE_TERM_BLUE_COLOR;
		term->gray = CUTE_TERM_GRAY_COLOR;
		term->fore = CUTE_TERM_FORE_COLOR;

		ret = sigaddset(msk, SIGHUP);
		cute_assert_intern(!ret);
		ret = sigaddset(msk, SIGINT);
		cute_assert_intern(!ret);
		ret = sigaddset(msk, SIGQUIT);
		cute_assert_intern(!ret);
		ret = sigaddset(msk, SIGTSTP);
		cute_assert_intern(!ret);
		ret = sigaddset(msk, SIGTERM);
		cute_assert_intern(!ret);
	}
	else {
		term->regular = "";
		term->bold = "";
		term->italic = "";
		term->red = "";
		term->green = "";
		term->yellow = "";
		term->blue = "";
		term->gray = "";
		term->fore = "";
	}
}

int
cute_cons_report_setup(struct cute_cons_report *  report,
                       const struct cute_config * config,
                       cute_report_handle_fn *    handle)
{
	cute_assert_intern(report);
	cute_config_assert_intern(config);
	cute_assert_intern(handle);

	report->super.handle = handle;
	report->super.release = _cute_cons_report_release;
	report->stdio = cute_iodir_stdout;
	report->colnr = 0;
	report->ncols = CUTE_CONS_NAME_COLS_MIN;
	report->fill = NULL;

	cute_cons_setup_term(report, cute_iodir_stdout, config->tty);

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
