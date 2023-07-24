#ifndef _CUTE_H
#define _CUTE_H

#include <cute/priv/core.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

/******************************************************************************
 * Test case implementation primitives
 ******************************************************************************/

#define cute_skip(_reason) \
	_cute_skip(_reason, __FILE__, __LINE__, __func__)

#define cute_fail(_reason) \
	_cute_fail(_reason, __FILE__, __LINE__, __func__)

/******************************************************************************
 * Common definitions
 ******************************************************************************/

#define CUTE_NULL_SETUP    ((cute_test_fn *)0)
#define CUTE_INHR_SETUP    ((cute_test_fn *)1)
#define CUTE_NULL_TEARDOWN ((cute_test_fn *)0)
#define CUTE_INHR_TEARDOWN ((cute_test_fn *)1)

#define CUTE_FOREVER_TMOUT (0U)
#define CUTE_DFLT_TMOUT    (3U)
#define CUTE_INHR_TMOUT    (UINT_MAX)

/******************************************************************************
 * Test case definitions
 ******************************************************************************/

#define CUTE_TEST_DECL(_name) \
	const struct cute_test _name

#define CUTE_TEST_DEFN(_name, _exec, _setup, _teardown, _tmout) \
	const struct cute_test _name = CUTE_TEST_INIT(# _name, \
		                                      _exec, \
		                                      _setup, \
		                                      _teardown, \
		                                      _tmout)

#define CUTE_TEST_STATIC(_name, _setup, _teardown, _tmout) \
	static void _name ## __cute_exec(void); \
	static CUTE_TEST_DEFN(_name, \
	                      _name ## __cute_exec, \
	                      _setup, \
	                      _teardown, \
	                      _tmout); \
	static void _name ## __cute_exec(void)

#define CUTE_TEST_EXTERN(_name, _setup, _teardown, _tmout) \
	static void _name ## __cute_exec(void); \
	CUTE_TEST_DEFN(_name, \
	               _name ## __cute_exec, \
	               _setup, \
	               _teardown, \
	               _tmout); \
	static void _name ## __cute_exec(void)

#define CUTE_TEST(_name) \
	CUTE_TEST_STATIC(_name, \
	                 CUTE_INHR_SETUP, \
	                 CUTE_INHR_TEARDOWN, \
	                 CUTE_INHR_TMOUT)

/******************************************************************************
 * Test group definitions
 ******************************************************************************/

#define CUTE_REF(_name) \
	(&(_name).super)

#define CUTE_GROUP_DECL(_name) \
	const struct cute_base * const * const _name

#define CUTE_GROUP_DEFN(_name) \
	const struct cute_base * const _name[]

#define CUTE_GROUP_STATIC(_name) \
	static CUTE_GROUP_DEFN(_name)

#define CUTE_GROUP_EXTERN(_name) \
	CUTE_GROUP_DEFN(_name)

#define CUTE_GROUP(_name) \
	CUTE_GROUP_STATIC(_name)

/******************************************************************************
 * Test suite definitions
 ******************************************************************************/

#define CUTE_SUITE_DECL(_name) \
	const struct cute_suite _name

#define CUTE_SUITE_DEFN(_name, _tests, _setup, _teardown, _tmout) \
	const struct cute_suite _name = CUTE_SUITE_INIT(# _name, \
	                                                _tests, \
	                                                _setup, \
	                                                _teardown, \
	                                                _tmout)

#define CUTE_SUITE_STATIC(_name, _group, _setup, _teardown, _tmout) \
	static CUTE_SUITE_DEFN(_name, \
	                       _group, \
	                       _setup, \
	                       _teardown, \
	                       _tmout)

#define CUTE_SUITE_EXTERN(_name, _setup, _teardown, _tmout) \
	CUTE_SUITE_DEFN(_name, \
	                _group, \
	                _setup, \
	                _teardown, \
	                _tmout)

#define CUTE_SUITE(_name, _group) \
	CUTE_SUITE_STATIC(_name, \
	                  _group, \
	                  CUTE_INHR_SETUP, \
	                  CUTE_INHR_TEARDOWN, \
	                  CUTE_INHR_TMOUT)

extern void
cute_show_suite(const struct cute_suite *  suite) __cute_export;

extern int
cute_run_suite(const struct cute_suite * suite) __cute_export;

/******************************************************************************
 * Top-level definitions
 ******************************************************************************/

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
