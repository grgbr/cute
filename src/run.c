#include "run.h"
#include "report.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>

struct cute_run * volatile cute_curr_run;

static const char *
cute_run_what(const struct cute_run * run, enum cute_issue issue)
{
	cute_run_assert_intern(run);

	switch (run->state) {
	case CUTE_SETUP_STATE:
		switch (issue) {
		case CUTE_SKIP_ISSUE:
			return "setup skipped";

		case CUTE_FAIL_ISSUE:
			return "setup failed";

		case CUTE_EXCP_ISSUE:
			return "setup crashed";

		default:
			__cute_unreachable();
		}

		break;

	case CUTE_EXEC_STATE:
		switch (issue) {
		case CUTE_SKIP_ISSUE:
			return "exec skipped";

		case CUTE_FAIL_ISSUE:
			return "exec failed";

		case CUTE_EXCP_ISSUE:
			return "exec crashed";

		default:
			__cute_unreachable();
		}

	case CUTE_TEARDOWN_STATE:
		return (issue == CUTE_FAIL_ISSUE) ? "teardown failed"
		                                  : "teardown crashed";

	default:
		__cute_unreachable();
	}
}

void __cute_noreturn
cute_break(enum cute_issue issue,
           const char *    file,
           int             line,
           const char *    func,
           const char *    why)
{
	cute_run_assert(cute_curr_run);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(line >= 0);
	cute_assert(!func || func[0]);
	cute_assert(why[0]);

	struct cute_run * run = cute_curr_run;

	switch (run->state) {
	case CUTE_SETUP_STATE:
	case CUTE_EXEC_STATE:
		cute_assert_intern((issue == CUTE_FAIL_ISSUE) ||
		                   (issue == CUTE_EXCP_ISSUE) ||
		                   (issue == CUTE_SKIP_ISSUE));
		cute_assert_intern(!run->what);
		cute_assert_intern(!run->why);
		cute_assert_intern(!run->assess.file);
		cute_assert_intern(run->assess.line < 0);
		cute_assert_intern(!run->assess.func);

		run->what = cute_run_what(run, issue);
		run->why = why;
		cute_assess_update_source(&run->assess, file, line, func);

		break;

	case CUTE_TEARDOWN_STATE:
		/* Cannot skip from within teardown fixture. */
		cute_assert((issue == CUTE_FAIL_ISSUE) ||
		            (issue == CUTE_EXCP_ISSUE));

		if (!cute_assess_has_source(&run->assess) &&
		    !run->what &&
		    !run->why) {
			run->what = cute_run_what(run, issue);
			run->why = why;
			cute_assess_update_source(&run->assess,
			                          file,
			                          line,
			                          func);
		}

		break;

	default:
		__cute_unreachable();
	}

	siglongjmp(cute_jmp_env, issue);
}

struct cute_run_sigact {
	int              sig;
	struct sigaction old;
};

static struct cute_run_sigact cute_run_sigs[] = {
	{ .sig = SIGFPE },
	{ .sig = SIGILL },
	{ .sig = SIGSEGV },
	{ .sig = SIGBUS },
	{ .sig = SIGSYS }
};

static struct sigaction       cute_run_timer_act;

static sigset_t               cute_run_sigmask;

static stack_t                cute_run_sigstack = {
	.ss_flags = 0,
};

static void
cute_run_handle_sig(int         sig,
                    siginfo_t * info __cute_unused,
                    void *      context __cute_unused)
{
	cute_run_assert_intern(cute_curr_run);

	cute_assess_build_excp(&cute_curr_run->assess, sig);

	cute_break(CUTE_EXCP_ISSUE,
	           cute_curr_run->base->file,
	           cute_curr_run->base->line,
	           NULL,
	           "exception raised");
}

static void
cute_run_settle_sigs(void)
{
	if (!cute_the_config->debug) {
		unsigned int s;

		for (s = 0;
		     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
		     s++) {
			int                    err __cute_unused;
			const struct sigaction act = {
				.sa_sigaction = cute_run_handle_sig,
				.sa_mask      = cute_run_sigmask,
				.sa_flags     = SA_SIGINFO | SA_ONSTACK
			};

			err = sigaction(cute_run_sigs[s].sig,
			                &act,
			                &cute_run_sigs[s].old);
			cute_assert_intern(!err);
		}
	}
}

static void
cute_run_unsettle_sigs(void)
{
	if (!cute_the_config->debug) {
		unsigned int s;

		for (s = 0;
		     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
		     s++) {
			int err __cute_unused;

			err = sigaction(cute_run_sigs[s].sig,
			                &cute_run_sigs[s].old,
			                NULL);
			cute_assert_intern(!err);
		}
	}
}

static void
cute_run_handle_tmout(int         sig __cute_unused,
                      siginfo_t * info __cute_unused,
                      void *      context __cute_unused)
{
	cute_assert_intern(sig == SIGALRM);
	cute_run_assert_intern(cute_curr_run);

	if (cute_curr_run->issue != CUTE_UNK_ISSUE)
		/*
		 * Test case has completed but timer has not yet been disabled:
		 * Ignore the timeout.
		 */
		return;

	cute_assess_build_expr(&cute_curr_run->assess, NULL);

	cute_break(CUTE_FAIL_ISSUE,
	           cute_curr_run->base->file,
	           cute_curr_run->base->line,
	           NULL,
	           "timer expired");
}

static void
cute_run_arm_timer(const struct cute_run * run)
{
	if (!cute_the_config->debug) {
		/*
		 * As stated into section "Sleeping" of the glibc manual:
		 * On GNU system, it is safe to use sleep and SIGALRM in the
		 * same program, because sleep does not work by means of
		 * SIGALRM.
		 */
		if (run->tmout > 0) {
			int                    err __cute_unused;
			const struct sigaction act = {
				.sa_sigaction = cute_run_handle_tmout,
				.sa_mask      = cute_run_sigmask,
				.sa_flags     = SA_SIGINFO | SA_ONSTACK
			};

			err = sigaction(SIGALRM, &act, &cute_run_timer_act);
			cute_assert_intern(!err);

			alarm(run->tmout);
		}
	}
}

static void
cute_run_disarm_timer(const struct cute_run * run)
{
	if (!cute_the_config->debug) {
		if (run->tmout > 0) {
			int err __cute_unused;

			alarm(0);

			err = sigaction(SIGALRM, &cute_run_timer_act, NULL);
			cute_assert_intern(!err);
		}
	}
}

void
cute_run_init_sigs(void)
{
	if (!cute_the_config->debug) {
		int           err __cute_unused;
		unsigned int  s;

		sigemptyset(&cute_run_sigmask);
		err = sigaddset(&cute_run_sigmask, SIGALRM);
		cute_assert_intern(!err);

		for (s = 0;
		     s < (sizeof(cute_run_sigs) / sizeof(cute_run_sigs[0]));
		     s++) {
			err = sigaddset(&cute_run_sigmask,
			                cute_run_sigs[s].sig);
			cute_assert_intern(!err);
		}

		cute_run_sigstack.ss_size = (size_t)SIGSTKSZ;
		cute_run_sigstack.ss_sp = cute_malloc((size_t)SIGSTKSZ);
		err = sigaltstack(&cute_run_sigstack, NULL);
		cute_assert_intern(!err);
	}
}

void
cute_run_fini_sigs(void)
{
	if (!cute_the_config->debug) {
		const stack_t stk = { .ss_flags = SS_DISABLE };
		int           err __cute_unused;

		err = sigaltstack(&stk, NULL);
		cute_assert_intern(!err);

		cute_free(cute_run_sigstack.ss_sp);
	}
}

void
cute_run_settle(struct cute_run * run)
{
	cute_run_assert_intern(run);

	cute_run_settle_sigs();

	cute_iodir_redirect(&run->ioout, &run->ioerr);

	cute_run_arm_timer(run);
}

void
cute_run_unsettle(struct cute_run * run)
{
	cute_run_assert_intern(run);

	cute_run_disarm_timer(run);

	cute_iodir_restore();

	cute_run_unsettle_sigs();
}

int
cute_run_done(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_assert_intern(run->state != CUTE_EXEC_STATE);
	cute_assert_intern(run->state != CUTE_DONE_STATE);
	cute_assert_intern(run->state != CUTE_FINI_STATE);

	int ret;

	if (run->issue == CUTE_UNK_ISSUE) {
		cute_assert_intern(run->state == CUTE_TEARDOWN_STATE);
		cute_assert_intern(!run->what);
		cute_assert_intern(!run->why);
		cute_assert_intern(!run->assess.file);
		cute_assert_intern(run->assess.line < 0);
		cute_assert_intern(!run->assess.func);

		if (cute_expect_release(&run->call, true)) {
			run->issue = CUTE_FAIL_ISSUE;
			run->what = cute_run_what(run, CUTE_FAIL_ISSUE);
			run->why = "extra mock expectation left";
		}
		else if (cute_expect_check_assert()) {
			run->issue = CUTE_FAIL_ISSUE;
			run->what = cute_run_what(run, CUTE_FAIL_ISSUE);
			run->why = "extra expected assertion left";
			run->assess.file = run->base->file;
			run->assess.line = run->base->line;
		}
		else
			run->issue = CUTE_PASS_ISSUE;

	}
	else if (run->issue != CUTE_OFF_ISSUE) {
		cute_assert_intern(run->what);
		cute_assert_intern(run->why);
		cute_assert_intern(run->assess.file);
		cute_assert_intern(run->assess.file[0]);
		cute_assert_intern(run->assess.line >= 0);
		cute_assert_intern(!run->assess.func || run->assess.func[0]);

		cute_expect_release(&run->call, false);
	}

	run->state = CUTE_DONE_STATE;

	cute_run_report(run, CUTE_DONE_EVT);

	switch (run->issue) {
	case CUTE_PASS_ISSUE:
	case CUTE_SKIP_ISSUE:
	case CUTE_OFF_ISSUE:
		ret = 0;
		break;

	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		ret = -EPERM;
		break;

	case CUTE_UNK_ISSUE:
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
	run->id = -1;
	run->base = base;
	run->issue = CUTE_UNK_ISSUE;
	run->begin.tv_sec = run->begin.tv_nsec = 0;
	run->end.tv_sec = run->end.tv_nsec = 0;
	run->what = NULL;
	run->why = NULL;
	cute_assess_build_null(&run->assess);
	cute_iodir_init_block(&run->ioout);
	cute_iodir_init_block(&run->ioerr);

	if (parent) {
		cute_run_assert_intern(parent);
		cute_assert_intern((parent->state == CUTE_INIT_STATE) ||
		                   (parent->state == CUTE_OFF_STATE));
		cute_assert_intern(parent->setup != CUTE_INHR_SETUP);
		cute_assert_intern(parent->teardown != CUTE_INHR_TEARDOWN);
		cute_assert_intern(parent->tmout != CUTE_INHR_TMOUT);

		run->depth = parent->depth + 1;

		if (base->setup == CUTE_INHR_SETUP)
			run->setup = parent->setup;
		else
			run->setup = base->setup;

		if (base->teardown == CUTE_INHR_TEARDOWN)
			run->teardown = parent->teardown;
		else
			run->teardown = base->teardown;

		run->state = parent->state;

		if (base->tmout == CUTE_INHR_TMOUT)
			run->tmout = parent->tmout;
		else
			run->tmout = base->tmout;

		parent->ops->join(parent, run);
	}
	else {
		run->depth = 0;

		if (base->setup == CUTE_INHR_SETUP)
			run->setup = NULL;
		else
			run->setup = base->setup;

		if (base->teardown == CUTE_INHR_TEARDOWN)
			run->teardown = NULL;
		else
			run->teardown = base->teardown;

		run->state = CUTE_INIT_STATE;

		if (base->tmout == CUTE_INHR_TMOUT)
			run->tmout = CUTE_DFLT_TMOUT;
		else
			run->tmout = base->tmout;
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
	cute_iodir_fini_block(&run->ioout);
	cute_iodir_fini_block(&run->ioerr);
	cute_assess_release(&run->assess);
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

void
_cute_skip(const char * reason,
           const char * file,
           int          line,
           const char * function)
{
	cute_assert(!reason || reason[0]);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	cute_assess_build_expr(&cute_curr_run->assess, reason);

	cute_break(CUTE_SKIP_ISSUE,
	           file,
	           line,
	           function,
	           "explicit skip requested");
}

void
_cute_fail(const char * reason,
           const char * file,
           int          line,
           const char * function)
{
	cute_assert(!reason || reason[0]);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	cute_assess_build_expr(&cute_curr_run->assess, reason);

	cute_break(CUTE_FAIL_ISSUE,
	           file,
	           line,
	           function,
	           "explicit fail requested");
}
