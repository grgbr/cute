#include "run.h"
#include "report.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

static volatile struct cute_run * volatile cute_curr_run;

static struct {
	int          no;
	const char * desc;
} const cute_run_sigs[] = {
    { SIGALRM, "timer expired" },
    { SIGFPE,  "floating point exception (SIGFPE) raised" },
    { SIGILL,  "illegal instruction (SIGILL) raised" },
    { SIGSEGV, "segmentation fault (SIGSEGV) raised" },
    { SIGBUS,  "bus error / bad memory access (SIGBUS) raised" },
    { SIGSYS,  "bad system call (SIGSYS) raised" }
};

static sighandler_t cute_run_saved_sigs[sizeof(cute_run_sigs) /
                                        sizeof(cute_run_sigs[0])];

static void
cute_run_handle_sig(int sig)
{
	cute_run_assert_intern(cute_curr_run);

	unsigned int s;
	const char * desc = NULL;

	for (s = 0;
	     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
	     s++) {
		if (sig == cute_run_sigs[s].no) {
			desc = cute_run_sigs[s].desc;
			break;
		}
	}

	cute_assert_intern(desc);

	cute_break(CUTE_FAIL_ISSUE,
	           cute_curr_run->base->file,
	           cute_curr_run->base->line,
	           desc);
}

void
cute_run_settle(const struct cute_run * run)
{
	cute_run_assert_intern(run);

	unsigned int s;

	for (s = 0;
	     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
	     s++) {
		cute_run_saved_sigs[s] = signal(cute_run_sigs[s].no,
		                                cute_run_handle_sig);
		cute_assert_intern(cute_run_saved_sigs[s] != SIG_ERR);
	}

	if (run->base->tmout > 0)
		alarm(run->base->tmout);
}

void
cute_run_unsettle(const struct cute_run * run)
{
	cute_run_assert_intern(run);

	unsigned int s;

	if (run->base->tmout > 0)
		alarm(0);

	for (s = 0;
	     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
	     s++) {
		sighandler_t err;

		err = signal(cute_run_sigs[s].no, cute_run_saved_sigs[s]);
		cute_assert_intern(err != SIG_ERR);
	}
}

static bool
cute_run_active(struct cute_run * run)
{
	const struct cute_run * parent;

	if (run->state == CUTE_OFF_STATE) {
		run->issue = CUTE_OFF_ISSUE;
		return false;
	}

	parent = run->parent;
	if (parent) {
		cute_run_assert_intern(parent);
		cute_assert_intern(parent->state != CUTE_EXEC_STATE);
		cute_assert_intern(parent->state != CUTE_TEARDOWN_STATE);
		cute_assert_intern(parent->state != CUTE_DONE_STATE);
		cute_assert_intern(parent->state != CUTE_FINI_STATE);

		enum cute_issue issue = run->parent->issue;

		if ((issue != CUTE_FAIL_ISSUE) && (issue != CUTE_SKIP_ISSUE))
			return true;

		run->issue = issue;
		run->file = run->base->file;
		run->line = run->base->line;
		run->what = "cannot run setup";
		run->why = (issue == CUTE_FAIL_ISSUE) ? "ancestor failed"
		                                      : "ancestor skipped";

		return false;
	}

	return true;
}

int
cute_run_setup(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern((run->state == CUTE_INIT_STATE) ||
	                   (run->state == CUTE_OFF_STATE));
	cute_assert_intern(run->issue == CUTE_UNK_ISSUE);
	cute_assert_intern(!run->file);
	cute_assert_intern(run->line == -1);
	cute_assert_intern(!run->what);
	cute_assert_intern(!run->why);
	cute_assert_intern(!run->ensure_cnt);

	int          ret = -EPERM;
	volatile int issue;

	if (!cute_run_active(run))
		goto report;

	run->state = CUTE_SETUP_STATE;

	cute_run_settle(run);

	issue = sigsetjmp(cute_jmp_env, 1);
	if (!issue) {
		run->base->setup();
		ret = 0;
		goto unsettle;
	}

	cute_assert_intern(issue == CUTE_FAIL_ISSUE);
	run->issue = CUTE_FAIL_ISSUE;

unsettle:
	cute_run_unsettle(run);
report:
	cute_run_report(run, CUTE_SETUP_EVT);

	return ret;
}

void
cute_run_teardown(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state != CUTE_TEARDOWN_STATE);
	cute_assert_intern(run->state != CUTE_DONE_STATE);
	cute_assert_intern(run->state != CUTE_FINI_STATE);
	cute_assert_intern(run->issue < CUTE_ISSUE_NR);

	volatile int issue;

	if (run->state == CUTE_INIT_STATE)
		goto report;

	run->state = CUTE_TEARDOWN_STATE;

	cute_run_settle(run);

	issue = sigsetjmp(cute_jmp_env, 1);
	if (!issue) {
		run->base->teardown();
		goto unsettle;
	}

	cute_assert_intern(issue == CUTE_FAIL_ISSUE);
	run->issue = issue;

unsettle:
	cute_run_unsettle(run);
report:
	cute_run_report(run, CUTE_TEARDOWN_EVT);
}

int
cute_run_done(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state != CUTE_EXEC_STATE);
	cute_assert_intern(run->state != CUTE_DONE_STATE);
	cute_assert_intern(run->state != CUTE_FINI_STATE);

	int ret;

	if ((run->issue == CUTE_UNK_ISSUE) &&
	    (run->state == CUTE_TEARDOWN_STATE))
		run->issue = CUTE_PASS_ISSUE;

	run->state = CUTE_DONE_STATE;
	cute_run_report(run, CUTE_DONE_EVT);

	switch (run->issue) {
	case CUTE_UNK_ISSUE:
	case CUTE_PASS_ISSUE:
	case CUTE_SKIP_ISSUE:
	case CUTE_OFF_ISSUE:
		ret = 0;
		break;

	case CUTE_FAIL_ISSUE:
		ret = -EPERM;
		break;

	default:
		__cute_unreachable();
	}

	return ret;
}

void
cute_run_oper(struct cute_run * run, enum cute_oper oper)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state != CUTE_EXEC_STATE);
	cute_assert_intern(run->state != CUTE_TEARDOWN_STATE);
	cute_assert_intern(run->state != CUTE_DONE_STATE);
	cute_assert_intern(run->state != CUTE_FINI_STATE);
	cute_assert_intern((oper == CUTE_SPAWN_OPER) ||
	                   (oper == CUTE_COMPLETE_OPER));
	cute_assert_intern(!cute_curr_run);

	cute_curr_run = run;

	run->ops->oper(run, oper);

	cute_curr_run = NULL;
}

void
cute_run_turn_on(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern((run->state == CUTE_INIT_STATE) ||
                           (run->state == CUTE_OFF_STATE));

	do {
		run->state = CUTE_INIT_STATE;
		run = run->parent;
	} while (run && (run->state == CUTE_OFF_STATE));
}

static char *
cute_run_build_name(const struct cute_base * base,
                    const struct cute_run *  parent)
{
	size_t blen;
	size_t plen;
	char * name;

	blen = strnlen(base->name, CUTE_NAME_MAX_LEN + 1);
	cute_assert(blen <= CUTE_NAME_MAX_LEN);

	if (parent) {
		plen = strnlen(parent->name, CUTE_FULL_NAME_MAX_LEN + 1);
		cute_assert((blen + 2 + plen) <= CUTE_FULL_NAME_MAX_LEN);

		name = cute_malloc(plen + 2 + blen + 1);

		memcpy(name, parent->name, plen);
		name[plen++] = ':';
		name[plen++] = ':';
	}
	else {
		plen = 0;
		name = cute_malloc(blen + 1);
	}

	memcpy(&name[plen], base->name, blen);
	name[plen + blen] = '\0';

	return name;
}

void
cute_run_init(struct cute_run *           run,
              const struct cute_run_ops * ops,
              const struct cute_base *    base,
              struct cute_run *           parent)
{
	cute_assert_intern(run);
	cute_run_assert_ops_intern(ops);
	cute_base_assert_intern(base);

	run->ops = ops;
	run->name = cute_run_build_name(base, parent);
	run->parent = parent;
        run->base = base;
	run->issue = CUTE_UNK_ISSUE;
	run->begin.tv_sec = run->begin.tv_nsec = 0;
	run->end.tv_sec = run->end.tv_nsec = 0;
	run->file = NULL;
	run->line = -1;
	run->what = NULL;
	run->why = NULL;
	run->ensure_cnt = 0;

	if (parent) {
		cute_run_assert_intern(parent);
		cute_assert_intern((parent->state == CUTE_INIT_STATE) ||
		                   (parent->state == CUTE_OFF_STATE));

		run->depth = parent->depth + 1;
		run->state = parent->state;

		parent->ops->join(parent, run);
	}
	else {
		run->depth = 0;
		run->state = CUTE_INIT_STATE;
	}
}

static void
cute_run_fini(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state != CUTE_SETUP_STATE);
	cute_assert_intern(run->state != CUTE_EXEC_STATE);
	cute_assert_intern(run->state != CUTE_TEARDOWN_STATE);
	cute_assert_intern(run->state != CUTE_FINI_STATE);

	run->ops->fini(run);
	run->state = CUTE_FINI_STATE;
	cute_run_report(run, CUTE_FINI_EVT);
	cute_free(run->name);
}

void
cute_run_destroy(struct cute_run * run)
{
	cute_run_assert_intern(run);

	cute_run_fini(run);
	cute_free(run);
}

static struct cute_iter *
cute_run_create_iter(struct cute_run * run)
{
	cute_run_assert_intern(run);

	return run->ops->iter(run);
}

void
cute_run_foreach(struct cute_run *     run,
                 cute_run_process_fn * process,
                 void *                data)
{
	cute_run_assert_intern(run);
	cute_assert_intern(process);

	struct cute_iter * iter;
	struct cute_stack  stk;

	iter = cute_run_create_iter(run);
	cute_assert_intern(iter);

	cute_stack_init(&stk);
	cute_stack_push(&stk, iter);

	process(run, CUTE_BEGIN_VISIT, data);

	do {
		iter = cute_stack_peek(&stk);
		cute_assert_intern(iter);
		if (!cute_iter_end(iter)) {
			enum cute_visit visit = CUTE_ONCE_VISIT;

			run = cute_iter_next(iter);
			iter = cute_run_create_iter(run);
			if (iter) {
				cute_stack_push(&stk, iter);
				visit = CUTE_BEGIN_VISIT;
			}

			process(run, visit, data);
			continue;
		}

		run = cute_iter_data(iter);
		cute_assert_intern(run);
		process(run, CUTE_END_VISIT, data);

		cute_stack_pop(&stk);
		cute_iter_destroy(iter);
	} while (cute_stack_count(&stk));

	cute_stack_fini(&stk);
}

void __cute_noreturn
cute_break(enum cute_issue issue,
           const char *    file,
           int             line,
           const char *    why)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(!why || why[0]);
	cute_run_assert_intern(cute_curr_run);

	volatile struct cute_run * run = cute_curr_run;

	switch (run->state) {
	case CUTE_SETUP_STATE:
		/* Cannot skip from within setup fixture. */
		cute_assert(issue == CUTE_FAIL_ISSUE);
		cute_assert_intern(!run->file);
		cute_assert_intern(run->line == -1);
		cute_assert_intern(!run->what);
		cute_assert_intern(!run->why);

		run->file = file;
		run->line = line;
		run->what = why ? "setup check failed"
		                : "setup failure requested";
		run->why = why;
		break;

	case CUTE_EXEC_STATE:
		cute_assert_intern(!run->file);
		cute_assert_intern(run->line == -1);
		cute_assert_intern(!run->what);
		cute_assert_intern(!run->why);

		run->file = file;
		run->line = line;
		if (issue == CUTE_FAIL_ISSUE) {
			run->what = why ? "exec check failed"
			                : "exec failure requested";
			run->why = why;
		}
		break;

	case CUTE_TEARDOWN_STATE:
		/* Cannot skip from within teardown fixture. */
		cute_assert(issue == CUTE_FAIL_ISSUE);

		if (!run->file && (run->line < 0) && !run->what && !run->why) {
			run->file = file;
			run->line = line;
			run->what = why ? "teardown check failed"
			                : "teardown failure requested";
			run->why = why;
		}
		break;

	default:
		__cute_unreachable();
	}

	siglongjmp(cute_jmp_env, issue);
}
