/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_REPORT_H
#define _CUTE_REPORT_H

#include "common.h"

#define CUTE_TERM_INDENT_PREF  "   "
#define CUTE_TERM_INDENT_WIDTH ((int)sizeof(CUTE_TERM_INDENT_PREF) - 1)

struct cute_term {
	const char * regular;
	const char * bold;
	const char * italic;
	const char * red;
	const char * green;
	const char * yellow;
	const char * blue;
	const char * gray;
	const char * fore;
};

#define cute_term_assert(_term) \
	cute_assert((_term)->red); \
	cute_assert((_term)->green); \
	cute_assert((_term)->yellow); \
	cute_assert((_term)->blue); \
	cute_assert((_term)->gray); \
	cute_assert((_term)->bold); \
	cute_assert((_term)->regular); \
	cute_assert((_term)->fore)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_term_assert_intern(_term) \
	cute_term_assert(_term)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_term_assert_intern(_term)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline int
cute_term_indent_width(int depth)
{
	cute_assert_intern(depth >= 0);

	return depth * CUTE_TERM_INDENT_WIDTH;
}

extern int
cute_term_depth_width(const char * name, int depth);

extern void
cute_term_depth_vprintf(const struct cute_term * term,
                        FILE *                   stdio,
                        int                      depth,
                        const char *             format,
                        va_list                  args);

extern void
cute_term_depth_printf(const struct cute_term * term,
                       FILE *                   stdio,
                       int                      depth,
                       const char *             format,
                                                ...)
	__attribute__((format(printf, 4, 5)));

extern const char *
cute_term_issue_color(const struct cute_term * term, enum cute_issue issue);

enum cute_event {
	CUTE_INIT_EVT,
	CUTE_HEAD_EVT,
	CUTE_SETUP_EVT,
	CUTE_EXEC_EVT,
	CUTE_TEARDOWN_EVT,
	CUTE_DONE_EVT,
	CUTE_FOOT_EVT,
	CUTE_SHOW_EVT,
	CUTE_FINI_EVT,
	CUTE_EVT_NR
};

#define cute_event_assert(_event) \
	cute_assert((_event) < CUTE_EVT_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_event_assert_intern(_event) \
	cute_event_assert(_event)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_event_assert_intern(_event)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

enum cute_kind {
	CUTE_SUITE_KIND,
	CUTE_TEST_KIND,
	CUTE_KIND_NR
};

#define cute_kind_assert(_kind) \
	cute_assert((_kind) < CUTE_KIND_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_kind_assert_intern(_kind) \
	cute_kind_assert(_kind)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_kind_assert_intern(_kind)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

struct cute_report;
struct cute_run;

typedef void (cute_report_handle_fn)(struct cute_report *,
                                     enum cute_event,
                                     enum cute_kind,
                                     const struct cute_run *);

typedef int (cute_report_release_fn)(struct cute_report *);

struct cute_report {
	cute_report_handle_fn *  handle;
	cute_report_release_fn * release;
};

#define cute_report_assert(_report) \
	cute_assert(_report); \
	cute_assert((_report)->handle); \
	cute_assert((_report)->release)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_report_assert_intern(_report) \
	cute_report_assert(_report)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_report_assert_intern(_report)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

extern void
cute_report_printf_block(const struct cute_text_block * block,
                         int                            indent,
                         FILE *                         stdio);

extern unsigned int
cute_report_progress(void);

extern void
cute_report_test(enum cute_event event, const struct cute_run * run);

extern void
cute_report_suite(enum cute_event event, const struct cute_run * run);

extern void
cute_report_on_show(const struct cute_run * run,
		    FILE *                  stdio,
		    const char  *           high,
		    const char  *           regular);

extern void
cute_report_register(struct cute_report * report);

extern void
cute_report_release(void);

extern int
cute_report_setup_silent(const struct cute_config * config);

extern int
cute_report_setup_tap(const struct cute_config * config);

extern int
cute_report_setup_terse(const struct cute_config * config);

extern int
cute_report_setup_verb(const struct cute_config * config);

extern int
cute_report_setup_xml(const struct cute_config * config);

#endif /* _CUTE_REPORT_H */
