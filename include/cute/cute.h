#ifndef _CUTE_H
#define _CUTE_H

#include <stdbool.h>
#include <stdlib.h>

#define __cute_export \
	__attribute__((visibility("default")))

#define __cute_noreturn \
	__attribute__((noreturn))

enum cute_config_report {
	CUTE_CONFIG_SILENT_REPORT = (1U << 0),
	CUTE_CONFIG_TERSE_REPORT  = (1U << 1),
	CUTE_CONFIG_VERB_REPORT   = (1U << 2),
	CUTE_CONFIG_TAP_REPORT    = (1U << 3),
	CUTE_CONFIG_XML_REPORT    = (1U << 4)
};

enum cute_config_tty {
	CUTE_CONFIG_FALSE_TTY  = 0,
	CUTE_CONFIG_TRUE_TTY   = 1,
	CUTE_CONFIG_PROBE_TTY,
	CUTE_CONFIG_TTY_NR
};

struct cute_config {
	const char *         match;
	bool                 icase;
	unsigned int         reports;
	enum cute_config_tty tty;
	const char *         tap_path;
	const char *         xml_path;
};

#define CUTE_CONFIG_INIT \
	{ \
		.match    = NULL, \
		.icase    = false, \
		.reports  = 0, \
		.tty      = CUTE_CONFIG_PROBE_TTY, \
		.tap_path = NULL, \
		.xml_path = NULL \
	}

struct cute_base;
struct cute_iter;
struct cute_run;

typedef struct cute_iter * (cute_iter_fn)(const struct cute_base * base);

typedef struct cute_run * (cute_run_fn)(const struct cute_base * base,
                                        struct cute_run *        parent);

struct cute_ops {
	cute_iter_fn * iter;
	cute_run_fn *  run;
};

typedef void (cute_test_fn)(void);

struct cute_base {
	const struct cute_ops * ops;
	cute_test_fn *          setup;
	cute_test_fn *          teardown;
	const char *            name;
	unsigned int            tmout;
	const char *            file;
	int                     line;
};

#define CUTE_BASE_INIT(_name, _ops, _setup, _teardown, _tmout) \
	{ \
		.ops      = _ops, \
		.setup    = _setup, \
		.teardown = _teardown, \
		.name     = _name, \
		.tmout    = _tmout, \
		.file     = __FILE__, \
		.line     = __LINE__ \
	}

static inline void
cute_dummy_fixture(void)
{
}

#define CUTE_NULL_SETUP    (cute_dummy_fixture)
#define CUTE_NULL_TEARDOWN (cute_dummy_fixture)
#define CUTE_FOREVER_TMOUT (0U)
#define CUTE_DFLT_TMOUT    (3U)

extern const struct cute_ops cute_test_ops __cute_export;

struct cute_test {
	struct cute_base super;
	cute_test_fn *   exec;
};

#define CUTE_TEST_INIT(_name, _exec, _setup, _teardown, _tmout) \
	{ \
		.super = CUTE_BASE_INIT(_name, \
		                        &cute_test_ops, \
		                        _setup, \
		                        _teardown, \
		                        _tmout), \
		.exec  = _exec, \
	}

#define CUTE_TEST_DEFINE(_name, _exec, _setup, _teardown, _tmout) \
	const struct cute_test _name = CUTE_TEST_INIT(# _name, \
		                                      _exec, \
		                                      _setup, \
		                                      _teardown, \
		                                      _tmout)

#define CUTE_STATIC_TEST(_name, _setup, _teardown, _tmout) \
	static void _name ## __cute_exec(void); \
	static CUTE_TEST_DEFINE(_name, \
	                        _name ## __cute_exec, \
	                        _setup, \
	                        _teardown, \
	                        _tmout); \
	static void _name ## __cute_exec(void)

#define CUTE_TEST(_name) \
	CUTE_STATIC_TEST(_name, \
	                 CUTE_NULL_SETUP, \
	                 CUTE_NULL_TEARDOWN, \
	                 CUTE_DFLT_TMOUT)

enum cute_issue {
	CUTE_UNK_ISSUE  = 0,
	CUTE_PASS_ISSUE,
	CUTE_SKIP_ISSUE,
	CUTE_FAIL_ISSUE,
	CUTE_EXCP_ISSUE,
	CUTE_OFF_ISSUE,
	CUTE_ISSUE_NR,
};

extern void
cute_break(enum cute_issue issue,
           const char *    file,
           int             line,
           const char *    reason) __cute_noreturn __cute_export;

#define cute_skip(void) \
	cute_break(CUTE_SKIP_ISSUE, \
	           __FILE__, \
	           __LINE__, \
	           "explicit skipping requested")

#define cute_fail(void) \
	cute_break(CUTE_FAIL_ISSUE, \
	           __FILE__, \
	           __LINE__, \
	           "explicit failure requested")

#define cute_ensure(_expr) \
	({ \
		if (!(_expr)) \
			cute_break(CUTE_FAIL_ISSUE, \
			           __FILE__, \
			           __LINE__, \
			           # _expr); \
	 })

extern const struct cute_ops cute_suite_ops __cute_export;

struct cute_suite {
	struct cute_base                 super;
	unsigned int                     nr;
	const struct cute_base * const * tests;
};

#define CUTE_SUITE_INIT(_name, _tests, _setup, _teardown, _tmout) \
	{ \
		.super = CUTE_BASE_INIT(_name, \
		                        &cute_suite_ops, \
		                        _setup, \
		                        _teardown, \
		                        _tmout), \
		.nr    = sizeof(_tests) / sizeof((_tests)[0]), \
		.tests = _tests \
	}

#define CUTE_SUITE_DEFINE(_name, _tests, _setup, _teardown, _tmout) \
	const struct cute_suite _name = CUTE_SUITE_INIT(# _name, \
	                                                _tests, \
	                                                _setup, \
	                                                _teardown, \
	                                                _tmout)

#define CUTE_SUITE_DEFINE_TESTS(_name) \
	const struct cute_base * const _name[]


#define CUTE_SUITE(_name, _setup, _teardown, _tmout) \
	static const struct cute_base * const * _name ## __cute_tests; \
	CUTE_SUITE_DEFINE(_name, \
	                  _name ## __cute_tests, \
	                  _setup, \
	                  _teardown, \
	                  _tmout); \
	static CUTE_SUITE_DEFINE_TESTS(_name ## __cute_tests)

#define CUTE_REF(_name) \
	(&(_name).super)

extern void
cute_show_suite(const struct cute_suite *  suite) __cute_export;

extern int
cute_run_suite(const struct cute_suite * suite) __cute_export;

extern int
cute_init(struct cute_config * config, const char * title) __cute_export;

extern int
cute_fini(void) __cute_export;

extern int
cute_main(int                       argc,
          char * const              argv[],
          const struct cute_suite * suite,
          const char *              title) __cute_export;

#define CUTE_MAIN(_root, _title) \
	int main(int argc, char * const argv[]) \
	{ \
		return cute_main(argc, argv, &(_root), _title); \
	}

#endif /* _CUTE_H */
