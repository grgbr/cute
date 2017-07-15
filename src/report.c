#include "core.h"
#include "utils.h"
#include <cute/cute.h>
#include <string.h>
#include <stdio.h>

static unsigned int text_indent;

static void text_show_indent(unsigned int indent)
{
	while (indent--)
		putchar('\t');
}

static void text_show_test(const struct cute_test *test)
{
	const struct cute_result *res = &test->result;

	text_show_indent(text_indent);

	if (res->reason) {
		if (res->file && res->line)
			printf("%s:: %s @%s:%s\n", test->object.name, res->reason,
			       res->file, res->line);
		else
			printf("%s:: %s @%s:%u\n", test->object.name, res->reason,
			       test->file, test->line);
	}
	else
		printf("%s:: success\n", test->object.name);

	if (res->console) {
		text_show_indent(text_indent + 1);
		printf("%s", res->console);
	}
}

static void text_show_suite_begin(const struct cute_suite *suite)
{
	text_show_indent(text_indent);

	printf("Running \"%s\" suite (%u tests)\n",
	       suite->object.name, suite->tests_count);

	text_indent++;
}

static void text_show_suite_end(const struct cute_suite *suite __unused)
{
	text_indent--;
}

static void text_show_footer(int error)
{
	if (error)
		printf("internal error: %s\n", strerror(-error));
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
