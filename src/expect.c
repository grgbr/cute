#include "expect.h"
#include "cute/expect.h"
#include "run.h"
#include <string.h>
#include <inttypes.h>

/******************************************************************************
 * Assertion mock expectation handling
 ******************************************************************************/

sigjmp_buf cute_expect_assert_env;
bool       cute_expect_assert;

bool
cute_expect_check_assert(void)
{
	if (cute_expect_assert) {
		cute_expect_assert = false;
		return true;
	}

	return false;
}

void
cute_expect_fail_assert(const char * file, int line, const char * function)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	cute_assert_intern(cute_expect_assert);
	cute_expect_assert = false;

	cute_assess_build_expr(&cute_curr_run->assess, NULL);

	cute_break(CUTE_FAIL_ISSUE,
	           file,
	           line,
	           function,
	           "expected assertion missed");
}

void
cute_mock_assert(const char * expression,
                 const char * file,
                 unsigned int line,
                 const char * function)
{
	cute_assert(expression);
	cute_assert(expression[0]);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line <= INT_MAX);
	cute_assert(function);
	cute_assert(function[0]);

	if (cute_expect_check_assert())
		/* Does not return. */
		siglongjmp(cute_expect_assert_env, 1);

	cute_assess_build_assert(&cute_curr_run->assess, expression);

	cute_break(CUTE_FAIL_ISSUE,
	           file,
	           (int)line,
	           function,
	           "uncaught assertion failure");
}

/******************************************************************************
 * Mock expectation generic handling
 ******************************************************************************/

typedef void (cute_run_desc_fn)(const struct cute_run * run,
                                FILE *                  stdio,
                                int                     indent,
                                const char *            normal,
                                const char *            emphasis,
                                const char *            regular);

#define cute_expect_assert_type(_type) \
	cute_assert((_type) >= 0); \
	cute_assert((_type) < CUTE_EXPECT_TYPE_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_expect_assert_type_intern(_type) \
	cute_expect_assert_type(_type)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_expect_assert_type_intern(_type)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_expect_assert(_expect) \
	cute_assert(_expect); \
	cute_assess_assert(&(_expect)->super); \
	cute_expect_assert_type((_expect)->xpct_type); \
	cute_assert((_expect)->xpct_file); \
	cute_assert((_expect)->xpct_file[0]); \
	cute_assert((_expect)->xpct_line >= 0); \
	cute_assert((_expect)->xpct_func); \
	cute_assert((_expect)->xpct_func[0])

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_expect_assert_intern(_expect) \
	cute_expect_assert(_expect)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_expect_assert_intern(_expect)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

struct cute_expect_queue {
	struct cute_expect * head;
	struct cute_expect * tail;
};

static struct cute_expect_queue cute_expect_sched;
static struct cute_expect_queue cute_expect_done;

static bool
cute_expect_empty(const struct cute_expect_queue * queue)
{
	cute_assert_intern(!(!!queue->head ^ !!queue->tail));

	return !queue->tail;
}

static void
cute_expect_nqueue(struct cute_expect_queue * queue,
                   struct cute_expect *       expect)
{
	cute_assert_intern(!(!!queue->head ^ !!queue->tail));
	cute_expect_assert_intern(expect);
	cute_assert(!expect->next);

	if (queue->head)
		queue->tail->next = expect;
	else
		queue->head = expect;

	queue->tail = expect;
}

static struct cute_expect *
cute_expect_dqueue(struct cute_expect_queue * queue)
{
	cute_assert_intern(queue->head);
	cute_assert_intern(queue->tail);

	struct cute_expect * xpct = queue->head;

	queue->head = xpct->next;
	if (!queue->head)
		queue->tail = NULL;
	else
		xpct->next = NULL;

	cute_expect_assert_intern(xpct);

	return xpct;
}

static void
cute_expect_join_queue(struct cute_expect_queue * before,
                       struct cute_expect_queue * after)
{
	if (before->head) {
		cute_assert_intern(before->tail);
		cute_assert_intern(!before->tail->next);

		if (!after->head) {
			cute_assert_intern(!after->tail);
			return;
		}

		cute_assert_intern(after->tail);
		cute_assert_intern(!after->tail->next);

		before->tail->next = after->head;
		before->tail = after->tail;

	}
	else {
		cute_assert_intern(!before->tail);

		if (!after->head) {
			cute_assert_intern(!after->tail);
			return;
		}

		cute_assert_intern(after->tail);
		cute_assert_intern(!after->tail->next);

		before->head = after->head;
		before->tail = after->tail;
	}

	after->head = after->tail = NULL;
}

static const char *
cute_expect_type_label(enum cute_expect_type type)
{
	cute_expect_assert_type_intern(type);

	static const char * labels[] = {
		[CUTE_EXPECT_CALL_TYPE] = "mock function call",
		[CUTE_EXPECT_PARM_TYPE] = "mock function parameter",
		[CUTE_EXPECT_RET_TYPE]  = "mock function return value"
	};

	return labels[type];
}

static void
cute_expect_claim_inval(struct cute_expect *           expect,
                        const struct cute_assess_ops * ops,
                        const struct cute_expect *     orig,
                        enum cute_expect_type          type)
{
	cute_assess_assert_intern(&expect->super);
	cute_assess_assert_ops_intern(ops);
	cute_expect_assert_intern(orig);
	cute_expect_assert_type_intern(type);

	expect->super.ops = ops;
	expect->next = NULL;
	expect->xpct_type = orig->xpct_type;
	expect->xpct_file = orig->xpct_file;
	expect->xpct_line = orig->xpct_line;
	expect->xpct_func = orig->xpct_func;
	expect->got_type = type;
}

static struct cute_text_block *
cute_expect_desc_inval_type(const struct cute_assess * assess)
{
	cute_expect_assert_intern((const struct cute_expect *)assess);

	struct cute_text_block *   blk;
	const char *               type;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;

	blk = cute_text_create(8);

	type = cute_expect_type_label(xpct->xpct_type);
	cute_text_enroll(blk,   "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->xpct_file,
	                                             xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s",    type);

	type = cute_expect_type_label(xpct->got_type);
	cute_text_enroll(blk,   "found:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->super.file,
	                                             xpct->super.line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->super.func);
	cute_text_asprintf(blk, "    found:  %s",    type);

	return blk;
}

static const struct cute_assess_ops cute_expect_inval_type_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_expect_desc_inval_type,
	.release = cute_assess_release_null
};

static void
cute_expect_claim_inval_type(struct cute_expect *       expect,
                             const struct cute_expect * orig,
                             enum cute_expect_type      type)
{
	cute_expect_claim_inval(expect,
	                        &cute_expect_inval_type_ops,
	                        orig,
	                        type);
}

static struct cute_text_block *
cute_expect_desc_inval_call(const struct cute_assess * assess)
{
	cute_expect_assert_intern((const struct cute_expect *)assess);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;

	blk = cute_text_create(6);

	cute_text_enroll(blk,   "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->xpct_file,
	                                             xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);

	cute_text_enroll(blk,   "found:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->super.file,
	                                             xpct->super.line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->super.func);

	return blk;
}

static const struct cute_assess_ops cute_expect_inval_call_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_expect_desc_inval_call,
	.release = cute_assess_release_null
};

static void
cute_expect_claim_inval_call(struct cute_expect *       expect,
                             const struct cute_expect * orig,
                             enum cute_expect_type      type)
{
	cute_expect_claim_inval(expect,
	                        &cute_expect_inval_call_ops,
	                        orig,
	                        type);
}

static struct cute_text_block *
cute_expect_desc_missing(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;

	blk = cute_text_create(4);

	cute_text_enroll(blk,   "wanted:     ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->xpct_file,
	                                             xpct->xpct_line);

	cute_text_enroll(blk,   "found:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", assess->file,
	                                             assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	return blk;
}

static const struct cute_assess_ops cute_expect_missing_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_expect_desc_missing,
	.release = cute_assess_release_null
};

static void
cute_expect_claim_missing(struct cute_expect * expect,
                          const char *         file,
                          int                  line)
{
	cute_assert_intern(expect);
	cute_assess_assert_intern(&expect->super);
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);

	expect->super.ops = &cute_expect_missing_ops;
	expect->xpct_file = file;
	expect->xpct_line = line;
	expect->xpct_func = NULL;
}

static struct cute_expect *
cute_expect_check(enum cute_expect_type type,
                  const char *          file,
                  int                   line,
                  const char *          function)
{
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(function);
	cute_assert_intern(function[0]);
	cute_assert_intern(cute_curr_run);

	struct cute_expect * xpct = NULL;
	const char *         why;


	if (!cute_expect_empty(&cute_expect_sched)) {
		xpct = cute_expect_dqueue(&cute_expect_sched);
		cute_expect_assert_intern(xpct);

		if (type != xpct->xpct_type) {
			cute_expect_claim_inval_type(&cute_curr_run->call,
			                             xpct,
			                             type);
			why = "mock expectation type mismatch";
			goto fail;
		}

		if (strcmp(function, xpct->xpct_func)) {
			cute_expect_claim_inval_call(&cute_curr_run->call,
			                             xpct,
			                             type);
			why = "mock caller function mismatch";
			goto fail;
		}

		return xpct;
	}

	cute_expect_claim_missing(&cute_curr_run->call,
	                          cute_curr_run->base->file,
	                          cute_curr_run->base->line);
	why = "missing mock expectation";

fail:
	if (xpct)
		cute_expect_nqueue(&cute_expect_done, xpct);

	/* Do not return... */
	cute_break(CUTE_FAIL_ISSUE, file, line, function, why);
}

static struct cute_expect *
cute_expect_create(enum cute_expect_type type,
                   const char *          file,
                   int                   line,
                   const char *          function,
                   size_t                size)
{
	cute_expect_assert_type_intern(type);
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(function);
	cute_assert_intern(function[0]);
	cute_assert_intern(size >= sizeof(struct cute_expect));

	struct cute_expect * xpct;

	xpct = cute_malloc(size);

	cute_assess_build_null(&xpct->super);
	xpct->next = NULL;
	xpct->xpct_type = type;
	xpct->xpct_file = file;
	xpct->xpct_line = line;
	xpct->xpct_func = function;
	xpct->got_type = CUTE_EXPECT_TYPE_NR;

	return xpct;
}

static void
cute_expect_destroy(struct cute_expect * expect)
{
	cute_expect_assert_intern(expect);

	cute_assess_release(&expect->super);
	cute_free(expect);
}

static struct cute_text_block *
cute_expect_desc_xcess(const struct cute_assess * assess)
{
	cute_expect_assert_intern((const struct cute_expect *)assess);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;
	const char *               type;

	blk = cute_text_create(5);

	type = cute_expect_type_label(xpct->xpct_type);
	cute_text_enroll(blk,   "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", xpct->xpct_file,
	                                             xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s",    type);

	cute_text_enroll(blk,   "found:  none", CUTE_TEXT_LEASE);

	return blk;
}

static const struct cute_assess_ops cute_expect_xcess_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_expect_desc_xcess,
	.release = cute_assess_release_null
};

static void
cute_expect_claim_xcess(struct cute_expect *       expect,
                        const struct cute_expect * orig)
{
	cute_assert_intern(expect);
	cute_expect_assert_intern(orig);

	expect->super.ops = &cute_expect_xcess_ops;
	expect->next = NULL;
	expect->xpct_type = orig->xpct_type;
	expect->xpct_file = orig->xpct_file;
	expect->xpct_line = orig->xpct_line;
	expect->xpct_func = orig->xpct_func;
}

int
cute_expect_release(struct cute_expect * expect, bool check)
{
	struct cute_expect * xpct;
	int                  ret = 0;

	if (check && !cute_expect_empty(&cute_expect_sched)) {
		xpct = cute_expect_dqueue(&cute_expect_sched);

		cute_expect_claim_xcess(expect, xpct);

		cute_expect_nqueue(&cute_expect_done, xpct);

		ret = -EBUSY;
	}

	cute_expect_join_queue(&cute_expect_done, &cute_expect_sched);

	while (!cute_expect_empty(&cute_expect_done)) {
		xpct = cute_expect_dqueue(&cute_expect_done);
		cute_expect_destroy(xpct);
	}

	return ret;
}

/******************************************************************************
 * Mock call expectation handling
 ******************************************************************************/

void
cute_expect_check_call(const char * file, int line, const char * function)
{
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(function);
	cute_assert_intern(function[0]);

	struct cute_expect * xpct;

	xpct = cute_expect_check(CUTE_EXPECT_CALL_TYPE, file, line, function);
	cute_assert_intern(xpct);

	cute_expect_nqueue(&cute_expect_done, xpct);
}

void
cute_expect_sched_call(const char * file, int line, const char * function)
{
	struct cute_expect * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_CALL_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	cute_expect_nqueue(&cute_expect_sched, xpct);
}

/******************************************************************************
 * Signed integer mock parameter expectation handling
 ******************************************************************************/

static struct cute_text_block *
cute_expect_desc_sint_parm_value(const struct cute_assess * assess,
                                 const char *               op,
                                 const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;
	const char *               parm = ((const struct cute_expect_parm *)
	                                   assess)->xpct_parm;
	const struct cute_sint *   chk = &assess->check.sint;
	const struct cute_sint *   ref = &assess->expect.sint.scal;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s %s %s", parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [%" PRIdMAX "] "
		                   "%s "
		                   "[%" PRIdMAX "]",
		                   chk->value,
		                   inv,
		                   ref->value);

	return blk;
}

static void
cute_expect_sched_sint_parm_value(const char *                   file,
                                  int                            line,
                                  const char *                   function,
                                  const struct cute_assess_ops * ops,
                                  const char *                   parm,
                                  const struct cute_sint *       reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.sint.scal = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

void
cute_expect_check_sint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);

	struct cute_expect *          xpct;
	const char *                  parm;
	struct cute_assess *          assess;
	const char *                  why;
	const union cute_assess_value chk = { .sint = *check };

	xpct = cute_expect_check(CUTE_EXPECT_PARM_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	parm = ((struct cute_expect_parm *)xpct)->xpct_parm;
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);

	cute_expect_nqueue(&cute_expect_done, xpct);

	assess = &xpct->super;
	if (strcmp(check->expr, parm)) {
		why = "signed integer mock parameter name mismatch";
		goto fail;
	}

	if (!assess->ops->cmp(assess, &chk)) {
		why = "signed integer mock parameter check failed";
		goto fail;
	}

	return;

fail:
	assess->check.sint = *check;
	cute_curr_run->parm = *(struct cute_expect_parm *)xpct;
	assess->ops = &cute_assess_null_ops;

	cute_break(CUTE_FAIL_ISSUE, file, line, function, why);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, "==", "!=");
}

static const struct cute_assess_ops cute_expect_sint_parm_equal_ops = {
	.cmp     = cute_assess_cmp_sint_equal,
	.desc    = cute_expect_desc_sint_parm_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_sint_parm_equal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_unequal(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, "!=", "==");
}

static const struct cute_assess_ops cute_expect_sint_parm_unequal_ops = {
	.cmp     = cute_assess_cmp_sint_unequal,
	.desc    = cute_expect_desc_sint_parm_unequal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_sint_parm_unequal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_greater(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, ">", "<=");
}

static const struct cute_assess_ops cute_expect_sint_parm_greater_ops = {
	.cmp     = cute_assess_cmp_sint_greater,
	.desc    = cute_expect_desc_sint_parm_greater,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_sint_parm_greater_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_greater_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, ">=", "<");
}

static const struct cute_assess_ops cute_expect_sint_parm_greater_equal_ops = {
	.cmp     = cute_assess_cmp_sint_greater_equal,
	.desc    = cute_expect_desc_sint_parm_greater_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(
		file,
		line,
		function,
		&cute_expect_sint_parm_greater_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_lower(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, "<", ">=");
}

static const struct cute_assess_ops cute_expect_sint_parm_lower_ops = {
	.cmp     = cute_assess_cmp_sint_lower,
	.desc    = cute_expect_desc_sint_parm_lower,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_sint_parm_lower_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_lower_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_value(assess, "<=", ">");
}

static const struct cute_assess_ops cute_expect_sint_parm_lower_equal_ops = {
	.cmp     = cute_assess_cmp_sint_lower_equal,
	.desc    = cute_expect_desc_sint_parm_lower_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_sint * expect)
{
	cute_expect_sched_sint_parm_value(
		file,
		line,
		function,
		&cute_expect_sint_parm_lower_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_range(const struct cute_assess * assess,
                                 const char *               op,
                                 const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *       blk;
	const struct cute_expect *     xpct = (const struct cute_expect *)
	                                      assess;
	const char *                   parm = ((const struct cute_expect_parm *)
	                                       assess)->xpct_parm;
	const struct cute_sint *       chk = &assess->check.sint;
	const struct cute_sint_range * ref = &assess->expect.sint.range;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);
	cute_assert_intern(ref->min <= ref->max);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s range %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s range %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [%" PRIdMAX "] "
		                   "%s "
		                   "range {%" PRIdMAX " ... %" PRIdMAX "}",
		                   chk->value,
		                   inv,
		                   ref->min, ref->max);

	return blk;
}

static void
cute_expect_sched_sint_parm_range(const char *                   file,
                                  int                            line,
                                  const char *                   function,
                                  const struct cute_assess_ops * ops,
                                  const char *                   parm,
                                  const struct cute_sint_range * reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);
	cute_assert(reference->min <= reference->max);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.sint.range = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_sint_parm_in_range = {
	.cmp     = cute_assess_cmp_sint_in_range,
	.desc    = cute_expect_desc_sint_parm_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_sint_range * expect)
{
	cute_expect_sched_sint_parm_range(
		file,
		line,
		function,
		&cute_expect_sint_parm_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_not_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_sint_parm_not_in_range = {
	.cmp     = cute_assess_cmp_sint_not_in_range,
	.desc    = cute_expect_desc_sint_parm_not_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_sint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_sint_range * expect)
{
	cute_expect_sched_sint_parm_range(
		file,
		line,
		function,
		&cute_expect_sint_parm_not_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_set(const struct cute_assess * assess,
                               const char *               op,
                               const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *       blk;
	const struct cute_expect *     xpct = (const struct cute_expect *)
	                                      assess;
	const char *                   parm = ((const struct cute_expect_parm *)
	                                       assess)->xpct_parm;
	const struct cute_sint *       chk = &assess->check.sint;
	const struct cute_sint_set *   ref = &assess->expect.sint.set;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s set %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (!strcmp(parm, chk->expr)) {
		/* Unexpected parameter value. */
		if (ref->count) {
			char * items;

			items = cute_assess_sint_set_str(ref->items,
			                                 ref->count);
			cute_text_asprintf(blk,
			                   "    actual: [%" PRIdMAX "] "
			                   "%s "
			                   "set {%s}",
			                   assess->check.sint.value,
			                   inv,
			                   items);
			cute_free(items);
		}
		else
			cute_text_asprintf(blk,
			                   "    actual: [%" PRIdMAX "] "
			                   "%s "
			                   "set {}",
			                   chk->value,
			                   inv);
	}
	else
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s set %s",
		                   chk->expr, op, ref->expr);

	return blk;
}

static void
cute_expect_sched_sint_parm_set(const char *                   file,
                                int                            line,
                                const char *                   function,
                                const struct cute_assess_ops * ops,
                                const char *                   parm,
                                const struct cute_sint_set *   reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	intmax_t *                items;
	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	if (reference->count) {
		unsigned int i;

		items = cute_malloc(reference->count * sizeof(items[0]));
		for (i = 0; i < reference->count; i++)
			items[i] = reference->items[i];
	}
	else
		items = NULL;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.sint.set = *reference;
	assess->expect.sint.set.items = items;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_sint_parm_in_set = {
	.cmp     = cute_assess_cmp_sint_in_set,
	.desc    = cute_expect_desc_sint_parm_in_set,
	.release = cute_assess_release_sint_set
};

void
cute_expect_sched_sint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_sint_set * expect)
{
	cute_expect_sched_sint_parm_set(
		file,
		line,
		function,
		&cute_expect_sint_parm_in_set,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_sint_parm_not_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_sint_parm_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_sint_parm_not_in_set = {
	.cmp     = cute_assess_cmp_sint_not_in_set,
	.desc    = cute_expect_desc_sint_parm_not_in_set,
	.release = cute_assess_release_sint_set
};

void
cute_expect_sched_sint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_sint_set * expect)
{
	cute_expect_sched_sint_parm_set(
		file,
		line,
		function,
		&cute_expect_sint_parm_not_in_set,
		parm,
		expect);
}

/******************************************************************************
 * Signed integer return value expectation handling
 ******************************************************************************/

intmax_t
cute_expect_check_sint_retval(const char * file,
                              int          line,
                              const char * function)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	struct cute_expect * xpct;

	xpct = cute_expect_check(CUTE_EXPECT_RET_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	cute_expect_nqueue(&cute_expect_done, xpct);

	return xpct->super.expect.sint.scal.value;
}

void
cute_expect_sched_sint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * retval)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(retval);
	cute_assert(retval->expr);
	cute_assert(retval->expr[0]);

	struct cute_expect * xpct;
	struct cute_assess * assess;

	xpct = cute_expect_create(CUTE_EXPECT_RET_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super;
	assess->ops = &cute_assess_null_ops;
	assess->expect.sint.scal = *retval;

	cute_expect_nqueue(&cute_expect_sched, xpct);
}

/******************************************************************************
 * Unsigned integer mock parameter expectation handling
 ******************************************************************************/

static struct cute_text_block *
cute_expect_desc_uint_parm_value(const struct cute_assess * assess,
                                 const char *               op,
                                 const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;
	const char *               parm = ((const struct cute_expect_parm *)
	                                   assess)->xpct_parm;
	const struct cute_uint *   chk = &assess->check.uint;
	const struct cute_uint *   ref = &assess->expect.uint.scal;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s %s %s", parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [%" PRIuMAX "] "
		                   "%s "
		                   "[%" PRIuMAX "]",
		                   chk->value,
		                   inv,
		                   ref->value);

	return blk;
}

static void
cute_expect_sched_uint_parm_value(const char *                   file,
                                  int                            line,
                                  const char *                   function,
                                  const struct cute_assess_ops * ops,
                                  const char *                   parm,
                                  const struct cute_uint *       reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.uint.scal = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

void
cute_expect_check_uint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);

	struct cute_expect *          xpct;
	const char *                  parm;
	struct cute_assess *          assess;
	const char *                  why;
	const union cute_assess_value chk = { .uint = *check };

	xpct = cute_expect_check(CUTE_EXPECT_PARM_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	parm = ((struct cute_expect_parm *)xpct)->xpct_parm;
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);

	cute_expect_nqueue(&cute_expect_done, xpct);

	assess = &xpct->super;
	if (strcmp(check->expr, parm)) {
		why = "unsigned integer mock parameter name mismatch";
		goto fail;
	}

	if (!assess->ops->cmp(assess, &chk)) {
		why = "unsigned integer mock parameter check failed";
		goto fail;
	}

	return;

fail:
	assess->check.uint = *check;
	cute_curr_run->parm = *(struct cute_expect_parm *)xpct;
	assess->ops = &cute_assess_null_ops;

	cute_break(CUTE_FAIL_ISSUE, file, line, function, why);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, "==", "!=");
}

static const struct cute_assess_ops cute_expect_uint_parm_equal_ops = {
	.cmp     = cute_assess_cmp_uint_equal,
	.desc    = cute_expect_desc_uint_parm_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_uint_parm_equal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_unequal(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, "!=", "==");
}

static const struct cute_assess_ops cute_expect_uint_parm_unequal_ops = {
	.cmp     = cute_assess_cmp_uint_unequal,
	.desc    = cute_expect_desc_uint_parm_unequal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_uint_parm_unequal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_greater(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, ">", "<=");
}

static const struct cute_assess_ops cute_expect_uint_parm_greater_ops = {
	.cmp     = cute_assess_cmp_uint_greater,
	.desc    = cute_expect_desc_uint_parm_greater,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_uint_parm_greater_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_greater_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, ">=", "<");
}

static const struct cute_assess_ops cute_expect_uint_parm_greater_equal_ops = {
	.cmp     = cute_assess_cmp_uint_greater_equal,
	.desc    = cute_expect_desc_uint_parm_greater_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(
		file,
		line,
		function,
		&cute_expect_uint_parm_greater_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_lower(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, "<", ">=");
}

static const struct cute_assess_ops cute_expect_uint_parm_lower_ops = {
	.cmp     = cute_assess_cmp_uint_lower,
	.desc    = cute_expect_desc_uint_parm_lower,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_uint_parm_lower_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_lower_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_value(assess, "<=", ">");
}

static const struct cute_assess_ops cute_expect_uint_parm_lower_equal_ops = {
	.cmp     = cute_assess_cmp_uint_lower_equal,
	.desc    = cute_expect_desc_uint_parm_lower_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_uint * expect)
{
	cute_expect_sched_uint_parm_value(
		file,
		line,
		function,
		&cute_expect_uint_parm_lower_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_range(const struct cute_assess * assess,
                                 const char *               op,
                                 const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *       blk;
	const struct cute_expect *     xpct = (const struct cute_expect *)
	                                      assess;
	const char *                   parm = ((const struct cute_expect_parm *)
	                                       assess)->xpct_parm;
	const struct cute_uint *       chk = &assess->check.uint;
	const struct cute_uint_range * ref = &assess->expect.uint.range;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);
	cute_assert_intern(ref->min <= ref->max);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s range %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s range %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [%" PRIuMAX "] "
		                   "%s "
		                   "range {%" PRIuMAX " ... %" PRIuMAX "}",
		                   chk->value,
		                   inv,
		                   ref->min, ref->max);

	return blk;
}

static void
cute_expect_sched_uint_parm_range(const char *                   file,
                                  int                            line,
                                  const char *                   function,
                                  const struct cute_assess_ops * ops,
                                  const char *                   parm,
                                  const struct cute_uint_range * reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);
	cute_assert(reference->min <= reference->max);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.uint.range = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_uint_parm_in_range = {
	.cmp     = cute_assess_cmp_uint_in_range,
	.desc    = cute_expect_desc_uint_parm_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_uint_range * expect)
{
	cute_expect_sched_uint_parm_range(
		file,
		line,
		function,
		&cute_expect_uint_parm_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_not_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_uint_parm_not_in_range = {
	.cmp     = cute_assess_cmp_uint_not_in_range,
	.desc    = cute_expect_desc_uint_parm_not_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_uint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_uint_range * expect)
{
	cute_expect_sched_uint_parm_range(
		file,
		line,
		function,
		&cute_expect_uint_parm_not_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_set(const struct cute_assess * assess,
                               const char *               op,
                               const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *       blk;
	const struct cute_expect *     xpct = (const struct cute_expect *)
	                                      assess;
	const char *                   parm = ((const struct cute_expect_parm *)
	                                       assess)->xpct_parm;
	const struct cute_uint *       chk = &assess->check.uint;
	const struct cute_uint_set *   ref = &assess->expect.uint.set;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s set %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (!strcmp(parm, chk->expr)) {
		/* Unexpected parameter value. */
		if (ref->count) {
			char * items;

			items = cute_assess_uint_set_str(ref->items,
			                                 ref->count);
			cute_text_asprintf(blk,
			                   "    actual: [%" PRIuMAX "] "
			                   "%s "
			                   "set {%s}",
			                   assess->check.uint.value,
			                   inv,
			                   items);
			cute_free(items);
		}
		else
			cute_text_asprintf(blk,
			                   "    actual: [%" PRIuMAX "] "
			                   "%s "
			                   "set {}",
			                   chk->value,
			                   inv);
	}
	else
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s set %s",
		                   chk->expr, op, ref->expr);

	return blk;
}

static void
cute_expect_sched_uint_parm_set(const char *                   file,
                                int                            line,
                                const char *                   function,
                                const struct cute_assess_ops * ops,
                                const char *                   parm,
                                const struct cute_uint_set *   reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	uintmax_t *               items;
	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	if (reference->count) {
		unsigned int i;

		items = cute_malloc(reference->count * sizeof(items[0]));
		for (i = 0; i < reference->count; i++)
			items[i] = reference->items[i];
	}
	else
		items = NULL;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.uint.set = *reference;
	assess->expect.uint.set.items = items;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_uint_parm_in_set = {
	.cmp     = cute_assess_cmp_uint_in_set,
	.desc    = cute_expect_desc_uint_parm_in_set,
	.release = cute_assess_release_uint_set
};

void
cute_expect_sched_uint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_uint_set * expect)
{
	cute_expect_sched_uint_parm_set(
		file,
		line,
		function,
		&cute_expect_uint_parm_in_set,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_uint_parm_not_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_uint_parm_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_uint_parm_not_in_set = {
	.cmp     = cute_assess_cmp_uint_not_in_set,
	.desc    = cute_expect_desc_uint_parm_not_in_set,
	.release = cute_assess_release_uint_set
};

void
cute_expect_sched_uint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_uint_set * expect)
{
	cute_expect_sched_uint_parm_set(
		file,
		line,
		function,
		&cute_expect_uint_parm_not_in_set,
		parm,
		expect);
}

/******************************************************************************
 * Unsigned integer return value expectation handling
 ******************************************************************************/

uintmax_t
cute_expect_check_uint_retval(const char * file,
                              int          line,
                              const char * function)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	struct cute_expect * xpct;

	xpct = cute_expect_check(CUTE_EXPECT_RET_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	cute_expect_nqueue(&cute_expect_done, xpct);

	return xpct->super.expect.uint.scal.value;
}

void
cute_expect_sched_uint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * retval)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(retval);
	cute_assert(retval->expr);
	cute_assert(retval->expr[0]);

	struct cute_expect * xpct;
	struct cute_assess * assess;

	xpct = cute_expect_create(CUTE_EXPECT_RET_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super;
	assess->ops = &cute_assess_null_ops;
	assess->expect.uint.scal = *retval;

	cute_expect_nqueue(&cute_expect_sched, xpct);
}

/******************************************************************************
 * Floating point number mock parameter expectation handling
 ******************************************************************************/

static struct cute_text_block *
cute_expect_desc_flt_parm_value(const struct cute_assess * assess,
                                const char *               op,
                                const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *   blk;
	const struct cute_expect * xpct = (const struct cute_expect *)assess;
	const char *               parm = ((const struct cute_expect_parm *)
	                                   assess)->xpct_parm;
	const struct cute_flt *    chk = &assess->check.flt;
	const struct cute_flt *    ref = &assess->expect.flt.scal;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s %s %s", parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [" CUTE_FLT_FORMAT_STR "] "
		                   "%s "
		                   "[" CUTE_FLT_FORMAT_STR "]",
		                   chk->value,
		                   inv,
		                   ref->value);

	return blk;
}

static void
cute_expect_sched_flt_parm_value(const char *                   file,
                                 int                            line,
                                 const char *                   function,
                                 const struct cute_assess_ops * ops,
                                 const char *                   parm,
                                 const struct cute_flt *        reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.flt.scal = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

void
cute_expect_check_flt_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);

	struct cute_expect *          xpct;
	const char *                  parm;
	struct cute_assess *          assess;
	const char *                  why;
	const union cute_assess_value chk = { .flt = *check };

	xpct = cute_expect_check(CUTE_EXPECT_PARM_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	parm = ((struct cute_expect_parm *)xpct)->xpct_parm;
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);

	cute_expect_nqueue(&cute_expect_done, xpct);

	assess = &xpct->super;
	if (strcmp(check->expr, parm)) {
		why = "floating point mock parameter name mismatch";
		goto fail;
	}

	if (!assess->ops->cmp(assess, &chk)) {
		why = "floating point mock parameter check failed";
		goto fail;
	}

	return;

fail:
	assess->check.flt = *check;
	cute_curr_run->parm = *(struct cute_expect_parm *)xpct;
	assess->ops = &cute_assess_null_ops;

	cute_break(CUTE_FAIL_ISSUE, file, line, function, why);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, "==", "!=");
}

static const struct cute_assess_ops cute_expect_flt_parm_equal_ops = {
	.cmp     = cute_assess_cmp_flt_equal,
	.desc    = cute_expect_desc_flt_parm_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_flt_parm_equal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_unequal(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, "!=", "==");
}

static const struct cute_assess_ops cute_expect_flt_parm_unequal_ops = {
	.cmp     = cute_assess_cmp_flt_unequal,
	.desc    = cute_expect_desc_flt_parm_unequal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(file,
	                                  line,
	                                  function,
	                                  &cute_expect_flt_parm_unequal_ops,
	                                  parm,
	                                  expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_greater(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, ">", "<=");
}

static const struct cute_assess_ops cute_expect_flt_parm_greater_ops = {
	.cmp     = cute_assess_cmp_flt_greater,
	.desc    = cute_expect_desc_flt_parm_greater,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_greater(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(file,
	                                 line,
	                                 function,
	                                 &cute_expect_flt_parm_greater_ops,
	                                 parm,
	                                 expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_greater_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, ">=", "<");
}

static const struct cute_assess_ops cute_expect_flt_parm_greater_equal_ops = {
	.cmp     = cute_assess_cmp_flt_greater_equal,
	.desc    = cute_expect_desc_flt_parm_greater_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_greater_equal(const char *            file,
                                         int                     line,
                                         const char *            function,
                                         const char *            parm,
                                         const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(
		file,
		line,
		function,
		&cute_expect_flt_parm_greater_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_lower(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, "<", ">=");
}

static const struct cute_assess_ops cute_expect_flt_parm_lower_ops = {
	.cmp     = cute_assess_cmp_flt_lower,
	.desc    = cute_expect_desc_flt_parm_lower,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_lower(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(file,
	                                 line,
	                                 function,
	                                 &cute_expect_flt_parm_lower_ops,
	                                 parm,
	                                 expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_lower_equal(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_value(assess, "<=", ">");
}

static const struct cute_assess_ops cute_expect_flt_parm_lower_equal_ops = {
	.cmp     = cute_assess_cmp_flt_lower_equal,
	.desc    = cute_expect_desc_flt_parm_lower_equal,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_lower_equal(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_flt * expect)
{
	cute_expect_sched_flt_parm_value(
		file,
		line,
		function,
		&cute_expect_flt_parm_lower_equal_ops,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_range(const struct cute_assess * assess,
                                const char *               op,
                                const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *      blk;
	const struct cute_expect *    xpct = (const struct cute_expect *)
	                                     assess;
	const char *                  parm = ((const struct cute_expect_parm *)
	                                      assess)->xpct_parm;
	const struct cute_flt *       chk = &assess->check.flt;
	const struct cute_flt_range * ref = &assess->expect.flt.range;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);
	cute_assert_intern(ref->min <= ref->max);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s range %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (strcmp(parm, chk->expr))
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s range %s",
		                   chk->expr, op, ref->expr);
	else
		/* Unexpected parameter value. */
		cute_text_asprintf(blk,
		                   "    actual: [" CUTE_FLT_FORMAT_STR "] "
		                   "%s range"
		                   "{" CUTE_FLT_FORMAT_STR
		                   " ... "
		                   CUTE_FLT_FORMAT_STR "}",
		                   chk->value,
		                   inv,
		                   ref->min, ref->max);

	return blk;
}

static void
cute_expect_sched_flt_parm_range(const char *                   file,
                                 int                            line,
                                 const char *                   function,
                                 const struct cute_assess_ops * ops,
                                 const char *                   parm,
                                 const struct cute_flt_range *  reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);
	cute_assert(reference->min <= reference->max);

	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.flt.range = *reference;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_flt_parm_in_range = {
	.cmp     = cute_assess_cmp_flt_in_range,
	.desc    = cute_expect_desc_flt_parm_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_in_range(const char *                  file,
                                    int                           line,
                                    const char *                  function,
                                    const char *                  parm,
                                    const struct cute_flt_range * expect)
{
	cute_expect_sched_flt_parm_range(
		file,
		line,
		function,
		&cute_expect_flt_parm_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_not_in_range(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_flt_parm_not_in_range = {
	.cmp     = cute_assess_cmp_flt_not_in_range,
	.desc    = cute_expect_desc_flt_parm_not_in_range,
	.release = cute_assess_release_null
};

void
cute_expect_sched_flt_parm_not_in_range(
	const char *                  file,
	int                           line,
	const char *                  function,
	const char *                  parm,
	const struct cute_flt_range * expect)
{
	cute_expect_sched_flt_parm_range(
		file,
		line,
		function,
		&cute_expect_flt_parm_not_in_range,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_set(const struct cute_assess * assess,
                              const char *               op,
                              const char *               inv)
{
	cute_assert_intern(assess);
	cute_assert_intern(op);
	cute_assert_intern(op[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *      blk;
	const struct cute_expect *    xpct = (const struct cute_expect *)
	                                     assess;
	const char *                  parm = ((const struct cute_expect_parm *)
	                                      assess)->xpct_parm;
	const struct cute_flt *       chk = &assess->check.flt;
	const struct cute_flt_set *   ref = &assess->expect.flt.set;

	cute_expect_assert_intern(xpct);
	cute_assert_intern(parm);
	cute_assert_intern(parm[0]);
	cute_assert_intern(chk->expr);
	cute_assert_intern(chk->expr[0]);
	cute_assert_intern(ref->expr);
	cute_assert_intern(ref->expr[0]);

	blk = cute_text_create(8);

	cute_text_enroll(blk, "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk,
	                   "    expect: %s %s set %s",
	                   parm, op, ref->expr);

	cute_text_enroll(blk, "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   assess->file, assess->line);
	cute_text_asprintf(blk, "    caller: %s()",  assess->func);

	if (!strcmp(parm, chk->expr)) {
		/* Unexpected parameter value. */
		if (ref->count) {
			char * items;

			items = cute_assess_flt_set_str(ref->items, ref->count);
			cute_text_asprintf(blk,
			                   "    actual: [" CUTE_FLT_FORMAT_STR
			                   "] %s "
			                   "set {%s}",
			                   assess->check.flt.value,
			                   inv,
			                   items);
			cute_free(items);
		}
		else
			cute_text_asprintf(blk,
			                   "    actual: [" CUTE_FLT_FORMAT_STR
			                   "] %s "
			                   "set {}",
			                   chk->value,
			                   inv);
	}
	else
		/*
		 * Parameter name checked using cute_mock_...() does not match
		 * parameter name scheduled using cute_expect_...().
		 */
		cute_text_asprintf(blk,
		                   "    expect: %s %s set %s",
		                   chk->expr, op, ref->expr);

	return blk;
}

static void
cute_expect_sched_flt_parm_set(const char *                   file,
                               int                            line,
                               const char *                   function,
                               const struct cute_assess_ops * ops,
                               const char *                   parm,
                               const struct cute_flt_set *    reference)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(parm);
	cute_assert(parm[0]);
	cute_assert(reference);
	cute_assert(reference->expr);
	cute_assert(reference->expr[0]);

	long double *             items;
	struct cute_expect_parm * xpct;
	struct cute_assess *      assess;

	if (reference->count) {
		unsigned int i;

		items = cute_malloc(reference->count * sizeof(items[0]));
		for (i = 0; i < reference->count; i++)
			items[i] = reference->items[i];
	}
	else
		items = NULL;

	xpct = (struct cute_expect_parm *)
	       cute_expect_create(CUTE_EXPECT_PARM_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super.super;
	assess->ops = ops;
	assess->expect.flt.set = *reference;
	assess->expect.flt.set.items = items;
	xpct->xpct_parm = parm;

	cute_expect_nqueue(&cute_expect_sched, &xpct->super);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_expect_flt_parm_in_set = {
	.cmp     = cute_assess_cmp_flt_in_set,
	.desc    = cute_expect_desc_flt_parm_in_set,
	.release = cute_assess_release_flt_set
};

void
cute_expect_sched_flt_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_flt_set * expect)
{
	cute_expect_sched_flt_parm_set(
		file,
		line,
		function,
		&cute_expect_flt_parm_in_set,
		parm,
		expect);
}

static struct cute_text_block *
cute_expect_desc_flt_parm_not_in_set(const struct cute_assess * assess)
{
	return cute_expect_desc_flt_parm_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_expect_flt_parm_not_in_set = {
	.cmp     = cute_assess_cmp_flt_not_in_set,
	.desc    = cute_expect_desc_flt_parm_not_in_set,
	.release = cute_assess_release_flt_set
};

void
cute_expect_sched_flt_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_flt_set * expect)
{
	cute_expect_sched_flt_parm_set(
		file,
		line,
		function,
		&cute_expect_flt_parm_not_in_set,
		parm,
		expect);
}

/******************************************************************************
 * Floating point number return value expectation handling
 ******************************************************************************/

long double
cute_expect_check_flt_retval(const char * file, int line, const char * function)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);

	struct cute_expect * xpct;

	xpct = cute_expect_check(CUTE_EXPECT_RET_TYPE, file, line, function);
	cute_expect_assert_intern(xpct);

	cute_expect_nqueue(&cute_expect_done, xpct);

	return xpct->super.expect.flt.scal.value;
}

void
cute_expect_sched_flt_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * retval)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(retval);
	cute_assert(retval->expr);
	cute_assert(retval->expr[0]);

	struct cute_expect * xpct;
	struct cute_assess * assess;

	xpct = cute_expect_create(CUTE_EXPECT_RET_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	assess = &xpct->super;
	assess->ops = &cute_assess_null_ops;
	assess->expect.flt.scal = *retval;

	cute_expect_nqueue(&cute_expect_sched, xpct);
}
