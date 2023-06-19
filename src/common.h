#ifndef _CUTE_COMMON_H
#define _CUTE_COMMON_H

#include "cute/config.h"
#include "cute/cute.h"
#include "util.h"
#include <setjmp.h>
#include <values.h>

#define CUTE_NAME_MAX_LEN      (96U)
#define CUTE_FULL_NAME_MAX_LEN (192U)

enum cute_state {
	CUTE_INIT_STATE,
	CUTE_OFF_STATE,
	CUTE_SETUP_STATE,
	CUTE_EXEC_STATE,
	CUTE_TEARDOWN_STATE,
	CUTE_DONE_STATE,
	CUTE_FINI_STATE,
	CUTE_STATE_NR
};

#define cute_state_assert(_state) \
	cute_assert((_state) < CUTE_STATE_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_state_assert_intern(_state) \
	cute_state_assert(_state)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_state_assert_intern(_state)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

const char *
cute_state_label(enum cute_state state);

#define cute_issue_assert(_issue) \
	cute_assert((_issue) < CUTE_ISSUE_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_issue_assert_intern(_issue) \
	cute_issue_assert(_issue)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_issue_assert_intern(_issue)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

const char *
cute_issue_label(enum cute_issue issue);

#define cute_base_assert_ops(_ops) \
	cute_assert(_ops); \
	cute_assert((_ops)->iter); \
	cute_assert((_ops)->run)

#define cute_base_assert(_base) \
	cute_assert(_base); \
	cute_base_assert_ops((_base)->ops); \
	cute_assert((_base)->setup); \
	cute_assert((_base)->teardown); \
	cute_assert((_base)->name); \
	cute_assert((_base)->name[0]); \
	cute_assert((_base)->file); \
	cute_assert((_base)->line >= 0)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_base_assert_ops_intern(_ops) \
	cute_base_assert_ops(_ops)

#define cute_base_assert_intern(_base) \
	cute_base_assert(_base)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_base_assert_ops_intern(_ops)

#define cute_base_assert_intern(_base)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define CUTE_CONFIG_REPORT_VALID \
	((unsigned int) \
	 (CUTE_CONFIG_SILENT_REPORT | \
	  CUTE_CONFIG_TERSE_REPORT | \
	  CUTE_CONFIG_VERB_REPORT | \
	  CUTE_CONFIG_TAP_REPORT | \
	  CUTE_CONFIG_XML_REPORT))

#define cute_config_assert_report(_report) \
	cute_assert(!((_report) & ~CUTE_CONFIG_REPORT_VALID))

#define cute_config_assert_tty(_tty) \
	cute_assert(((_tty) >= 0) && ((_tty) < CUTE_CONFIG_TTY_NR))

#define cute_config_assert(_conf) \
	cute_assert(_conf); \
	cute_config_assert_report((_conf)->reports); \
	cute_config_assert_tty((_conf)->tty)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_config_assert_report_intern(_report) \
	cute_config_assert_report(_report)

#define cute_config_assert_tty_intern(_tty) \
	cute_config_assert_tty(_tty)

#define cute_config_assert_intern(_conf) \
	cute_config_assert(_conf)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_config_assert_report_intern(_report)

#define cute_config_assert_tty_intern(_tty)

#define cute_config_assert_intern(_conf)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

extern struct cute_config * cute_the_config;
extern sigjmp_buf           cute_jmp_env;
extern unsigned int         cute_run_nr;

extern struct cute_iter *
cute_base_create_iter(const struct cute_base * base);

struct cute_run;

extern struct cute_run *
cute_create_run(const struct cute_base * base, struct cute_run * parent);

enum cute_visit {
	CUTE_BEGIN_VISIT,
	CUTE_END_VISIT,
	CUTE_ONCE_VISIT
};

typedef void
        (cute_base_process_fn)(const struct cute_base *,
                               enum cute_visit,
                               void *);

extern void
cute_base_foreach(const struct cute_base * root,
                  cute_base_process_fn *   process,
                  void *                   data);

extern bool
cute_config_match_run(const struct cute_run * run);

#endif /* _CUTE_COMMON_H */
