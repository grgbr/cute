#include "core.h"
#include "utils.h"
#include <cute/cute.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static unsigned int text_indent;

static void text_show_indent(unsigned int indent)
{
	while (indent--)
		printf("    ");
}

static void text_show_test(const struct cute_test *test)
{
	const struct cute_result *res = &test->result;

	text_show_indent(text_indent);
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
		text_show_indent(text_indent + 1);
		printf("@%s:%s\n", res->file, res->line);
		text_show_indent(text_indent + 1);
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
	text_show_indent(text_indent);

	printf("Running %s suite...\n", suite->object.name);

	text_indent++;
}

static void text_show_suite_end(const struct cute_suite *suite)
{
	text_indent--;
	text_show_indent(text_indent);

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

const struct report text_report = {
	.show_test        = text_show_test,
	.show_suite_begin = text_show_suite_begin,
	.show_suite_end   = text_show_suite_end,
	.show_footer      = text_show_footer,
};

void
cute_setup_text_report(void)
{
	text_indent = 0;

	current_report = &text_report;
}
