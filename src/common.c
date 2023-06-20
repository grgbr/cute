#include "common.h"
#include "run.h"
#include "report.h"
#include <stdlib.h>
#include <string.h>

sigjmp_buf   cute_jmp_env;
unsigned int cute_run_nr;

const char *
cute_state_label(enum cute_state state)
{
	cute_state_assert_intern(state);

	static const char * const labels[] = {
		[CUTE_INIT_STATE]     = "init",
		[CUTE_SETUP_STATE]    = "setup",
		[CUTE_EXEC_STATE]     = "exec",
		[CUTE_TEARDOWN_STATE] = "teardown",
		[CUTE_DONE_STATE]     = "done",
		[CUTE_OFF_STATE]      = "off",
		[CUTE_FINI_STATE]     = "fini"
	};

	return labels[state];
}

const char *
cute_issue_label(enum cute_issue issue)
{
	cute_issue_assert_intern(issue);

	static const char * const labels[] = {
		[CUTE_UNK_ISSUE]  = "unk",
		[CUTE_PASS_ISSUE] = "pass",
		[CUTE_SKIP_ISSUE] = "skip",
		[CUTE_FAIL_ISSUE] = "fail",
		[CUTE_EXCP_ISSUE] = "excp",
		[CUTE_OFF_ISSUE]  = "off"
	};

	return labels[issue];
}

struct cute_iter *
cute_base_create_iter(const struct cute_base * base)
{
	cute_base_assert_intern(base);

	return base->ops->iter(base);
}

struct cute_run *
cute_create_run(const struct cute_base * base, struct cute_run * parent)
{
	cute_base_assert_intern(base);

	struct cute_run * run;

	run = base->ops->run(base, parent);

	cute_run_nr++;

	return run;
}

void
cute_base_foreach(const struct cute_base * root,
                  cute_base_process_fn *   process,
                  void *                   data)
{
	cute_base_assert_intern(root);
	cute_assert_intern(process);

	struct cute_iter * iter;
	struct cute_stack  stk;

	iter = cute_base_create_iter(root);
	if (!iter) {
		process(root, CUTE_ONCE_VISIT, data);
		return;
	}

	cute_assert_intern(iter);

	cute_stack_init(&stk);
	cute_stack_push(&stk, iter);

	process(root, CUTE_BEGIN_VISIT, data);

	do {
		const struct cute_base * base;

		iter = cute_stack_peek(&stk);
		cute_assert_intern(iter);
		if (!cute_iter_end(iter)) {
			enum cute_visit visit = CUTE_ONCE_VISIT;

			base = cute_iter_const_next(iter);
			iter = cute_base_create_iter(base);
			if (iter) {
				cute_stack_push(&stk, iter);
				visit = CUTE_BEGIN_VISIT;
			}

			process(base, visit, data);
			continue;
		}

		base = cute_iter_const_data(iter);
		cute_assert_intern(base);
		process(base, CUTE_END_VISIT, data);

		cute_stack_pop(&stk);
		cute_iter_destroy(iter);
	} while (cute_stack_count(&stk));

	cute_stack_fini(&stk);
}
