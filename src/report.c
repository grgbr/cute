/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cons.h"
#include "run.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/******************************************************************************
 * Primitive terminal handling
 ******************************************************************************/

#include <stdarg.h>

int
cute_term_depth_width(const char * name, int depth)
{
	cute_assert_intern(name);
	cute_assert_intern(depth >= 0);

	size_t len;

	len = strlen(name);
	cute_assert_intern(len);
	cute_assert_intern(len < INT_MAX);

	return (int)len + cute_term_indent_width(depth);
}

void
cute_term_depth_vprintf(const struct cute_term * term __cute_unused,
                        FILE *                   stdio,
                        int                      depth,
                        const char *             format,
                        va_list                  args)
{
	cute_term_assert_intern(term);
	cute_assert_intern(stdio);
	cute_assert_intern(depth >= 0);
	cute_assert_intern(format);
	cute_assert_intern(format[0]);

	while (depth--)
		fputs(CUTE_TERM_INDENT_PREF, stdio);

	vfprintf(stdio, format, args);
}

void
cute_term_depth_printf(const struct cute_term * term,
                       FILE *                   stdio,
                       int                      depth,
                       const char *             format,
                                                ...)
{
	cute_term_assert_intern(term);
	cute_assert_intern(stdio);
	cute_assert_intern(depth >= 0);
	cute_assert_intern(format);
	cute_assert_intern(format[0]);

	va_list args;

	va_start(args, format);
	cute_term_depth_vprintf(term, stdio, depth, format, args);
	va_end(args);
}

const char *
cute_term_issue_color(const struct cute_term * term, enum cute_issue issue)
{
	switch (issue) {
	case CUTE_UNK_ISSUE:
		return term->regular;

	case CUTE_PASS_ISSUE:
		return  term->green;

	case CUTE_SKIP_ISSUE:
		return term->yellow;

	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		return term->red;

	case CUTE_OFF_ISSUE:
		return term->gray;

	default:
		__cute_unreachable();
	}
}

/******************************************************************************
 * Generic report handling
 ******************************************************************************/

static unsigned int          cute_report_count;
static struct cute_report ** cute_report_table;
static unsigned int          cute_report_done;

void
cute_report_printf_block(const struct cute_text_block * block,
                         int                            indent,
                         FILE *                         stdio,
                         cute_text_format_fn *          format)
{
	unsigned int                  a;
	const struct cute_text_atom * atom;

	cute_text_foreach(block, a, atom)
		format(stdio, indent, atom->str);
}

unsigned int
cute_report_progress(void)
{
	return (cute_report_done * 100) / cute_run_nr;
}

void
cute_report_register(struct cute_report * report)
{
	cute_report_assert_intern(report);

	cute_report_table = cute_realloc(cute_report_table,
	                                 (cute_report_count + 1) *
	                                 sizeof(*cute_report_table));
	cute_report_table[cute_report_count++] = report;
}

static void
cute_report_handle(enum cute_event         event,
                   enum cute_kind          kind,
                   const struct cute_run * run)
{
	cute_event_assert_intern(event);
	cute_kind_assert_intern(kind);
	cute_run_assert_intern(run);

	unsigned int r;

	if (event == CUTE_DONE_EVT)
		cute_report_done++;

	cute_cons_report_handle(event, kind, run);

	for (r = 0; r < cute_report_count; r++)
		cute_report_table[r]->handle(cute_report_table[r],
		                             event,
		                             kind,
		                             run);
}

void
cute_report_test(enum cute_event event, const struct cute_run * run)
{
	cute_event_assert_intern(event);
	cute_run_assert_intern(run);

	cute_report_handle(event, CUTE_TEST_KIND, run);
}

void
cute_report_suite(enum cute_event event, const struct cute_run * run)
{
	cute_event_assert_intern(event);
	cute_run_assert_intern(run);

	cute_report_handle(event, CUTE_SUITE_KIND, run);
}

static int
_cute_report_release(struct cute_report * report)
{
	cute_report_assert_intern(report);
	cute_config_assert_intern(cute_the_config);

	return report->release(report);
}

void
cute_report_release(void)
{
	unsigned int r;
	int          err;

	err = cute_cons_report_release();
	if (err)
		goto err;

	for (r = 0; r < cute_report_count; r++) {
		err = _cute_report_release(cute_report_table[r]);
		if (err)
			goto err;
	}

	cute_free(cute_report_table);

	return;

err:
	cute_error("cannot finalize report: %s (%d)\n",
	           strerror(-err),
	           -err);
	exit(EXIT_FAILURE);
}

static void
cute_silent_report_handle(struct cute_report *    report __cute_unused,
                          enum cute_event         event __cute_unused,
                          enum cute_kind          kind __cute_unused,
                          const struct cute_run * run __cute_unused)
{
	return;
}

static int
cute_silent_report_release(struct cute_report * report)
{
	cute_free(report);

	return 0;
}

int
cute_report_setup_silent(const struct cute_config * config __cute_unused)
{
	struct cute_report * rprt;

	rprt = cute_malloc(sizeof(*rprt));

	rprt->handle = cute_silent_report_handle;
	rprt->release = cute_silent_report_release;

	cute_cons_report_register(rprt);

	return 0;
}
