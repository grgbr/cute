/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cons.h"
#include "suite.h"
#include <langinfo.h>
#include <string.h>
#include <stdlib.h>

struct cute_xml_report {
	struct cute_report super;
	FILE *             stdio;
};

#define cute_xml_report_assert(_report) \
	cute_assert(_report); \
	cute_report_assert(&(_report)->super); \
	cute_assert((_report)->stdio)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_xml_report_assert_intern(_report) \
	cute_xml_report_assert(_report)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_xml_report_assert_intern(_report)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static bool
cute_xml_escape_sequence(FILE * stdio, const char * string, const char * sym)
{
	cute_assert_intern(stdio);
	cute_assert_intern(string);
	cute_assert_intern(sym);

	const char * seq;

	switch (*sym) {
	case '"':
		seq = "&quot;";
		break;
	case '\'':
		seq = "&apos;";
		break;
	case '<':
		seq = "&lt;";
		break;
	case '>':
		seq = "&gt;";
		break;
	case '&':
		seq = "&amp;";
		break;
	default:
		return false;
	}

	fprintf(stdio, "%.*s%s", (int)(sym - string), string, seq);

	return true;
}

static void
cute_xml_format_iodir(FILE *       stdio,
                      int          indent,
                      size_t       length,
                      const char * string)
{
	cute_assert_intern(stdio);
	cute_assert_intern(string);

	const char * const end = &string[length];
	const char *       sym = string;

	fprintf(stdio, "%*s", indent, "");

	while (sym < end) {
		if (!cute_xml_escape_sequence(stdio, string, sym)) {
			sym++;
			continue;
		}

		string = ++sym;
	}

	fprintf(stdio, "%.*s\n", (int)(end - string), string);
}

static void
cute_xml_report_prolog(const struct cute_xml_report * report)
{
	const char * codeset;

	codeset = nl_langinfo(CODESET);
	fprintf(report->stdio,
	        "<?xml version=\"1.0\" encoding=\"%s\"?>\n\n",
	        codeset);
}

static void
cute_xml_report_stdio(FILE *                  stdio,
                      int                     depth,
                      const struct cute_run * run)
{
	cute_assert_intern(stdio);
	cute_assert_intern(run);

	if (cute_iodir_is_block_busy(&run->ioout)) {
		fprintf(stdio, "%*s<system-out>\n", depth, "");
		cute_iodir_print_block(stdio,
		                       0,
		                       &run->ioout,
		                       cute_xml_format_iodir);
		fprintf(stdio, "%*s</system-out>\n", depth, "");
	}

	if (cute_iodir_is_block_busy(&run->ioerr)) {
		fprintf(stdio, "%*s<system-err>\n", depth, "");
		cute_iodir_print_block(stdio,
		                       0,
		                       &run->ioerr,
		                       cute_xml_format_iodir);
		fprintf(stdio, "%*s</system-err>\n", depth, "");
	}
}

static void
cute_xml_format_text(FILE * stdio, int indent, const char * string)
{
	cute_assert_intern(stdio);
	cute_assert_intern(string);

	const char * sym = string;

	fprintf(stdio, "%*s", indent, "");

	while (*sym != '\0') {
		if (!cute_xml_escape_sequence(stdio, string, sym)) {
			sym++;
			continue;
		}

		string = ++sym;
	}

	fprintf(stdio, "%s\n", string);
}

static void
cute_xml_report_testcase_details(FILE *                  stdio,
                                 int                     depth,
                                 const char *            label,
                                 const struct cute_run * run)
{
	cute_assert_intern(stdio);
	cute_assert_intern(label);
	cute_assert_intern(label[0]);
	cute_assert_intern(run->what);
	cute_assert_intern(run->why);

	struct cute_text_block * blk;

	fprintf(stdio,
	        ">\n"
	        "%*s<%s message=\"%s\">\n"
	        "reason: %s\n",
	        depth + 4, "", label, run->what,
	        run->why);

	blk = cute_assess_desc(&run->assess);
	if (blk) {
		cute_report_printf_block(blk,
		                         0,
		                         stdio,
		                         cute_xml_format_text);
		cute_text_destroy(blk);
	}

	fprintf(stdio, "%*s</%s>\n", depth + 4, "", label);

	cute_xml_report_stdio(stdio, depth + 4, run);
}

static void
cute_xml_report_testcase(const struct cute_xml_report * report,
                         const struct cute_run *        run)
{
	int             depth = run->depth * 4;
	const char *    status = NULL;
	struct timespec diff;
	char            str[CUTE_TIME_STRING_SZ];
	const char *    tstamp;

	cute_diff_tspec(&diff, &run->begin, &run->end);
	tstamp = cute_time_string(str, &run->begin);

	switch (run->issue) {
	case CUTE_PASS_ISSUE:
		status = "passed";
		break;

	case CUTE_SKIP_ISSUE:
		status = "skipped";
		break;

	case CUTE_FAIL_ISSUE:
		status = "failure";
		break;

	case CUTE_EXCP_ISSUE:
		status = "error";
		break;

	case CUTE_OFF_ISSUE:
		status = "disabled";
		break;

	default:
		__cute_unreachable();
	}

	fprintf(report->stdio,
	        "%2$*1$s<testcase name=\"%3$s\"\n"
	        "%2$*1$s          classname=\"%4$s\"\n"
	        "%2$*1$s          status=\"%5$s\"\n",
	        depth, "",
	        run->base->name,
	        run->parent->name,
	        status);

	fprintf(report->stdio,
	        "%*s          time=\"%ld.%06ld\"\n",
	        depth, "",
	        diff.tv_sec, diff.tv_nsec / 1000L);

	if (tstamp)
		fprintf(report->stdio,
		        "%*s          timestamp=\"%s+00:00\"\n",
		        depth, "",
		        tstamp);

	fprintf(report->stdio,
	        "%2$*1$s          file=\"%3$s\"\n"
	        "%2$*1$s          line=\"%4$d\"",
	        depth, "",
	        run->base->file,
	        run->base->line);

	switch (run->issue) {
	case CUTE_PASS_ISSUE:
		if (cute_iodir_is_block_busy(&run->ioout) ||
		    cute_iodir_is_block_busy(&run->ioerr)) {
			fputs(">\n", report->stdio);
			cute_xml_report_stdio(report->stdio, depth + 4, run);
			break;
		}
		__attribute__ ((fallthrough));

	case CUTE_OFF_ISSUE:
		fputs(" />\n", report->stdio);
		return;

	case CUTE_SKIP_ISSUE:
	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		cute_xml_report_testcase_details(report->stdio,
		                                 depth,
		                                 status,
		                                 run);
		break;

	default:
		break;
	}

	fprintf(report->stdio, "%*s</testcase>\n", depth, "");
}

static void
cute_xml_report_build_id(const struct cute_xml_report * report, int depth)
{
	if (cute_build_id)
		fprintf(report->stdio,
		        "%2$*1$s<property name=\"build-id\"\n"
		        "%2$*1$s          value=\"%3$s\" />\n",
		        depth, "",
		        cute_build_id);
}

static void
cute_xml_report_prop(const struct cute_xml_report * report,
                     const char *                   name,
                     const struct cute_prop *       prop,
                     int                            depth)
{
	size_t sz = prop->size;

	if (sz) {
		const char * str;

		fprintf(report->stdio,
			"%*s<property name=\"%s\">\n",
			depth, "", name);

		cute_foreach_string(str, prop->data, sz)
			fprintf(report->stdio, "%s\n", str);

		fprintf(report->stdio, "%*s</property>\n", depth, "");
	}
}

static void
cute_xml_report_begin_testsuite(const struct cute_xml_report * report,
                                const struct cute_suite_run *  suite)
{
	int             depth = suite->super.depth * 4;
	struct timespec diff;
	char            str[CUTE_TIME_STRING_SZ];
	const char *    tstamp;

	cute_diff_tspec(&diff, &suite->super.begin, &suite->super.end);

	fprintf(report->stdio,
	        "%2$*1$s<testsuite name=\"%3$s\"\n"
	        "%2$*1$s           failures=\"%4$u\"\n"
	        "%2$*1$s           errors=\"%5$u\"\n"
	        "%2$*1$s           skipped=\"%6$u\"\n"
	        "%2$*1$s           tests=\"%7$u\"\n"
	        "%2$*1$s           disabled=\"%8$u\"\n"
	        "%2$*1$s           time=\"%9$ld.%10$06ld\"\n"
	        "%2$*1$s           package=\"%11$s\"\n"
	        "%2$*1$s           classname=\"%12$s\"\n",
	        depth, "", suite->super.base->name,
	        suite->sums.fail,
	        suite->sums.excp,
	        suite->sums.skip,
	        suite->sums.total,
	        suite->sums.total - suite->sums.exec,
	        diff.tv_sec, diff.tv_nsec / 1000L,
	        cute_package,
	        suite->super.parent ? suite->super.parent->name : cute_package);

	if (cute_package_version)
		fprintf(report->stdio,
		        "%*s           version=\"%s\"\n",
		        depth, "",
		        cute_package_version);

	if (suite->super.id >= 0)
		fprintf(report->stdio,
		        "%*s           id=\"%u\"\n",
		        depth, "",
		        suite->super.id);

	tstamp = cute_time_string(str, &suite->super.begin);
	if (tstamp)
		fprintf(report->stdio,
		        "%*s           timestamp=\"%s+00:00\"\n",
		        depth, "",
		        tstamp);

	fprintf(report->stdio,
	        "%2$*1$s           hostname=\"%3$s\"\n"
	        "%2$*1$s           file=\"%4$s\"\n"
	        "%2$*1$s           line=\"%5$d\">\n",
	        depth, "",
	        cute_hostname,
	        suite->super.base->file,
	        suite->super.base->line);

	if (!depth) {
		fprintf(report->stdio, "%*s<properties>\n", depth + 4, "");
		cute_xml_report_build_id(report, depth + 8);
		cute_xml_report_prop(report,
		                     "build-tool",
		                     &cute_build_tool,
		                     depth + 8);
		cute_xml_report_prop(report,
		                     "build-flags",
		                     &cute_build_flags,
		                     depth + 8);
		cute_xml_report_prop(report,
		                     "build-config",
		                     &cute_build_conf,
		                     depth + 8);
		fprintf(report->stdio, "%*s</properties>\n", depth + 4, "");
	}
}

static void
cute_xml_report_end_testsuite(const struct cute_xml_report * report,
                              const struct cute_suite_run *  suite)
{
	fprintf(report->stdio, "%*s</testsuite>\n", suite->super.depth * 4, "");
}

static void
cute_xml_report_process_run(struct cute_run * run,
                            enum cute_visit   visit,
                            void *            data)
{
	switch (visit) {
	case CUTE_BEGIN_VISIT:
		cute_xml_report_begin_testsuite(
			(const struct cute_xml_report *)data,
			(const struct cute_suite_run *)run);
		break;

	case CUTE_END_VISIT:
		cute_xml_report_end_testsuite(
			(const struct cute_xml_report *)data,
			(const struct cute_suite_run *)run);
			break;

	case CUTE_ONCE_VISIT:
		cute_xml_report_testcase(
			(const struct cute_xml_report *)data,
			run);
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_xml_report_on_foot(const struct cute_xml_report * report,
                        const struct cute_suite_run *  suite)
{
	cute_xml_report_assert_intern(report);
	cute_run_assert_intern(&suite->super);


	cute_xml_report_prolog(report);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	cute_run_foreach((struct cute_run *)suite,
	                 cute_xml_report_process_run,
	                 (void *)report);
#pragma GCC diagnostic pop
}

static void
cute_xml_report_suite(struct cute_xml_report *      report,
                      enum cute_event               event,
                      const struct cute_suite_run * suite)
{
	switch (event) {

	case CUTE_FOOT_EVT:
		cute_xml_report_on_foot(report, suite);
		break;

	case CUTE_INIT_EVT:
	case CUTE_HEAD_EVT:
	case CUTE_SETUP_EVT:
	case CUTE_EXEC_EVT:
	case CUTE_TEARDOWN_EVT:
	case CUTE_DONE_EVT:
	case CUTE_INFO_EVT:
	case CUTE_SHOW_EVT:
	case CUTE_FINI_EVT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_xml_report_handle(struct cute_report *    report,
                       enum cute_event         event,
                       enum cute_kind          kind,
                       const struct cute_run * run)
{
	cute_xml_report_assert_intern((struct cute_xml_report *)report);
	cute_run_assert_intern(run);

	switch (kind) {
	case CUTE_TEST_KIND:
		break;

	case CUTE_SUITE_KIND:
		cute_xml_report_suite((struct cute_xml_report *)report,
		                      event,
		                      (const struct cute_suite_run *)run);
		break;

	default:
		__cute_unreachable();
	}
}

static int
cute_xml_report_release(struct cute_report * report)
{
	cute_xml_report_assert_intern((struct cute_xml_report *)report);

	struct cute_xml_report * rprt = (struct cute_xml_report *)report;
	FILE *                   stdio = rprt->stdio;

	cute_free(report);

	if (!cute_the_config->xml_path) {
		fflush(stdio);
		return 0;
	}
	else
		return cute_close_stdio(stdio);
}

int
cute_report_setup_xml(const struct cute_config * config)
{
	cute_config_assert_intern(config);

	struct cute_xml_report * rprt;
	int                      ret;

	rprt = cute_malloc(sizeof(*rprt));

	rprt->super.handle = cute_xml_report_handle;
	rprt->super.release = cute_xml_report_release;

	if (config->xml_path) {
		rprt->stdio = fopen(config->xml_path, "w");
		if (!rprt->stdio) {
			ret = -errno;
			cute_error("'%s': cannot open output file: %s.\n",
			           config->xml_path,
			           strerror(errno));
			goto free;
		}

		cute_report_register(&rprt->super);
	}
	else {
		rprt->stdio = cute_iodir_stdout;
		cute_cons_report_register(&rprt->super);
	}

	return 0;

free:
	cute_free(rprt);

	return ret;
}
