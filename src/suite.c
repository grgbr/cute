#include "suite.h"
#include "report.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define cute_suite_assert(_suite) \
	cute_base_assert(&(_suite)->super); \
	cute_assert((_suite)->nr); \
	cute_assert((_suite)->tests)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_suite_assert_intern(_suite) \
	cute_suite_assert(_suite)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_suite_assert_intern(_suite)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static void
cute_suite_report(const struct cute_run * run, enum cute_event event)
{
	cute_run_assert_intern(run);
	cute_suite_assert_intern((const struct cute_suite *)run->base);
	cute_event_assert_intern(event);

	cute_report_suite(event, run);
}

static struct cute_iter *
cute_suite_run_iter(struct cute_run * run)
{
	cute_run_assert_intern(run);
	cute_suite_assert_intern((const struct cute_suite *)run->base);

	return cute_iter_create((void * const *)
	                        ((struct cute_suite_run *)run)->subs,
	                        ((const struct cute_suite *)run->base)->nr,
	                        run);
}

static void
cute_suite_complete_run(struct cute_suite_run * run)
{
	cute_run_assert_intern(&run->super);
	cute_suite_assert((const struct cute_suite *)run->super.base);
	cute_assert_intern(run->super.state != CUTE_EXEC_STATE);
	cute_assert_intern(run->super.state != CUTE_TEARDOWN_STATE);
	cute_assert_intern(run->super.state != CUTE_DONE_STATE);
	cute_assert_intern(run->super.state != CUTE_FINI_STATE);
	cute_assert_intern(!run->stats.pass);
	cute_assert_intern(!run->stats.skip);
	cute_assert_intern(!run->stats.fail);
	cute_assert_intern(!run->stats.excp);
	cute_assert_intern(!run->stats.exec);
	cute_assert_intern(run->stats.total);

	unsigned int s;
	unsigned int nr = run->stats.total;

	if ((run->super.state != CUTE_OFF_STATE) &&
	    (run->super.state != CUTE_INIT_STATE)) {
		cute_run_teardown(&run->super);
		cute_gettime(&run->super.end);
	}

	memset(&run->sums, 0, sizeof(run->sums));

	for (s = 0; s < nr; s++) {
		if (!run->subs[s])
			continue;

		switch (run->subs[s]->issue) {
		case CUTE_UNK_ISSUE:
		case CUTE_OFF_ISSUE:
			break;

		case CUTE_PASS_ISSUE:
			run->stats.pass++;
			run->stats.exec++;
			break;

		case CUTE_SKIP_ISSUE:
			run->stats.skip++;
			run->stats.exec++;
			break;

		case CUTE_FAIL_ISSUE:
			run->stats.fail++;
			run->stats.exec++;
			break;

		case CUTE_EXCP_ISSUE:
			run->stats.excp++;
			run->stats.exec++;
			break;

		default:
			__cute_unreachable();
		}

		cute_run_sum_stats(run->subs[s], &run->sums);
	}

	if (run->super.issue == CUTE_UNK_ISSUE) {
		if (run->stats.skip == run->stats.exec) {
			cute_assert_intern(!run->stats.fail);
			cute_assert_intern(!run->stats.excp);

			run->super.issue = CUTE_SKIP_ISSUE;
			run->super.what = "exec skipped";
			run->super.why = "all descendants skipped";
			cute_assess_update_source(&run->super.assess,
			                          run->super.base->file,
			                          run->super.base->line,
			                          NULL);
			cute_assess_build_expr(&run->super.assess, NULL);
		}
		else if (run->stats.fail || run->stats.excp) {
			run->super.issue = CUTE_FAIL_ISSUE;
			run->super.what = "exec failed";
			run->super.why = "descendants failed";
			cute_assess_update_source(&run->super.assess,
			                          run->super.base->file,
			                          run->super.base->line,
			                          NULL);
			cute_assess_build_expr(&run->super.assess, NULL);
		}
	}

	cute_run_done(&run->super);
}

static void
cute_suite_oper_run(struct cute_run * run, enum cute_oper oper)
{
	cute_run_assert_intern(run);
	cute_suite_assert_intern((const struct cute_suite *)run->base);
	cute_assert_intern(!((struct cute_suite_run *)run)->stats.pass);
	cute_assert_intern(!((struct cute_suite_run *)run)->stats.skip);
	cute_assert_intern(!((struct cute_suite_run *)run)->stats.fail);
	cute_assert_intern(!((struct cute_suite_run *)run)->stats.excp);
	cute_assert_intern(!((struct cute_suite_run *)run)->stats.exec);
	cute_assert_intern(((struct cute_suite_run *)run)->stats.total);

	switch (oper) {
	case CUTE_SPAWN_OPER:
		if (run->state != CUTE_OFF_STATE)
			cute_gettime(&run->begin);
		cute_run_setup(run);
		break;

	case CUTE_COMPLETE_OPER:
		cute_suite_complete_run((struct cute_suite_run *)run);
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_suite_sum_run(const struct cute_run * run, struct cute_stats * sums)
{
	cute_run_assert_intern(run);
	cute_suite_assert_intern((const struct cute_suite *)run->base);
	cute_assert_intern(run->state == CUTE_DONE_STATE);
	cute_assert_intern(((const struct cute_suite_run *)run)->stats.total);
	cute_assert_intern(sums);

	const struct cute_stats * stats;

	stats = &((const struct cute_suite_run *)run)->stats;

	sums->pass += stats->pass;
	sums->skip += stats->skip;
	sums->fail += stats->fail;
	sums->excp += stats->excp;
	sums->exec += stats->exec;
	sums->total += stats->total;
}

static void
cute_suite_join_run(struct cute_run * run, struct cute_run * sub)
{
	cute_run_assert_intern(run);

	struct cute_suite_run *   srun = (struct cute_suite_run *)run;
	const struct cute_suite * suite __cute_unused = \
		(const struct cute_suite *)run->base;

	cute_suite_assert_intern(suite);
	cute_assert_intern(suite->nr);
	cute_assert_intern((run->state == CUTE_INIT_STATE) ||
	                   (run->state == CUTE_OFF_STATE));
	cute_assert_intern(!srun->stats.pass);
	cute_assert_intern(!srun->stats.skip);
	cute_assert_intern(!srun->stats.fail);
	cute_assert_intern(!srun->stats.excp);
	cute_assert_intern(!srun->stats.exec);
	cute_assert_intern(srun->count < suite->nr);

	sub->id = (int)srun->count;
	srun->subs[srun->count++] = sub;
}

static const struct cute_run_ops cute_suite_run_ops = {
	.report = cute_suite_report,
	.iter   = cute_suite_run_iter,
	.oper   = cute_suite_oper_run,
	.sum    = cute_suite_sum_run,
	.join   = cute_suite_join_run,
	.fini   = cute_run_dummy_fini
};

static struct cute_run *
cute_suite_run(const struct cute_base * suite, struct cute_run * parent)
{
	cute_suite_assert_intern((const struct cute_suite *)suite);

	struct cute_suite_run * run;
	unsigned int            nr = ((const struct cute_suite *)suite)->nr;
	unsigned int            s;

	run = cute_malloc(sizeof(*run) + (nr * sizeof(run->subs[0])));

	for (s = 0; s < nr; s++)
		run->subs[s] = NULL;

	memset(&run->stats, 0, sizeof(run->stats));
	run->stats.total = nr;
	run->count = 0;

	cute_run_init(&run->super, &cute_suite_run_ops, suite, parent);

	return &run->super;
}

static struct cute_iter *
cute_suite_iter(const struct cute_base * suite)
{
	cute_suite_assert_intern((const struct cute_suite *)suite);

	return cute_iter_const_create((const void * const *)
	                              ((const struct cute_suite *)suite)->tests,
	                              ((const struct cute_suite *)suite)->nr,
	                              suite);
}

const struct cute_ops cute_suite_ops = {
	.iter = cute_suite_iter,
	.run  = cute_suite_run
};

/******************************************************************************
 * Run tree handling
 ******************************************************************************/

static struct cute_run * cute_suite_root_run;

static void
cute_suite_oper_tree_run(struct cute_run * run,
                         enum cute_visit   visit,
                         void *            data __cute_unused)
{
	cute_run_assert_intern(run);

	switch (visit) {
	case CUTE_BEGIN_VISIT:
		cute_run_oper(run, CUTE_SPAWN_OPER);
		break;

	case CUTE_END_VISIT:
	case CUTE_ONCE_VISIT:
		cute_run_oper(run, CUTE_COMPLETE_OPER);
		break;

	default:
		__cute_unreachable();
	}
}

static int
cute_suite_run_tree(void)
{
	cute_assert_intern(cute_suite_root_run);

	cute_run_foreach(cute_suite_root_run, cute_suite_oper_tree_run, NULL);

	switch (cute_suite_root_run->issue) {
	case CUTE_PASS_ISSUE:
	case CUTE_SKIP_ISSUE:
	case CUTE_OFF_ISSUE:
		return 0;

	case CUTE_FAIL_ISSUE:
	case CUTE_EXCP_ISSUE:
		return -EPERM;

	default:
		__cute_unreachable();
	}
}

static void
cute_suite_init_tree_run(const struct cute_base * base,
                         enum cute_visit          visit,
                         void *                   data)
{
	cute_base_assert_intern(base);
	cute_assert_intern(data);
	cute_stack_assert_intern((struct cute_stack *)data);
	cute_assert_intern(cute_stack_count((struct cute_stack *)data));
	cute_assert_intern(cute_suite_root_run);

	struct cute_stack * stk = (struct cute_stack *)data;

	switch (visit) {
	case CUTE_BEGIN_VISIT:
	case CUTE_ONCE_VISIT:
		{
			struct cute_run * run;

			run = (struct cute_run *)cute_stack_peek(stk);
			cute_run_assert_intern(run);

			run = cute_create_run(base, run);
			cute_run_assert_intern(run);

			if (visit == CUTE_BEGIN_VISIT)
				cute_stack_push(stk, run);
		}
		break;

	case CUTE_END_VISIT:
		cute_stack_pop(stk);
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_suite_enable_tree_run(struct cute_run * run,
                           enum cute_visit   visit,
                           void *            data __cute_unused)
{
	cute_run_assert_intern(run);

	switch (visit) {
	case CUTE_BEGIN_VISIT:
	case CUTE_ONCE_VISIT:
		if (cute_config_match_run(run))
			cute_run_turn_on(run);
		cute_run_report(run, CUTE_INIT_EVT);
		break;

	case CUTE_END_VISIT:
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_suite_init_tree(const struct cute_suite * suite)
{
	cute_suite_assert_intern(suite);

	struct cute_stack  stk;
	struct cute_run *  run;
	struct cute_iter * iter;

	cute_stack_init(&stk);

	run = cute_create_run(&suite->super, NULL);
	cute_run_assert_intern(run);
	if (cute_the_config->match)
		run->state = CUTE_OFF_STATE;

	cute_suite_root_run = run;

	cute_stack_push(&stk, run);
	iter = cute_base_create_iter(&suite->super);
	cute_iter_assert_intern(iter);
	while (!cute_iter_end(iter)) {
		const struct cute_base * child;

		child = cute_iter_const_next(iter);
		cute_base_assert_intern(child);

		cute_base_foreach(child, cute_suite_init_tree_run, &stk);
	}

	cute_iter_destroy(iter);

	cute_stack_fini(&stk);

	cute_run_foreach(run, cute_suite_enable_tree_run, NULL);
}

static void
cute_suite_fini_tree_run(struct cute_run * run,
                         enum cute_visit   visit,
                         void *            data __cute_unused)
{
	cute_run_assert_intern(run);

	switch (visit) {
	case CUTE_BEGIN_VISIT:
		break;

	case CUTE_END_VISIT:
	case CUTE_ONCE_VISIT:
		cute_run_destroy(run);
		break;

	default:
		__cute_unreachable();
	}
}

static void
cute_suite_fini_tree(void)
{
	if (cute_suite_root_run) {
		cute_run_foreach(cute_suite_root_run,
		                 cute_suite_fini_tree_run,
		                 NULL);

		cute_suite_root_run = NULL;
	}
}

void
cute_show_suite(const struct cute_suite * suite)
{
	cute_suite_assert(suite);
	cute_config_assert(cute_the_config);

	cute_suite_init_tree(suite);

	cute_run_report(cute_suite_root_run, CUTE_SHOW_EVT);

	cute_suite_fini_tree();
}

int
cute_run_suite(const struct cute_suite * suite)
{
	cute_suite_assert(suite);
	cute_config_assert(cute_the_config);

	int ret;

	cute_suite_init_tree(suite);

	cute_suite_report(cute_suite_root_run, CUTE_HEAD_EVT);
	ret = cute_suite_run_tree();
	cute_suite_report(cute_suite_root_run, CUTE_FOOT_EVT);

	cute_suite_fini_tree();

	return ret;
}
