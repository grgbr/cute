#include "expect.h"
#include "cute/expect.h"
#include "run.h"
#include <string.h>
#include <inttypes.h>

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

	struct cute_text_block * blk;

	blk = cute_text_create(4);

	cute_text_enroll(blk,   "wanted: none", CUTE_TEXT_LEASE);

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
cute_expect_claim_missing(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	assess->ops = &cute_expect_missing_ops;
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
			why = "mock expectation caller mismatch";
			goto fail;
		}

		return xpct;
	}

	cute_expect_claim_missing(&cute_curr_run->assess);
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

	cute_text_enroll(blk,   "wanted:", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk,
	                   "    source: %s:%d",
	                   xpct->xpct_file, xpct->xpct_line);
	cute_text_asprintf(blk, "    caller: %s()",  xpct->xpct_func);
	cute_text_asprintf(blk, "    expect: %s %s %s", parm, op, ref->expr);

	cute_text_enroll(blk,   "found: ", CUTE_TEXT_LEASE);
	cute_text_asprintf(blk, "    source: %s:%d", assess->file,
	                                             assess->line);
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
cute_expect_check_sint_parm_value(const char *             file,
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

#if 0
/******************************************************************************
 * Mock return expectation handling
 ******************************************************************************/

uintmax_t
cute_expect_check_retval(const char * file, int line, const char * function)
{
	struct cute_expect_retval * xpct =
		(struct cute_expect_retval *)
		cute_expect_check(CUTE_EXPECT_RET_TYPE, file, line, function);
	int                         ret = xpct->code;

	cute_expect_destroy(&xpct->super);

	return xpct->code;
}

void
cute_expect_push_retval(const char * file,
                        int          line,
                        const char * function,
                        uintmax_t    retval)
{
	struct cute_expect_retval * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_RET_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));
	xpct->code = retval;

	cute_expect_nqueue(xpct);
}
#endif
