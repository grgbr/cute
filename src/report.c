#include "cons.h"
#include "run.h"
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/******************************************************************************
 * Primitive terminal handling
 ******************************************************************************/

#include <stdarg.h>

#define CUTE_TERM_RED_COLOR     "\e[91m"
#define CUTE_TERM_GREEN_COLOR   "\e[32m"
#define CUTE_TERM_YELLOW_COLOR  "\e[33m"
#define CUTE_TERM_BLUE_COLOR    "\e[34m"
#define CUTE_TERM_GRAY_COLOR    "\e[37m"
#define CUTE_TERM_FORE_COLOR    "\e[39m"

#define CUTE_TERM_REGULAR       "\e[0m"
#define CUTE_TERM_BOLD_GRAPH    "\e[1m"
#define CUTE_TERM_ITALIC_GRAPH  "\e[3m"

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

void
cute_term_setup(struct cute_term *   term,
                FILE *               stdio,
                enum cute_config_tty config)
{
	cute_assert_intern(term);
	cute_assert_intern(stdio);

	bool tty;
	int  ret;

	switch (config) {
	case CUTE_CONFIG_TRUE_TTY:
		tty = true;
		break;

	case CUTE_CONFIG_FALSE_TTY:
		tty = false;
		break;

	case CUTE_CONFIG_PROBE_TTY:
		ret = isatty(fileno(stdio));
		if (ret >= 0)
			tty = !!ret;
		else
			tty = false;
		break;

	default:
		__cute_unreachable();
	}

	if (tty) {
		term->regular = CUTE_TERM_REGULAR;
		term->bold = CUTE_TERM_BOLD_GRAPH;
		term->italic = CUTE_TERM_ITALIC_GRAPH;
		term->red = CUTE_TERM_RED_COLOR;
		term->green = CUTE_TERM_GREEN_COLOR;
		term->yellow = CUTE_TERM_YELLOW_COLOR;
		term->blue = CUTE_TERM_BLUE_COLOR;
		term->gray = CUTE_TERM_GRAY_COLOR;
		term->fore = CUTE_TERM_FORE_COLOR;
	}
	else {
		term->regular = "";
		term->bold = "";
		term->italic = "";
		term->red = "";
		term->green = "";
		term->yellow = "";
		term->blue = "";
		term->gray = "";
		term->fore = "";
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
                         FILE *                         stdio)
{
	unsigned int                  a;
	const struct cute_text_atom * atom;

	cute_text_foreach(block, a, atom)
		fprintf(stdio, "%*s%s\n", indent, "", atom->str);
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
