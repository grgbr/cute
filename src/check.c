#include "cute/check.h"
#include "run.h"
#include "assess.h"
#include <inttypes.h>

void
__cute_check_assert(bool         fail,
                    const char * file,
                    int          line,
                    const char * function,
                    const char * expr)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(expr);
	cute_assert(expr[0]);

	if (!fail)
		return;

	cute_assess_build_assert(&cute_curr_run->assess, expr);

	cute_break(CUTE_FAIL_ISSUE,
	           file,
	           line,
	           function,
	           "assertion check failed");
}

/******************************************************************************
 * Signed integer numbers handling
 ******************************************************************************/

static struct cute_text_block *
cute_check_desc_sint(const struct cute_assess * assess,
                     const char *               oper,
                     const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.sint.expr);
	cute_assert_intern(assess->check.sint.expr[0]);
	cute_assert_intern(assess->expect.sint.scal.expr);
	cute_assert_intern(assess->expect.sint.scal.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s %s",
	                   assess->check.sint.expr,
	                   oper,
	                   assess->expect.sint.scal.expr);

	cute_text_asprintf(blk,
	                   "found:  [%" PRIdMAX "] %s [%" PRIdMAX "]",
	                   assess->check.sint.value,
	                   inv,
	                   assess->expect.sint.scal.value);

	return blk;
}

static void
cute_check_assess_sint(const char *                   file,
                       int                            line,
                       const char *                   function,
                       const struct cute_assess_ops * ops,
                       const struct cute_sint *       check,
                       const struct cute_sint *       expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .sint = *check };

	assess->ops = ops;
	assess->expect.sint.scal = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer value check failed");
}

static struct cute_text_block *
cute_check_desc_sint_equal(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, "==", "!=");
}

static const struct cute_assess_ops cute_assess_sint_equal_ops = {
	.cmp     = cute_assess_cmp_sint_equal,
	.desc    = cute_check_desc_sint_equal,
	.release = cute_assess_release_null
};

void
cute_check_sint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_unequal(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, "!=", "==");
}

static const struct cute_assess_ops cute_assess_sint_unequal_ops = {
	.cmp     = cute_assess_cmp_sint_unequal,
	.desc    = cute_check_desc_sint_unequal,
	.release = cute_assess_release_null
};

void
cute_check_sint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_unequal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_greater(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, ">", "<=");
}

static const struct cute_assess_ops cute_assess_sint_greater_ops = {
	.cmp     = cute_assess_cmp_sint_greater,
	.desc    = cute_check_desc_sint_greater,
	.release = cute_assess_release_null
};

void
cute_check_sint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_greater_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_greater_equal(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, ">=", "<");
}

static const struct cute_assess_ops cute_assess_sint_greater_equal_ops = {
	.cmp     = cute_assess_cmp_sint_greater_equal,
	.desc    = cute_check_desc_sint_greater_equal,
	.release = cute_assess_release_null
};

void
cute_check_sint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * check,
                              const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_greater_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_lower(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, "<", ">=");
}

static const struct cute_assess_ops cute_assess_sint_lower_ops = {
	.cmp     = cute_assess_cmp_sint_lower,
	.desc    = cute_check_desc_sint_lower,
	.release = cute_assess_release_null
};

void
cute_check_sint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_lower_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_lower_equal(const struct cute_assess * assess)
{
	return cute_check_desc_sint(assess, "<=", ">");
}

static const struct cute_assess_ops cute_assess_sint_lower_equal_ops = {
	.cmp     = cute_assess_cmp_sint_lower_equal,
	.desc    = cute_check_desc_sint_lower_equal,
	.release = cute_assess_release_null
};

void
cute_check_sint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check,
                            const struct cute_sint * expect)
{
	cute_check_assess_sint(file,
	                       line,
	                       function,
	                       &cute_assess_sint_lower_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_sint_range(const struct cute_assess * assess,
                           const char *               oper,
                           const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.sint.expr);
	cute_assert_intern(assess->check.sint.expr[0]);
	cute_assert_intern(assess->expect.sint.range.expr);
	cute_assert_intern(assess->expect.sint.range.expr[0]);
	cute_assert_intern(assess->expect.sint.range.min <=
	                   assess->expect.sint.range.max);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s range %s",
	                   assess->check.sint.expr,
	                   oper,
	                   assess->expect.sint.range.expr);

	cute_text_asprintf(blk,
	                   "found:  [%" PRIdMAX "] "
	                   "%s "
	                   "range {%" PRIdMAX " ... %" PRIdMAX "}",
	                   assess->check.sint.value,
	                   inv,
	                   assess->expect.sint.range.min,
	                   assess->expect.sint.range.max);

	return blk;
}

static void
cute_check_assess_sint_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_assess_ops * ops,
                             const struct cute_sint *       check,
                             const struct cute_sint_range * expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .sint = *check };

	assess->ops = ops;
	assess->expect.sint.range = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer range check failed");
}

static struct cute_text_block *
cute_check_desc_sint_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_sint_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_sint_in_range_ops = {
	.cmp     = cute_assess_cmp_sint_in_range,
	.desc    = cute_check_desc_sint_in_range,
	.release = cute_assess_release_null
};

void
cute_check_sint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_sint *       check,
                         const struct cute_sint_range * expect)
{
	cute_check_assess_sint_range(file,
	                             line,
	                             function,
	                             &cute_assess_sint_in_range_ops,
	                             check,
	                             expect);
}

static struct cute_text_block *
cute_check_desc_sint_not_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_sint_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_sint_not_in_range_ops = {
	.cmp     = cute_assess_cmp_sint_not_in_range,
	.desc    = cute_check_desc_sint_not_in_range,
	.release = cute_assess_release_null
};

void
cute_check_sint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_sint *       check,
                             const struct cute_sint_range * expect)
{
	cute_check_assess_sint_range(file,
	                             line,
	                             function,
	                             &cute_assess_sint_not_in_range_ops,
	                             check,
	                             expect);
}

static struct cute_text_block *
cute_check_desc_sint_set(const struct cute_assess * assess,
                         const char *               oper,
                         const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.sint.expr);
	cute_assert_intern(assess->check.sint.expr[0]);
	cute_assert_intern(assess->expect.sint.set.expr);
	cute_assert_intern(assess->expect.sint.set.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *     blk;
	const struct cute_sint_set * set = &assess->expect.sint.set;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s set %s",
	                   assess->check.sint.expr,
	                   oper,
	                   set->expr);


	if (set->count) {
		char * items;

		items = cute_assess_sint_set_str(set->items, set->count);
		cute_text_asprintf(blk,
		                   "found:  [%" PRIdMAX "] %s set {%s}",
		                   assess->check.sint.value,
		                   inv,
		                   items);
		cute_free(items);
	}
	else
		cute_text_asprintf(blk,
		                   "found:  [%" PRIdMAX "] %s set {}",
		                   assess->check.sint.value,
		                   inv);

	return blk;
}

static void
cute_check_assess_sint_set(const char *                   file,
                           int                            line,
                           const char *                   function,
                           const struct cute_assess_ops * ops,
                           const struct cute_sint *       check,
                           const struct cute_sint_set *   expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	intmax_t *                    items;
	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .sint = *check };

	if (expect->count) {
		unsigned int i;

		items = cute_malloc(expect->count * sizeof(items[0]));
		for (i = 0; i < expect->count; i++)
			items[i] = expect->items[i];
	}
	else
		items = NULL;

	assess->ops = ops;
	assess->expect.sint.set = *expect;
	assess->expect.sint.set.items = items;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer set check failed");
}

static struct cute_text_block *
cute_check_desc_sint_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_sint_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_sint_in_set_ops = {
	.cmp     = cute_assess_cmp_sint_in_set,
	.desc    = cute_check_desc_sint_in_set,
	.release = cute_assess_release_sint_set
};

void
cute_check_sint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_sint *     check,
                       const struct cute_sint_set * expect)
{
	cute_check_assess_sint_set(file,
	                           line,
	                           function,
	                           &cute_assess_sint_in_set_ops,
	                           check,
	                           expect);
}

static struct cute_text_block *
cute_check_desc_sint_not_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_sint_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_sint_not_in_set_ops = {
	.cmp     = cute_assess_cmp_sint_not_in_set,
	.desc    = cute_check_desc_sint_not_in_set,
	.release = cute_assess_release_sint_set
};

void
cute_check_sint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_sint *     check,
                           const struct cute_sint_set * expect)
{
	cute_check_assess_sint_set(file,
	                           line,
	                           function,
	                           &cute_assess_sint_not_in_set_ops,
	                           check,
	                           expect);
}

/******************************************************************************
 * Unsigned integer numbers handling
 ******************************************************************************/

static struct cute_text_block *
cute_check_desc_uint(const struct cute_assess * assess,
                     const char *               oper,
                     const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.uint.expr);
	cute_assert_intern(assess->check.uint.expr[0]);
	cute_assert_intern(assess->expect.uint.scal.expr);
	cute_assert_intern(assess->expect.uint.scal.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s %s",
	                   assess->check.uint.expr,
	                   oper,
	                   assess->expect.uint.scal.expr);

	cute_text_asprintf(blk,
	                   "found:  [%" PRIuMAX "] %s [%" PRIuMAX "]",
	                   assess->check.uint.value,
	                   inv,
	                   assess->expect.uint.scal.value);

	return blk;
}

static void
cute_check_assess_uint(const char *                   file,
                       int                            line,
                       const char *                   function,
                       const struct cute_assess_ops * ops,
                       const struct cute_uint *       check,
                       const struct cute_uint *       expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .uint = *check };

	assess->ops = ops;
	assess->expect.uint.scal = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer value check failed");
}

static struct cute_text_block *
cute_check_desc_uint_equal(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, "==", "!=");
}

static const struct cute_assess_ops cute_assess_uint_equal_ops = {
	.cmp     = cute_assess_cmp_uint_equal,
	.desc    = cute_check_desc_uint_equal,
	.release = cute_assess_release_null
};

void
cute_check_uint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_unequal(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, "!=", "==");
}

static const struct cute_assess_ops cute_assess_uint_unequal_ops = {
	.cmp     = cute_assess_cmp_uint_unequal,
	.desc    = cute_check_desc_uint_unequal,
	.release = cute_assess_release_null
};

void
cute_check_uint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_unequal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_greater(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, ">", "<=");
}

static const struct cute_assess_ops cute_assess_uint_greater_ops = {
	.cmp     = cute_assess_cmp_uint_greater,
	.desc    = cute_check_desc_uint_greater,
	.release = cute_assess_release_null
};

void
cute_check_uint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_greater_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_greater_equal(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, ">=", "<");
}

static const struct cute_assess_ops cute_assess_uint_greater_equal_ops = {
	.cmp     = cute_assess_cmp_uint_greater_equal,
	.desc    = cute_check_desc_uint_greater_equal,
	.release = cute_assess_release_null
};

void
cute_check_uint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * check,
                              const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_greater_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_lower(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, "<", ">=");
}

static const struct cute_assess_ops cute_assess_uint_lower_ops = {
	.cmp     = cute_assess_cmp_uint_lower,
	.desc    = cute_check_desc_uint_lower,
	.release = cute_assess_release_null
};

void
cute_check_uint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_lower_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_lower_equal(const struct cute_assess * assess)
{
	return cute_check_desc_uint(assess, "<=", ">");
}

static const struct cute_assess_ops cute_assess_uint_lower_equal_ops = {
	.cmp     = cute_assess_cmp_uint_lower_equal,
	.desc    = cute_check_desc_uint_lower_equal,
	.release = cute_assess_release_null
};

void
cute_check_uint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check,
                            const struct cute_uint * expect)
{
	cute_check_assess_uint(file,
	                       line,
	                       function,
	                       &cute_assess_uint_lower_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_uint_range(const struct cute_assess * assess,
                           const char *               oper,
                           const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.uint.expr);
	cute_assert_intern(assess->check.uint.expr[0]);
	cute_assert_intern(assess->expect.uint.range.expr);
	cute_assert_intern(assess->expect.uint.range.expr[0]);
	cute_assert_intern(assess->expect.uint.range.min <=
	                   assess->expect.uint.range.max);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s range %s",
	                   assess->check.uint.expr,
	                   oper,
	                   assess->expect.uint.range.expr);

	cute_text_asprintf(blk,
	                   "found:  [%" PRIuMAX "] "
	                   "%s "
	                   "range {%" PRIuMAX " ... %" PRIuMAX "}",
	                   assess->check.uint.value,
	                   inv,
	                   assess->expect.uint.range.min,
	                   assess->expect.uint.range.max);

	return blk;
}

static void
cute_check_assess_uint_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_assess_ops * ops,
                             const struct cute_uint *       check,
                             const struct cute_uint_range * expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .uint = *check };

	assess->ops = ops;
	assess->expect.uint.range = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer range check failed");
}

static struct cute_text_block *
cute_check_desc_uint_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_uint_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_uint_in_range_ops = {
	.cmp     = cute_assess_cmp_uint_in_range,
	.desc    = cute_check_desc_uint_in_range,
	.release = cute_assess_release_null
};

void
cute_check_uint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_uint *       check,
                         const struct cute_uint_range * expect)
{
	cute_check_assess_uint_range(file,
	                             line,
	                             function,
	                             &cute_assess_uint_in_range_ops,
	                             check,
	                             expect);
}

static struct cute_text_block *
cute_check_desc_uint_not_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_uint_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_uint_not_in_range_ops = {
	.cmp     = cute_assess_cmp_uint_not_in_range,
	.desc    = cute_check_desc_uint_not_in_range,
	.release = cute_assess_release_null
};

void
cute_check_uint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_uint *       check,
                             const struct cute_uint_range * expect)
{
	cute_check_assess_uint_range(file,
	                             line,
	                             function,
	                             &cute_assess_uint_not_in_range_ops,
	                             check,
	                             expect);
}

static struct cute_text_block *
cute_check_desc_uint_set(const struct cute_assess * assess,
                         const char *               oper,
                         const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.uint.expr);
	cute_assert_intern(assess->check.uint.expr[0]);
	cute_assert_intern(assess->expect.uint.set.expr);
	cute_assert_intern(assess->expect.uint.set.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *     blk;
	const struct cute_uint_set * set = &assess->expect.uint.set;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s set %s",
	                   assess->check.uint.expr,
	                   oper,
	                   set->expr);

	if (set->count) {
		char * items;

		items = cute_assess_uint_set_str(set->items, set->count);
		cute_text_asprintf(blk,
		                   "found:  [%" PRIuMAX "] %s set {%s}",
		                   assess->check.uint.value,
		                   inv,
		                   items);
		cute_free(items);
	}
	else
		cute_text_asprintf(blk,
		                   "found:  [%" PRIuMAX "] %s set {}",
		                   assess->check.uint.value,
		                   inv);

	return blk;
}

static void
cute_check_assess_uint_set(const char *                   file,
                           int                            line,
                           const char *                   function,
                           const struct cute_assess_ops * ops,
                           const struct cute_uint *       check,
                           const struct cute_uint_set *   expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	uintmax_t *                   items;
	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .uint = *check };

	if (expect->count) {
		unsigned int i;

		items = cute_malloc(expect->count * sizeof(items[0]));
		for (i = 0; i < expect->count; i++)
			items[i] = expect->items[i];
	}
	else
		items = NULL;

	assess->ops = ops;
	assess->expect.uint.set = *expect;
	assess->expect.uint.set.items = items;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer set check failed");
}

static struct cute_text_block *
cute_check_desc_uint_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_uint_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_uint_in_set_ops = {
	.cmp     = cute_assess_cmp_uint_in_set,
	.desc    = cute_check_desc_uint_in_set,
	.release = cute_assess_release_uint_set
};

void
cute_check_uint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_uint *     check,
                       const struct cute_uint_set * expect)
{
	cute_check_assess_uint_set(file,
	                           line,
	                           function,
	                           &cute_assess_uint_in_set_ops,
	                           check,
	                           expect);
}

static struct cute_text_block *
cute_check_desc_uint_not_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_uint_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_uint_not_in_set_ops = {
	.cmp     = cute_assess_cmp_uint_not_in_set,
	.desc    = cute_check_desc_uint_not_in_set,
	.release = cute_assess_release_uint_set
};

void
cute_check_uint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_uint *     check,
                           const struct cute_uint_set * expect)
{
	cute_check_assess_uint_set(file,
	                           line,
	                           function,
	                           &cute_assess_uint_not_in_set_ops,
	                           check,
	                           expect);
}

/******************************************************************************
 * Floating point numbers handling
 ******************************************************************************/

static struct cute_text_block *
cute_check_desc_flt(const struct cute_assess * assess,
                    const char *               oper,
                    const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.flt.expr);
	cute_assert_intern(assess->check.flt.expr[0]);
	cute_assert_intern(assess->expect.flt.scal.expr);
	cute_assert_intern(assess->expect.flt.scal.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s %s",
	                   assess->check.flt.expr,
	                   oper,
	                   assess->expect.flt.scal.expr);

	cute_text_asprintf(blk,
	                   "found:  [" CUTE_FLT_FORMAT_STR "] "
	                   "%s "
	                   "[" CUTE_FLT_FORMAT_STR "]",
	                   assess->check.flt.value,
	                   inv,
	                   assess->expect.flt.scal.value);

	return blk;
}

static void
cute_check_assess_flt(const char *                   file,
                      int                            line,
                      const char *                   function,
                      const struct cute_assess_ops * ops,
                      const struct cute_flt *        check,
                      const struct cute_flt *        expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .flt = *check };

	assess->ops = ops;
	assess->expect.flt.scal = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point value check failed");
}

static struct cute_text_block *
cute_check_desc_flt_equal(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, "==", "!=");
}

static const struct cute_assess_ops cute_assess_flt_equal_ops = {
	.cmp     = cute_assess_cmp_flt_equal,
	.desc    = cute_check_desc_flt_equal,
	.release = cute_assess_release_null
};

void
cute_check_flt_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                      line,
	                      function,
	                      &cute_assess_flt_equal_ops,
	                      check,
	                      expect);
}

static struct cute_text_block *
cute_check_desc_flt_unequal(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, "!=", "==");
}

static const struct cute_assess_ops cute_assess_flt_unequal_ops = {
	.cmp     = cute_assess_cmp_flt_unequal,
	.desc    = cute_check_desc_flt_unequal,
	.release = cute_assess_release_null
};

void
cute_check_flt_unequal(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                      line,
	                      function,
	                      &cute_assess_flt_unequal_ops,
	                      check,
	                      expect);
}

static struct cute_text_block *
cute_check_desc_flt_greater(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, ">", "<=");
}

static const struct cute_assess_ops cute_assess_flt_greater_ops = {
	.cmp     = cute_assess_cmp_flt_greater,
	.desc    = cute_check_desc_flt_greater,
	.release = cute_assess_release_null
};

void
cute_check_flt_greater(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                      line,
	                      function,
	                      &cute_assess_flt_greater_ops,
	                      check,
	                      expect);
}

static struct cute_text_block *
cute_check_desc_flt_greater_equal(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, ">=", "<");
}

static const struct cute_assess_ops cute_assess_flt_greater_equal_ops = {
	.cmp     = cute_assess_cmp_flt_greater_equal,
	.desc    = cute_check_desc_flt_greater_equal,
	.release = cute_assess_release_null
};

void
cute_check_flt_greater_equal(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * check,
                             const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                       line,
	                       function,
	                       &cute_assess_flt_greater_equal_ops,
	                       check,
	                       expect);
}

static struct cute_text_block *
cute_check_desc_flt_lower(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, "<", ">=");
}

static const struct cute_assess_ops cute_assess_flt_lower_ops = {
	.cmp     = cute_assess_cmp_flt_lower,
	.desc    = cute_check_desc_flt_lower,
	.release = cute_assess_release_null
};

void
cute_check_flt_lower(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                      line,
	                      function,
	                      &cute_assess_flt_lower_ops,
	                      check,
	                      expect);
}

static struct cute_text_block *
cute_check_desc_flt_lower_equal(const struct cute_assess * assess)
{
	return cute_check_desc_flt(assess, "<=", ">");
}

static const struct cute_assess_ops cute_assess_flt_lower_equal_ops = {
	.cmp     = cute_assess_cmp_flt_lower_equal,
	.desc    = cute_check_desc_flt_lower_equal,
	.release = cute_assess_release_null
};

void
cute_check_flt_lower_equal(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check,
                           const struct cute_flt * expect)
{
	cute_check_assess_flt(file,
	                      line,
	                      function,
	                      &cute_assess_flt_lower_equal_ops,
	                      check,
	                      expect);
}

static struct cute_text_block *
cute_check_desc_flt_range(const struct cute_assess * assess,
                          const char *               oper,
                          const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.flt.expr);
	cute_assert_intern(assess->check.flt.expr[0]);
	cute_assert_intern(assess->expect.flt.range.expr);
	cute_assert_intern(assess->expect.flt.range.expr[0]);
	cute_assert_intern(assess->expect.flt.range.min <=
	                   assess->expect.flt.range.max);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s range %s",
	                   assess->check.flt.expr,
	                   oper,
	                   assess->expect.flt.range.expr);

	cute_text_asprintf(blk,
	                   "found:  [" CUTE_FLT_FORMAT_STR "] "
	                   "%s "
	                   "range {" CUTE_FLT_FORMAT_STR
	                   " ... "
	                   CUTE_FLT_FORMAT_STR "}",
	                   assess->check.flt.value,
	                   inv,
	                   assess->expect.flt.range.min,
	                   assess->expect.flt.range.max);

	return blk;
}

static void
cute_check_assess_flt_range(const char *                   file,
                            int                            line,
                            const char *                   function,
                            const struct cute_assess_ops * ops,
                            const struct cute_flt *        check,
                            const struct cute_flt_range *  expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .flt = *check };

	assess->ops = ops;
	assess->expect.flt.range = *expect;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point number range check failed");
}

static struct cute_text_block *
cute_check_desc_flt_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_flt_range(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_flt_in_range_ops = {
	.cmp     = cute_assess_cmp_flt_in_range,
	.desc    = cute_check_desc_flt_in_range,
	.release = cute_assess_release_null
};

void
cute_check_flt_in_range(const char *                  file,
                        int                           line,
                        const char *                  function,
                        const struct cute_flt *       check,
                        const struct cute_flt_range * expect)
{
	cute_check_assess_flt_range(file,
	                            line,
	                            function,
	                            &cute_assess_flt_in_range_ops,
	                            check,
	                            expect);
}

static struct cute_text_block *
cute_check_desc_flt_not_in_range(const struct cute_assess * assess)
{
	return cute_check_desc_flt_range(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_flt_not_in_range_ops = {
	.cmp     = cute_assess_cmp_flt_not_in_range,
	.desc    = cute_check_desc_flt_not_in_range,
	.release = cute_assess_release_null
};

void
cute_check_flt_not_in_range(const char *                  file,
                            int                           line,
                            const char *                  function,
                            const struct cute_flt *       check,
                            const struct cute_flt_range * expect)
{
	cute_check_assess_flt_range(file,
	                            line,
	                            function,
	                            &cute_assess_flt_not_in_range_ops,
	                            check,
	                            expect);
}

static struct cute_text_block *
cute_check_desc_flt_set(const struct cute_assess * assess,
                        const char *               oper,
                        const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.flt.expr);
	cute_assert_intern(assess->check.flt.expr[0]);
	cute_assert_intern(assess->expect.flt.set.expr);
	cute_assert_intern(assess->expect.flt.set.expr[0]);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	struct cute_text_block *    blk;
	const struct cute_flt_set * set = &assess->expect.flt.set;

	blk = cute_assess_desc_source(assess,
	                              3 +
	                              (unsigned int)!!assess->func);

	cute_text_asprintf(blk,
	                   "wanted: %s %s set %s",
	                   assess->check.flt.expr,
	                   oper,
	                   set->expr);

	if (set->count) {
		char * items;

		items = cute_assess_flt_set_str(set->items, set->count);
		cute_text_asprintf(blk,
		                   "found:  [" CUTE_FLT_FORMAT_STR "] %s "
		                   "set {%s}",
		                   assess->check.flt.value,
		                   inv,
		                   items);
		cute_free(items);
	}
	else
		cute_text_asprintf(blk,
		                   "found:  [" CUTE_FLT_FORMAT_STR "] %s "
		                   "set {}",
		                   assess->check.flt.value,
		                   inv);

	return blk;
}

static void
cute_check_assess_flt_set(const char *                   file,
                          int                            line,
                          const char *                   function,
                          const struct cute_assess_ops * ops,
                          const struct cute_flt *        check,
                          const struct cute_flt_set *    expect)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assess_assert_ops(ops);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	long double *                 items;
	struct cute_assess *          assess = &cute_curr_run->assess;
	const union cute_assess_value chk = { .flt = *check };

	if (expect->count) {
		unsigned int i;

		items = cute_malloc(expect->count * sizeof(items[0]));
		for (i = 0; i < expect->count; i++)
			items[i] = expect->items[i];
	}
	else
		items = NULL;

	assess->ops = ops;
	assess->expect.flt.set = *expect;
	assess->expect.flt.set.items = items;

	if (!cute_assess_check(assess, &chk))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point number set check failed");
}

static struct cute_text_block *
cute_check_desc_flt_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_flt_set(assess, "in", "not in");
}

static const struct cute_assess_ops cute_assess_flt_in_set_ops = {
	.cmp     = cute_assess_cmp_flt_in_set,
	.desc    = cute_check_desc_flt_in_set,
	.release = cute_assess_release_flt_set
};

void
cute_check_flt_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_flt *     check,
                      const struct cute_flt_set * expect)
{
	cute_check_assess_flt_set(file,
	                           line,
	                           function,
	                           &cute_assess_flt_in_set_ops,
	                           check,
	                           expect);
}

static struct cute_text_block *
cute_check_desc_flt_not_in_set(const struct cute_assess * assess)
{
	return cute_check_desc_flt_set(assess, "not in", "in");
}

static const struct cute_assess_ops cute_assess_flt_not_in_set_ops = {
	.cmp     = cute_assess_cmp_flt_not_in_set,
	.desc    = cute_check_desc_flt_not_in_set,
	.release = cute_assess_release_flt_set
};

void
cute_check_flt_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_flt *     check,
                          const struct cute_flt_set * expect)
{
	cute_check_assess_flt_set(file,
	                           line,
	                           function,
	                           &cute_assess_flt_not_in_set_ops,
	                           check,
	                           expect);
}