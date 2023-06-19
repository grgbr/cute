#include "run.h"
#include "report.h"
#include <unistd.h>

#define cute_test_assert(_test) \
	cute_base_assert(&(_test)->super); \
	cute_assert((_test)->exec)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_test_assert_intern(_test) \
	cute_test_assert(_test)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_test_assert_intern(_test)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static void
cute_test_report(const struct cute_run * run, enum cute_event event)
{
	cute_run_assert_intern(run);
	cute_test_assert_intern((const struct cute_test *)run->base);
	cute_event_assert_intern(event);

	cute_report_test(event, run);
}

static struct cute_iter *
cute_test_run_iter(struct cute_run * run __cute_unused)
{
	cute_run_assert_intern(run);
	cute_test_assert_intern((const struct cute_test *)run->base);

	errno = ENOENT;
	return NULL;
}

static void
cute_test_exec_run(struct cute_run * run, const struct cute_test * test)
{
	cute_run_assert_intern(run);
	cute_test_assert_intern(test);
	cute_assert_intern(run->state == CUTE_SETUP_STATE);
	cute_assert_intern(run->issue == CUTE_UNK_ISSUE);
	cute_assert_intern(!run->file);
	cute_assert_intern(run->line == -1);
	cute_assert_intern(!run->what);
	cute_assert_intern(!run->why);

	volatile int issue;

	run->state = CUTE_EXEC_STATE;

	fflush(NULL);
	sync();

	cute_run_settle(run);

	issue = sigsetjmp(cute_jmp_env, 1);
	if (!issue) {
		cute_gettime(&run->begin);
		test->exec();
		goto out;
	}

	cute_issue_assert_intern(issue);
	cute_assert_intern(issue != CUTE_UNK_ISSUE);
	cute_assert_intern(issue != CUTE_PASS_ISSUE);
	run->issue = issue;

out:
	cute_gettime(&run->end);
	cute_run_unsettle(run);

	cute_run_report(run, CUTE_EXEC_EVT);
}

static void
cute_test_oper_run(struct cute_run * run, enum cute_oper oper __cute_unused)
{
	cute_run_assert_intern(run);
	cute_test_assert_intern((const struct cute_test *)run->base);
	cute_assert_intern((run->state == CUTE_INIT_STATE) ||
	                   (run->state == CUTE_OFF_STATE));
	cute_assert_intern(run->issue == CUTE_UNK_ISSUE);
	cute_assert_intern(!run->file);
	cute_assert_intern(run->line == -1);
	cute_assert_intern(!run->what);
	cute_assert_intern(!run->why);
	cute_assert_intern(!run->ensure_cnt);
	cute_assert_intern(oper == CUTE_COMPLETE_OPER);

	if (cute_run_setup(run))
		goto done;

	cute_test_exec_run(run, (const struct cute_test *)run->base);

	cute_run_teardown(run);

done:
	cute_run_done(run);
}

static void
cute_test_sum_run(const struct cute_run * run, struct cute_stats * sums)
{
	cute_run_assert_intern(run);
	cute_test_assert_intern((const struct cute_test *)run->base);
	cute_assert_intern(run->state == CUTE_DONE_STATE);
	cute_assert_intern(sums);

	sums->total++;

	switch (run->issue) {
	case CUTE_UNK_ISSUE:
	case CUTE_OFF_ISSUE:
		return;

	case CUTE_PASS_ISSUE:
		sums->pass++;
		break;

	case CUTE_SKIP_ISSUE:
		sums->skip++;
		break;

	case CUTE_FAIL_ISSUE:
		sums->fail++;
		break;

	default:
		__cute_unreachable();
	}

	sums->exec++;
}

static const struct cute_run_ops cute_test_run_ops = {
	.report = cute_test_report,
	.iter   = cute_test_run_iter,
	.oper   = cute_test_oper_run,
	.sum    = cute_test_sum_run,
	.join   = cute_run_dummy_join,
	.fini   = cute_run_dummy_fini
};

static struct cute_iter *
cute_test_iter(const struct cute_base * test __cute_unused)
{
	cute_test_assert_intern((const struct cute_test *)test);

	errno = ENOENT;
	return NULL;
}

static struct cute_run *
cute_test_run(const struct cute_base * test, struct cute_run * parent)
{
	cute_test_assert_intern((const struct cute_test *)test);

	struct cute_run * run;

	run = cute_malloc(sizeof(*run));
	cute_run_init(run, &cute_test_run_ops, test, parent);

	return run;
}

const struct cute_ops cute_test_ops = {
	.iter = cute_test_iter,
	.run  = cute_test_run
};