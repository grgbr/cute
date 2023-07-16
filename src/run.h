#ifndef _CUTE_RUN_H
#define _CUTE_RUN_H

#include "expect.h"
#include "report.h"
#include "iodir.h"
#include "util.h"

struct cute_run;

enum cute_oper {
	CUTE_SPAWN_OPER,
	CUTE_COMPLETE_OPER
};

struct cute_stats {
	unsigned int pass;
	unsigned int skip;
	unsigned int fail;
	unsigned int excp;
	unsigned int exec;
	unsigned int total;
};

typedef void
        (cute_run_report_fn)(const struct cute_run *, enum cute_event);

typedef struct cute_iter *
        (cute_run_iter_fn)(struct cute_run *);

typedef void
        (cute_run_oper_fn)(struct cute_run *, enum cute_oper);

typedef void
        (cute_run_sum_fn)(const struct cute_run *, struct cute_stats *);

typedef void
        (cute_run_join_fn)(struct cute_run *, struct cute_run *);

typedef void
        (cute_run_fini_fn)(struct cute_run *);

struct cute_run_ops {
	cute_run_report_fn * report;
	cute_run_iter_fn *   iter;
	cute_run_oper_fn *   oper;
	cute_run_sum_fn *    sum;
	cute_run_join_fn *   join;
	cute_run_fini_fn *   fini;
};

#define cute_run_assert_ops(_ops) \
	cute_assert(_ops); \
	cute_assert((_ops)->report); \
	cute_assert((_ops)->iter); \
	cute_assert((_ops)->oper); \
	cute_assert((_ops)->sum); \
	cute_assert((_ops)->join); \
	cute_assert((_ops)->fini)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_run_assert_ops_intern(_ops) \
	cute_run_assert_ops(_ops)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_run_assert_ops_intern(_ops)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

struct cute_run {
	const struct cute_run_ops *     ops;
	char *                          name;
	struct cute_run *               parent;
	int                             depth;
	int                             id;
	const struct cute_base *        base;
	enum cute_state                 state;
	enum cute_issue                 issue;
	struct timespec                 begin;
	struct timespec                 end;
	const char *                    what;
	const char *                    why;
	union {
		struct cute_assess      assess;
		struct cute_expect      call;
		struct cute_expect_parm parm;
		struct cute_expect      retval;
	};
	struct cute_iodir_block         ioout;
	struct cute_iodir_block         ioerr;
};

#define cute_run_assert(_run) \
	cute_assert(_run); \
	cute_run_assert_ops((_run)->ops); \
	cute_assert((_run)->name); \
	cute_assert((_run)->name[0]); \
	cute_base_assert((_run)->base); \
	cute_state_assert((_run)->state); \
	cute_issue_assert((_run)->issue)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_run_assert_intern(_run) \
	cute_run_assert(_run)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_run_assert_intern(_run)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline void
cute_run_dummy_join(struct cute_run * run __cute_unused,
                    struct cute_run * sub __cute_unused)
{
	cute_run_assert_intern(run);
	cute_assert_intern((run->state == CUTE_INIT_STATE) ||
	                   (run->state == CUTE_OFF_STATE));
	cute_assert_intern(sub);
}

static inline void
cute_run_dummy_fini(struct cute_run * run __cute_unused)
{
	cute_run_assert_intern(run);
}

extern void
cute_run_settle(struct cute_run * run);

extern void
cute_run_unsettle(struct cute_run * run);

extern int
cute_run_setup(struct cute_run * run);

extern void
cute_run_teardown(struct cute_run * run);

extern int
cute_run_done(struct cute_run * run);

static inline void
cute_run_sum_stats(const struct cute_run * run, struct cute_stats * stats)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state == CUTE_DONE_STATE);
	cute_assert_intern(stats);

	run->ops->sum(run, stats);
}

extern void
cute_run_oper(struct cute_run * run, enum cute_oper oper);

static inline void
cute_run_report(const struct cute_run * run, enum cute_event event)
{
	cute_run_assert_intern(run);
	cute_event_assert_intern(event);

	run->ops->report(run, event);
}

extern void
cute_run_turn_on(struct cute_run * run);

extern void
cute_run_init(struct cute_run *           run,
              const struct cute_run_ops * ops,
              const struct cute_base *    base,
              struct cute_run *           parent);

extern void
cute_run_destroy(struct cute_run * run);

typedef void
        (cute_run_process_fn)(struct cute_run *, enum cute_visit, void *);

extern void
cute_run_foreach(struct cute_run *     run,
                 cute_run_process_fn * process,
                 void *                data);

extern struct cute_run * volatile cute_curr_run;

extern void __cute_noreturn
cute_break(enum cute_issue issue,
           const char *    file,
           int             line,
           const char *    func,
           const char *    why);

#endif /* _CUTE_RUN_H */
