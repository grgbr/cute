#ifndef _CUTE_H
#define _CUTE_H

#include <cute/utils.h>
#include <stddef.h>

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
 * Base testing object
 ******************************************************************************/

#define CUTE_ROOT_OBJECT (&cute_root_suite.object);

struct cute_object {
	const char         *name;
	struct cute_object *next;
	struct cute_object *parent;
	struct cute_object *eldest;
	struct cute_object *youngest;
};

#define CUTE_INIT_OBJECT(_name, _parent)     \
	{                                    \
		.name     = CUTE_STR(_name), \
		.parent   = _parent,         \
		.next     = NULL,            \
		.eldest   = NULL,            \
		.youngest = NULL,            \
	}

extern int cute_run(struct cute_object *object);

extern void cute_list(const struct cute_object *object);

extern struct cute_object * cute_find(struct cute_object *object,
                                      const char         *name);

/******************************************************************************
 * Suite definition
 ******************************************************************************/

typedef void (cute_fixture_fn)(void);

struct cute_suite {
	struct cute_object  object;
	cute_fixture_fn    *setup;
	cute_fixture_fn    *teardown;
	unsigned int        total_count;
	unsigned int        success_count;
	unsigned int        failure_count;
	unsigned int        error_count;
	unsigned int        skipped_count;
};

extern struct cute_suite cute_root_suite;

#define CUTE_INIT_SUITE(_suite_name, _parent, _setup, _teardown)         \
	{                                                                \
		.object        = CUTE_INIT_OBJECT(_suite_name, _parent), \
		.setup         = _setup,                                 \
		.teardown      = _teardown,                              \
		.total_count   = 0,                                      \
		.success_count = 0,                                      \
		.failure_count = 0,                                      \
		.error_count   = 0,                                      \
		.skipped_count = 0,                                      \
	}

#define CUTE_FIXTURED_SUITE(_suite_name, _setup, _teardown) \
	struct cute_suite _suite_name =                     \
		CUTE_INIT_SUITE(_suite_name, NULL, _setup, _teardown)

#define CUTE_SUITE(_suite_name) \
	CUTE_FIXTURED_SUITE(_suite_name, NULL, NULL)

#define CUTE_PNP_FIXTURED_SUITE(_suite_name, _parent, _setup, _teardown) \
	struct cute_suite _suite_name __cute_pnp("cute_suites") =        \
		CUTE_INIT_SUITE(_suite_name, _parent, _setup, _teardown)

#define CUTE_PNP_SUITE(_suite_name, _parent) \
	CUTE_PNP_FIXTURED_SUITE(_suite_name, _parent, NULL, NULL)

extern int cute_register_suite(struct cute_suite *parent,
                               struct cute_suite *suite);

static inline int
cute_run_suite(struct cute_suite *suite)
{
	return cute_run(&suite->object);
}

/******************************************************************************
 * Test definition
 ******************************************************************************/

enum cute_state {
	CUTE_SKIPPED_STATE = 0,
	CUTE_SUCCESS_STATE = 1,
	CUTE_FAILURE_STATE = 2,
	CUTE_ERROR_STATE   = 3,
};

struct cute_result {
	enum cute_state  state;
	char            *reason;
	char            *line;
	char            *file;
	char            *console;
};

#define CUTE_INIT_RESULT()                     \
	{                                      \
		.state   = CUTE_SKIPPED_STATE, \
		.reason  = NULL,               \
		.line    = NULL,               \
		.file    = NULL,               \
		.console = NULL,               \
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

#define CUTE_INIT_TEST(_test_name, _suite, _test_function, _timeout)        \
	{                                                                   \
		.object  = CUTE_INIT_OBJECT(_test_name, &(_suite)->object), \
		.file    = __FILE__,                                        \
		.line    = __LINE__,                                        \
		.timeout = _timeout,                                        \
		.run     = _test_function,                                  \
		.result  = CUTE_INIT_RESULT()                               \
	}

#define CUTE_TIMED_TEST(_test_name, _timeout)                           \
	static void _test_name ## __cute_test_fn(void);                 \
	static struct cute_test _test_name =                            \
		CUTE_INIT_TEST(_test_name, (struct cute_suite *)NULL,   \
		               _test_name ## __cute_test_fn, _timeout); \
	static void _test_name ## __cute_test_fn(void)

#define CUTE_TEST(_test_name) \
	CUTE_TIMED_TEST(_test_name, CUTE_DEFAULT_TIMEOUT)

#define CUTE_PNP_TIMED_TEST(_test_name, _suite, _timeout)               \
	static void _test_name ## __cute_test_fn(void);                 \
	static struct cute_test _test_name __cute_pnp("cute_tests") =   \
		CUTE_INIT_TEST(_test_name, _suite,                      \
		               _test_name ## __cute_test_fn, _timeout); \
	static void _test_name ## __cute_test_fn(void)

#define CUTE_PNP_TEST(_test_name, _suite) \
	CUTE_PNP_TIMED_TEST(_test_name, _suite, CUTE_DEFAULT_TIMEOUT)

extern void cute_register_test(struct cute_suite *suite,
                               struct cute_test  *test);

static inline int
cute_run_test(struct cute_test *test)
{
	return cute_run(&test->object);
}

/******************************************************************************
 * High level API
 ******************************************************************************/

#define CUTE_DEFAULT_TIMEOUT (0U)

extern void cute_setup_text_report(void);

extern int cute_setup_posix_run(unsigned int default_timeout);

extern int cute_pnp_main(int argc, char *argv[], const char *root_name);

#endif
