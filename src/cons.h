#ifndef _CUTE_CONS_H
#define _CUTE_CONS_H

#include "report.h"

#define CUTE_CONS_REPORT_HRULE_SYM   '#'
#define CUTE_CONS_REPORT_FILL_SYM    '.'
#define CUTE_CONS_REPORT_TABLE_SYM   '-'
#define CUTE_CONS_REPORT_PROG_FMT    "[%3u%%]"
#define CUTE_CONS_REPORT_PROG_PAD    "      "
#define CUTE_CONS_REPORT_PROG_WIDTH  ((int) \
                                      sizeof(CUTE_CONS_REPORT_PROG_PAD) - 1)
#define CUTE_CONS_REPORT_NAME_HEAD   "NAME"
#define CUTE_CONS_REPORT_NAME_FMT    "%-*.*s"
#define CUTE_CONS_REPORT_STAT_HEAD   "STAT"
#define CUTE_CONS_REPORT_STAT_PAD    "    "
#define CUTE_CONS_REPORT_STAT_FMT    "%4.4s"
#define CUTE_CONS_REPORT_STAT_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_STAT_HEAD) - 1)
#define CUTE_CONS_REPORT_TIME_HEAD   " TIME(s.us)"
#define CUTE_CONS_REPORT_TIME_PAD    "           "
#define CUTE_CONS_REPORT_TIME_FMT    "%4ld.%06ld"
#define CUTE_CONS_REPORT_TIME_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_TIME_HEAD) - 1)
#define CUTE_CONS_REPORT_PASS_HEAD   " #PASS"
#define CUTE_CONS_REPORT_PASS_FMT    "%6u"
#define CUTE_CONS_REPORT_PASS_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_PASS_HEAD) - 1)
#define CUTE_CONS_REPORT_SKIP_HEAD   " #SKIP"
#define CUTE_CONS_REPORT_SKIP_FMT    "%6u"
#define CUTE_CONS_REPORT_SKIP_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_SKIP_HEAD) - 1)
#define CUTE_CONS_REPORT_FAIL_HEAD   " #FAIL"
#define CUTE_CONS_REPORT_FAIL_FMT    "%6u"
#define CUTE_CONS_REPORT_FAIL_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_FAIL_HEAD) - 1)
#define CUTE_CONS_REPORT_EXEC_HEAD   " #EXEC"
#define CUTE_CONS_REPORT_EXEC_FMT    "%6u"
#define CUTE_CONS_REPORT_EXEC_WIDTH  ((int) \
				      sizeof(CUTE_CONS_REPORT_EXEC_HEAD) - 1)
#define CUTE_CONS_REPORT_TOTAL_HEAD  "#TOTAL"
#define CUTE_CONS_REPORT_TOTAL_FMT   "%6u"
#define CUTE_CONS_REPORT_TOTAL_WIDTH ((int) \
				      sizeof(CUTE_CONS_REPORT_TOTAL_HEAD) - 1)
#define CUTE_CONS_REPORT_SUMUP_WIDTH \
	((1 + CUTE_CONS_REPORT_STAT_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_TIME_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_PASS_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_SKIP_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_FAIL_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_EXEC_WIDTH) + \
	 (1 + CUTE_CONS_REPORT_TOTAL_WIDTH))

#define CUTE_CONS_NAME_COLS_MIN      12

struct cute_cons_report {
	struct cute_report super;
	FILE *             stdio; /* Output file */
	int                colnr; /* Total output width in number of columns */
	int                ncols; /* Name field width in number of columns */
	char *             fill;  /* Name filler horizontal rule */
	struct cute_term   term;
};

#define cute_cons_report_assert(_report) \
	cute_assert(_report); \
	cute_report_assert(&(_report)->super); \
	cute_assert((_report)->stdio); \
	cute_term_assert(&(_report)->term)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_cons_report_assert_intern(_report) \
	cute_cons_report_assert(_report)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_cons_report_assert_intern(_report)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

extern void
cute_cons_report_head(const struct cute_cons_report * report);

extern void
cute_cons_report_test_done(const struct cute_cons_report * report,
                           const struct cute_run *         run);

struct cute_suite_run;

extern void
cute_cons_report_sumup(const struct cute_cons_report * report,
                       const struct cute_suite_run *   suite);

extern void
cute_cons_report_on_init(struct cute_cons_report * report, int name_cols);

extern void
cute_cons_report_on_head(struct cute_cons_report * report,
                         int                       colnr,
                         int                       fill_cols);

extern int
cute_cons_report_setup(struct cute_cons_report *  report,
                       const struct cute_config * config,
                       cute_report_handle_fn *    handle);

extern void
cute_cons_report_handle(enum cute_event         event,
                        enum cute_kind          kind,
                        const struct cute_run * run);

extern void
cute_cons_report_register(struct cute_report * report);

extern int
cute_cons_report_release(void);

#endif /* _CUTE_CONS_H */
