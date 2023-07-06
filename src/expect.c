#include "expect.h"
#include "cute/expect.h"
#include "run.h"
#include <string.h>

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

static const char *
cute_expect_type_label(enum cute_expect_type type)
{
	cute_expect_assert_type_intern(type);

	static const char * labels[] = {
		[CUTE_EXPECT_CALL_TYPE] = "mocked function call",
		[CUTE_EXPECT_PARM_TYPE] = "mocked function parameter",
		[CUTE_EXPECT_RET_TYPE]  = "mocked return value"
	};

	return labels[type];
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
cute_expect_claim_inval_type(struct cute_expect *  expect,
                             enum cute_expect_type type)
{
	cute_expect_assert_intern(expect);
	cute_expect_assert_type_intern(type);

	expect->super.ops = &cute_expect_inval_type_ops;
	expect->got_type = type;
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
	cute_assess_assert_intern(&expect->super);
	cute_expect_assert_intern(orig);
	cute_expect_assert_type_intern(type);

	expect->super.ops = &cute_expect_inval_call_ops;
	expect->next = NULL;
	expect->xpct_type = orig->xpct_type;
	expect->xpct_file = orig->xpct_file;
	expect->xpct_line = orig->xpct_line;
	expect->xpct_func = orig->xpct_func;
	expect->got_type = type;
}

static struct cute_text_block *
cute_expect_desc_missing(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	struct cute_text_block * blk;

	blk = cute_text_create(6);

	cute_text_enroll(blk,   "wanted:", CUTE_TEXT_LEASE);
	cute_text_enroll(blk,   "    source: ??", CUTE_TEXT_LEASE);
	cute_text_enroll(blk,   "    caller: ??", CUTE_TEXT_LEASE);

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
			                             type);
			why = "erroneous mock expectation type";
			goto fail;
		}

		if (strcmp(function, xpct->xpct_func)) {
			cute_expect_claim_inval_call(&cute_curr_run->call,
			                             xpct,
			                             type);
			why = "erroneous mock expectation caller";
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

#warning FIXME: save error at cute_expect_release() time; \
                make sure expectations are not released twice at \
                cute_expect_claim_inval_type(), cute_expect_claim_inval_call() \
                and cute_expect_claim_missing() time

void
cute_expect_release(void)
{
	struct cute_expect * xpct;

	while (!cute_expect_empty(&cute_expect_done)) {
		xpct = cute_expect_dqueue(&cute_expect_done);
		cute_expect_destroy(xpct);
	}

	while (!cute_expect_empty(&cute_expect_sched)) {
		xpct = cute_expect_dqueue(&cute_expect_sched);
		cute_expect_destroy(xpct);
	}
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
	if (xpct)
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

#if 0
/******************************************************************************
 * Mock parameter expectation handling
 ******************************************************************************/

static void
cute_expect_desc_inval_parm(const struct cute_run * run,
                            FILE *                  stdio,
                            int                     indent,
                            const char *            normal,
                            const char *            emphasis,
                            const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$s    expect: %8$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %9$s:%10$s\n"
	        "%3$*2$s    caller: %11$s\n"
	        "%3$*2$s    actual: %12$s%13$s\n"
	        "%14$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        ((struct cute_expect_parm *)xpct)->xpct_parm,
	        run->file, run->line,
	        run->func,
	        emphasis, ((struct cute_expect_parm *)xpct)->chk_parm,
	        regular);
}

static void
cute_expect_desc_inval_parm_val(const struct cute_run * run,
                                FILE *                  stdio,
                                int                     indent,
                                const char *            normal,
                                const char *            emphasis,
                                const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$s    expect: %8$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %9$s:%10$s\n"
	        "%3$*2$s    caller: %11$s\n"
	        "%3$*2$s    actual: %12$s%13$s\n"
	        "%14$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        ((struct cute_expect_parm *)xpct)->xpct_parm,
	        run->file, run->line,
	        run->func,
	        emphasis, ((struct cute_expect_parm *)xpct)->chk_parm,
	        regular);
}

void
cute_expect_check_parm(const char * file,
                       int          line,
                       const char * function,
                       const char * parameter,
                       uintmax_t    value)
{
	cute_assert_intern(parameter);
	cute_assert_intern(parameter[0]);

	struct cute_expect_parm * xpct = (struct cute_expect_parm *)
	                                 cute_expect_check(CUTE_EXPECT_PARM_TYPE,
	                                                   file,
	                                                   line,
	                                                   function);
	cute_run_desc_fn *        desc;
	const char *              why;


	cute_assert_intern(xpct->xpct_parm);
	cute_assert_intern(xpct->xpct_parm[0]);
	cute_assert_intern(xpct->chk_parm);
	cute_assert_intern(xpct->chk_parm[0]);

	if (strcmp(parameter, xpct->xpct_parm)) {
		desc = cute_expect_desc_inval_parm;
		why = "erroneous mock expectation parameter name";
		goto break;
	}

	if (value != parm->xpct_val) {
		xpct->chk_val = value;
		desc = cute_expect_desc_inval_parm_val;
		why = "erroneous mock expectation parameter value";
		goto break;
	}

	cute_expect_destroy(xpct);

	return;

break:
	xpct->got_type = type;
	xpct->chk_parm = parameter;
	cute_curr_run->xpct = &xpct->super;

	/* Does not return !! */
	cute_break(CUTE_FAIL_ISSUE, file, line, function, desc, why);
}

void
cute_expect_push_parm(const char * file,
                      int          line,
                      const char * function,
                      const char * parameter,
                      uintmax_t    value)
{
	cute_assert_intern(parameter);
	cute_assert_intern(parameter[0]);

	struct cute_expect_parm * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_PARM_TYPE;
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));
	xpct->xpct_parm = parameter;
	xpct->xpct_val = value;

	cute_expect_nqueue(&xpct->super);
}

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
