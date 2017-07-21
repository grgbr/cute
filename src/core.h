#ifndef _CUTE_CORE_H
#define _CUTE_CORE_H

#include <stdio.h>

/******************************************************************************
 * Reporting
 ******************************************************************************/

struct cute_test;

typedef void (report_show_test_fn)(const struct cute_test *test);

struct cute_suite;

typedef void (report_show_suite_fn)(const struct cute_suite *suite);

struct report;

typedef void (report_show_footer_fn)(const struct cute_suite *suite, int error);

struct report {
	report_show_test_fn   *show_test;
	report_show_suite_fn  *show_suite_begin;
	report_show_suite_fn  *show_suite_end;
	report_show_footer_fn *show_footer;
};

extern const struct report *report_current;

extern int report_indent_depth;

#define TEXT_INDENT_CHAR_NR (4)

#define text_start_line(_indent, _format, ...)                     \
	printf("%*s" _format, TEXT_INDENT_CHAR_NR * (_indent), "", \
	       ## __VA_ARGS__)

/******************************************************************************
 * Running
 ******************************************************************************/

typedef void (core_expect_failed_fn)(const char *line,
                                     const char *file,
                                     const char *reason);

typedef int (core_spawn_test_fn)(struct cute_test *test);

typedef void (core_fini_run_fn)(void);

struct core_run {
	core_expect_failed_fn *expect_failed;
	core_spawn_test_fn    *spawn_test;
	core_fini_run_fn      *fini_run;
};

extern const struct core_run *core_current_run;

#endif
