#ifndef _CUTE_H
#define _CUTE_H

#include <stddef.h>

#define _CUTE_STR(_symbol) # _symbol
#define CUTE_STR(_symbol)  _CUTE_STR(_symbol)

struct cute_object {
	const char         *name;
	struct cute_object *next;
	struct cute_object *parent;
	struct cute_object *eldest;
	struct cute_object *youngest;
};

#define CUTE_INIT_OBJECT(_name)              \
	{                                    \
		.name     = CUTE_STR(_name), \
		.parent   = NULL,            \
		.next     = NULL,            \
		.eldest   = NULL,            \
		.youngest = NULL,            \
	}

/******************************************************************************
 * Suite definition
 ******************************************************************************/

typedef void (cute_fixture_fn)(void);

struct cute_suite {
	struct cute_object  object;
	cute_fixture_fn    *setup;
	cute_fixture_fn    *teardown;
	unsigned int        tests_count;
};

#define CUTE_INIT_SUITE(_suite_name, _setup, _teardown)       \
	{                                                     \
		.object      = CUTE_INIT_OBJECT(_suite_name), \
		.setup       = _setup,                        \
		.teardown    = _teardown,                     \
		.tests_count = 0,                             \
	}

#define CUTE_SUITE(_suite_name, _setup, _teardown)                           \
	struct cute_suite _suite_name = CUTE_INIT_SUITE(_suite_name, _setup, \
	                                                _teardown)

extern int cute_register_suite(struct cute_suite *parent,
                               struct cute_suite *suite);

/******************************************************************************
 * Test definition
 ******************************************************************************/

struct cute_result {
	char *reason;
	char *line;
	char *file;
	char *console;
};

#define CUTE_INIT_RESULT()       \
	{                        \
		.reason  = NULL, \
		.line    = NULL, \
		.file    = NULL, \
		.console = NULL, \
	}

typedef void (cute_test_fn)(void);

struct cute_test {
	struct cute_object  object;
	const char         *file;
	int                 line;
	unsigned int        timeout;
	cute_test_fn       *run;
	struct cute_result  result;
};

#define CUTE_INIT_TEST(_test_name, _test_function)           \
	{                                                    \
		.object  = CUTE_INIT_OBJECT(_test_name),     \
		.file    = __FILE__,                         \
		.line    = __LINE__,                         \
		.timeout = CUTE_DEFAULT_TIMEOUT,             \
		.run     = _test_function,                   \
		.result  = CUTE_INIT_RESULT()                \
	}

#define CUTE_TEST(_test_name)                                             \
	static void _test_name ## __cute_test_fn(void);                   \
	static struct cute_test _test_name =                              \
		CUTE_INIT_TEST(_test_name, _test_name ## __cute_test_fn); \
	static void _test_name ## __cute_test_fn(void)

extern void cute_register_test(struct cute_suite *suite,
                               struct cute_test  *test);

/******************************************************************************
 * Assertion
 ******************************************************************************/

extern void cute_expect_failed(const char *line,
                               const char *file,
                               const char *reason);

#define cute_ensure(_expect)                                   \
	({                                                     \
		if (!(_expect))                                \
			cute_expect_failed(CUTE_STR(__LINE__), \
			                   __FILE__,           \
			                   CUTE_STR(_expect)); \
	 })

/******************************************************************************
 * Reporting
 ******************************************************************************/

extern void cute_setup_text_report(void);

/******************************************************************************
 * Running
 ******************************************************************************/

#define CUTE_DEFAULT_TIMEOUT (0U)

extern int cute_setup_posix_run(unsigned int default_timeout);

extern int cute_run_test(struct cute_test *test);

extern int cute_run_suite(struct cute_suite *suite);

extern void cute_fini(void);

#endif
