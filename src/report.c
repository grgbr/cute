#include "core.h"
#include <cute/cute.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

unsigned int         report_indent_depth;
const struct report *report_current;

void report_indent(unsigned int indent)
{
	while (indent--)
		printf("    ");
}

static void text_show_test(const struct cute_test *test)
{
	const struct cute_result *res = &test->result;

	report_indent(report_indent_depth);
	printf("%-28.28s", test->object.name);

	switch (res->state) {
	case CUTE_SKIPPED_STATE:
		printf("  skipped\n");
		break;

	case CUTE_SUCCESS_STATE:
		printf("  success\n");
		break;

	case CUTE_FAILURE_STATE:
		printf("  failure\n");
		report_indent(report_indent_depth + 1);
		printf("@%s:%s\n", res->file, res->line);
		report_indent(report_indent_depth + 1);
		printf("%s\n", res->reason);
		break;

	case CUTE_ERROR_STATE:
		printf("  error    %s\n", res->reason);
		if (res->console) {
			printf("<console>\n%s</console>\n", res->console);
		}
		break;

	default:
		assert(0);
	}
}

static void text_show_suite_begin(const struct cute_suite *suite)
{
	report_indent(report_indent_depth);

	printf("Running %s suite...\n", suite->object.name);

	report_indent_depth++;
}

static void text_show_suite_end(const struct cute_suite *suite)
{
	report_indent_depth--;
	report_indent(report_indent_depth);

	printf("Completed %s suite [total:%u, success:%u, failures:%u, "
	       "errors:%u, skipped:%u]\n",
	       suite->object.name,
	       suite->total_count, suite->success_count, suite->failure_count,
	       suite->error_count, suite->skipped_count);
}

static void text_show_footer(const struct cute_suite *suite __unused, int error)
{
	if (error)
		printf("\nInternal error: %s\n", strerror(-error));
}

static const struct report text_report = {
	.show_test        = text_show_test,
	.show_suite_begin = text_show_suite_begin,
	.show_suite_end   = text_show_suite_end,
	.show_footer      = text_show_footer,
};

void
cute_setup_text_report(void)
{
	report_indent_depth = 0;

	report_current = &text_report;
}
