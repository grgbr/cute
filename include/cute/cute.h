#ifndef _CUTE_H
#define _CUTE_H

#include <cute/priv/core.h>
#include <cute/priv/types.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#define CUTE_SINT_RANGE(_min, _max) \
	__CUTE_RANGE(sint, _min, _max)

#define CUTE_SINT_SET(...) \
	__CUTE_SET(sint, __VA_ARGS__)

#define CUTE_UINT_RANGE(_min, _max) \
	__CUTE_RANGE(uint, _min, _max)

#define CUTE_UINT_SET(...) \
	__CUTE_SET(uint, __VA_ARGS__)

#define CUTE_FLT_RANGE(_min, _max) \
	__CUTE_RANGE(flt, _min, _max)

#define CUTE_FLT_SET(...) \
	__CUTE_SET(flt, __VA_ARGS__)

#define CUTE_STR_SET(...) \
	__CUTE_SET(str, __VA_ARGS__)

#define CUTE_PTR_RANGE(_min, _max) \
	__CUTE_RANGE(ptr, _min, _max)

#define CUTE_PTR_SET(...) \
	__CUTE_SET(ptr, __VA_ARGS__)

#define CUTE_NULL_SETUP    ((cute_test_fn *)0)
#define CUTE_INHR_SETUP    ((cute_test_fn *)1)
#define CUTE_NULL_TEARDOWN ((cute_test_fn *)0)
#define CUTE_INHR_TEARDOWN ((cute_test_fn *)1)

#define CUTE_FOREVER_TMOUT (0U)
#define CUTE_DFLT_TMOUT    (3U)
#define CUTE_INHR_TMOUT    (UINT_MAX)

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

#define cute_skip(_reason) \
	_cute_skip(_reason, __FILE__, __LINE__, __func__)

#define cute_fail(_reason) \
	_cute_fail(_reason, __FILE__, __LINE__, __func__)

extern void
cute_show_suite(const struct cute_suite *  suite) __cute_export;

extern int
cute_run_suite(const struct cute_suite * suite) __cute_export;

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
	bool                 debug;
	bool                 icase;
	unsigned int         reports;
	enum cute_config_tty tty;
	const char *         tap_path;
	const char *         xml_path;
};

#define CUTE_CONFIG_INIT \
	{ \
		.match    = NULL, \
		.debug    = false, \
		.icase    = false, \
		.reports  = 0, \
		.tty      = CUTE_CONFIG_PROBE_TTY, \
		.tap_path = NULL, \
		.xml_path = NULL \
	}

extern int
cute_init(struct cute_config * config,
          const char *         package,
          const char *         version) __cute_export;

extern void
cute_fini(void) __cute_export;

extern int
cute_main(int                       argc,
          char * const              argv[],
          const struct cute_suite * suite,
          const char *              package,
          const char *              version) __cute_export;

#define CUTE_MAIN(_root, _package, _version) \
	int main(int argc, char * const argv[]) \
	{ \
		return cute_main(argc, argv, &(_root), _package, _version); \
	}

#endif /* _CUTE_H */
