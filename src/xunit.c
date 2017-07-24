#include "core.h"
#include <cute/cute.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <assert.h>

static char        xunit_hostname[HOST_NAME_MAX];
static const char *xunit_package_name;

static const char * xunit_timestamp(void)
{
	static char stamp[] = "YYYY-MM-DDThh:mm:ss";
	time_t      now = time(NULL);

	strftime(stamp, sizeof(stamp), "%FT%T", gmtime(&now));

	return stamp;
}

static void xunit_show_test(const struct cute_object *object)
{
	const struct cute_test   *test = (struct cute_test *)object;
	const struct cute_result *res = &test->result;
	const char               *stat;

	switch (res->state) {
	case CUTE_SKIPPED_STATE:
		stat = "skipped";
		break;

	case CUTE_SUCCESS_STATE:
		stat = "success";
		break;

	case CUTE_FAILURE_STATE:
		stat = "failure";
		break;

	case CUTE_ERROR_STATE:
		stat = "error";
		break;

	default:
		assert(0);
	}

	text_start_line(report_indent_depth + 1,
			"<testcase name='%s' status='%s' file='%s' "
			"line='%d' time='%f'",
			object->name, stat, test->file, test->line,
			object->usecs / 1000000.0);

	switch (res->state) {
	case CUTE_SKIPPED_STATE:
	case CUTE_SUCCESS_STATE:
		printf("/>\n");
		return;

	case CUTE_FAILURE_STATE:
		printf(">\n");
		text_start_line(report_indent_depth + 2,
				"<failure message='failed at %s:%s'>\n",
				res->file, res->line);
		text_start_line(report_indent_depth + 3,
				"%s\n", res->reason);
		text_start_line(report_indent_depth + 2,
				"</failure>\n");
		break;

	case CUTE_ERROR_STATE:
		printf(">\n");
		text_start_line(report_indent_depth + 2,
				"<error message='%s'>\n", res->reason);
		printf("%s", res->console);
		text_start_line(report_indent_depth + 2, "</error>\n");
		break;

	default:
		assert(0);
	}

	text_start_line(report_indent_depth + 1, "</testcase>\n");
}

static void xunit_show_object(const struct cute_object *object)
{
	const struct cute_object *obj = object->eldest;

	if (!obj)
		return xunit_show_test(object);

	report_indent_depth++;

	text_start_line(report_indent_depth,
	                "<testsuite name='%s' tests='%u' failures='%u' "
	                "errors='%u' skipped='%u' "
	                "timestamp='%s' time='%f' "
	                "package='%s' hostname='%s'>\n",
	                object->name,
	                ((struct cute_suite *)object)->total_count,
	                ((struct cute_suite *)object)->failure_count,
	                ((struct cute_suite *)object)->error_count,
	                ((struct cute_suite *)object)->skipped_count,
	                xunit_timestamp(), object->usecs / 1000000.0,
	                xunit_package_name, xunit_hostname);

	do {
		xunit_show_object(obj);

		obj = obj->next;
	} while (obj);

	text_start_line(report_indent_depth, "</testsuite>\n");

	report_indent_depth--;
}

static void xunit_show_footer(const struct cute_suite *suite, int error)
{
	const struct cute_object *obj = suite->object.eldest;

	if (error) {
		printf("\nInternal error: %s\n", strerror(-error));
		return;
	}

	report_indent_depth = 0;

	xunit_package_name = suite->object.name;

	printf("<?xml version='1.0'?>\n<testsuites>\n");

	while (obj) {
		xunit_show_object(obj);

		obj = obj->next;
	}

	printf("</testsuites>\n");
}

static const struct report xunit_report = {
	.show_footer = xunit_show_footer,
};

void
cute_setup_xunit_report(void)
{
	gethostname(xunit_hostname, sizeof(xunit_hostname));
	xunit_hostname[sizeof(xunit_hostname) - 1] = '\0';

	report_current = &xunit_report;
}
