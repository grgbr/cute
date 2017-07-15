#ifndef _CUTE_CORE_H
#define _CUTE_CORE_H

/******************************************************************************
 * Report handling
 ******************************************************************************/

struct cute_test;

typedef void (show_test_fn)(const struct cute_test *test);

struct cute_suite;

typedef void (show_suite_fn)(const struct cute_suite *suite);

struct report;

typedef void (show_footer_fn)(int error);

struct report {
	show_test_fn   *show_test;
	show_suite_fn  *show_suite_begin;
	show_suite_fn  *show_suite_end;
	show_footer_fn *show_footer;
};

extern const struct report *current_report;

/******************************************************************************
 * Run handling
 ******************************************************************************/

typedef void (expect_failed_fn)(const char *line,
                                const char *file,
                                const char *reason);

typedef int (spawn_test_fn)(struct cute_test *test);

typedef void (fini_run_fn)(void);

struct run {
	expect_failed_fn *expect_failed;
	spawn_test_fn    *spawn_test;
	fini_run_fn      *fini_run;
};

extern const struct run *current_run;

#endif
