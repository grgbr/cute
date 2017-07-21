#include "core.h"
#include <cute/cute.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

bool                 report_isatty = false;
int                  report_indent_depth;
const struct report *report_current;

#define TEXT_REGULAR_SGR    (0U)
#define TEXT_BOLD_SGR       (1U)

#define TEXT_BLACK_SGR      (30U)
#define TEXT_RED_SGR        (31U)
#define TEXT_GREEN_SGR      (32U)
#define TEXT_BROWN_SGR      (33U)
#define TEXT_BLUE_SGR       (34U)
#define TEXT_MAGENTA_SGR    (35U)
#define TEXT_CYAN_SGR       (36U)
#define TEXT_WHITE_SGR      (37U)

/*
 * Set Graphics rendition (man console_codes).
 */
static void text_set_gr(unsigned int attribute, unsigned int color)
{
	/* Disable colored output if current terminal is not a TTY. */
	if (report_isatty)
		printf("\033[%02u;%02um", attribute, color);
}

/*
 * Reset graphics rendition to default.
 */
static void text_clear_gr(void)
{
	/* Disable colored output if current terminal is not a TTY. */
	if (report_isatty)
		fputs("\033[00m", stdout);
}

static void text_suite_gr(void)
{
	text_set_gr(TEXT_BOLD_SGR, TEXT_MAGENTA_SGR);
}

static void text_skipped_gr(void)
{
	text_set_gr(TEXT_REGULAR_SGR, TEXT_BLUE_SGR);
}

static void text_success_gr(void)
{
	text_set_gr(TEXT_REGULAR_SGR, TEXT_GREEN_SGR);
}

static void text_failure_gr(void)
{
	text_set_gr(TEXT_REGULAR_SGR, TEXT_BROWN_SGR);
}

static void text_error_gr(void)
{
	text_set_gr(TEXT_REGULAR_SGR, TEXT_RED_SGR);
}

static void text_details_gr(void)
{
	text_set_gr(TEXT_REGULAR_SGR, TEXT_WHITE_SGR);
}

static void text_title_gr(void)
{
	text_set_gr(TEXT_BOLD_SGR, TEXT_WHITE_SGR);
}

static void text_show_test(const struct cute_test *test)
{
	const struct cute_result *res = &test->result;

	switch (res->state) {
	case CUTE_SKIPPED_STATE:
		text_skipped_gr();
		text_start_line(report_indent_depth, "K");

		text_clear_gr();
		printf(". %-2.3f  %s\n", 0.0, test->object.name);
		break;

	case CUTE_SUCCESS_STATE:
		text_success_gr();
		text_start_line(report_indent_depth, "S");

		text_clear_gr();
		printf(". %-2.3f  %s\n", 0.0, test->object.name);
		break;

	case CUTE_FAILURE_STATE:
		text_failure_gr();
		text_start_line(report_indent_depth, "F. %-2.3f  %s\n",
		                0.0, test->object.name);

		text_clear_gr();
		text_start_line(report_indent_depth + 1, "@");

		text_details_gr();
		fputs(res->file, stdout);

		text_clear_gr();
		putchar(':');

		text_details_gr();
		puts(res->line);

		text_start_line(report_indent_depth + 1, "%s\n", res->reason);
		text_clear_gr();
		break;

	case CUTE_ERROR_STATE:
		text_error_gr();
		text_start_line(report_indent_depth, "E. %-2.3f  %s\n",
		                0.0, test->object.name);

		if (res->console) {
			const char *begin;
			const char *end;

			text_details_gr();

			begin = res->console;
			assert(*begin);
			do {
				end = index(begin, '\n');
				if (!end) {
					text_start_line(report_indent_depth + 1,
					                "%s\n", begin);
					break;
				}

				text_start_line(report_indent_depth + 1, "%*s",
				                (int)((unsigned long)end -
				                      (unsigned long)begin),
				                begin);
				begin = end + 1;
			} while (*begin);
		}

		text_clear_gr();
		break;

	default:
		assert(0);
	}
}

static void text_show_suite_begin(const struct cute_suite *suite)
{
	report_indent_depth++;

	text_suite_gr();
	text_start_line(report_indent_depth, "%s", suite->object.name);

	text_clear_gr();
	puts(" suite");
}

static void text_show_suite_end(const struct cute_suite *suite)
{
	text_suite_gr();
	text_start_line(report_indent_depth, "#");

	text_clear_gr();
	printf(". %-2.3f  total:", 0.0);
	if (suite->total_count != suite->success_count) {
		text_suite_gr();
		printf("%u  ", suite->total_count);

		text_clear_gr();
		fputs("Success:", stdout);

		text_success_gr();
		printf("%u  ", suite->success_count);

		text_clear_gr();
	}
	else
		printf("%u  Success:%u  ",
		       suite->total_count, suite->success_count);

	printf("Failure:");
	if (suite->failure_count)
		text_failure_gr();
	printf("%u  ", suite->failure_count);
	if (suite->failure_count)
		text_clear_gr();

	printf("Error:");
	if (suite->error_count)
		text_error_gr();
	printf("%u  ", suite->error_count);
	if (suite->error_count)
		text_clear_gr();

	printf("sKipped:");
	if (suite->skipped_count)
		text_skipped_gr();
	printf("%u\n", suite->skipped_count);
	if (suite->skipped_count)
		text_clear_gr();

	report_indent_depth--;
}

static void text_show_footer(const struct cute_suite *suite, int error)
{
	if (error) {
		printf("\nInternal error: %s\n", strerror(-error));
		return;
	}

	if (suite) {
		text_suite_gr();
		fputs("\n======== ", stdout);
		text_title_gr();
		fputs(suite->object.name, stdout);
		text_suite_gr();
		fputs(" summary (", stdout);
		text_title_gr();
		printf("%.3f", 0.0);
		text_suite_gr();
		fputs(" ms) ========\n", stdout);
		text_clear_gr();

		fputs("Error  :  ", stdout);
		if (suite->error_count)
			text_error_gr();
		printf("%u\n", suite->error_count);
		if (suite->error_count)
			text_clear_gr();

		fputs("Failure:  ", stdout);
		if (suite->failure_count)
			text_failure_gr();
		printf("%u\n", suite->failure_count);
		if (suite->failure_count)
			text_clear_gr();

		fputs("sKipped:  ", stdout);
		if (suite->skipped_count)
			text_skipped_gr();
		printf("%u\n", suite->skipped_count);
		if (suite->skipped_count)
			text_clear_gr();

		fputs("Success:  ", stdout);
		text_success_gr();
		printf("%u\n", suite->success_count);

		text_clear_gr();
		printf("Total  :  %u\n\n", suite->total_count);
	}
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
	report_isatty = !!isatty(STDOUT_FILENO);

	report_indent_depth = -1;

	report_current = &text_report;
}
