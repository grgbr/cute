#include "assess.h"
#include "run.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>

static struct cute_text_block *
cute_assess_desc_null(const struct cute_assess * assess __cute_unused)
{
	return NULL;
}

const struct cute_assess_ops cute_assess_null_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_null,
	.release = cute_assess_release_null
};

void
cute_assess_build_null(struct cute_assess * assess)
{
	cute_assert_intern(assess);

	assess->ops = &cute_assess_null_ops;
	assess->file = NULL;
	assess->line = -1;
	assess->func = NULL;
}

struct cute_text_block *
cute_assess_desc_source(const struct cute_assess * assess, unsigned int nr)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->file);
	cute_assert_intern(assess->file[0]);
	cute_assert_intern(assess->line >= 0);
	cute_assert_intern(!assess->func || assess->func[0]);
	cute_assert_intern(nr >= 1);

	struct cute_text_block * blk;

	blk = cute_text_create(nr);

	cute_text_asprintf(blk,         "source: %s:%d", assess->file,
	                                                 assess->line);

	if (assess->func)
		cute_text_asprintf(blk, "caller: %s()", assess->func);

	return blk;
}

static struct cute_text_block *
cute_assess_desc_expr(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	unsigned int             nr = (unsigned int)!!assess->check.expr +
	                              (unsigned int)!!assess->func;
	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess, 1 + nr);

	if (assess->check.expr)
		cute_text_asprintf(blk, "detail: %s", assess->check.expr);

	return blk;
}

static const struct cute_assess_ops cute_assess_expr_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_expr,
	.release = cute_assess_release_null
};

void
cute_assess_build_expr(struct cute_assess * assess, const char * expr)
{
	cute_assert_intern(assess);
	cute_assert_intern(!expr || expr[0]);

	assess->ops = &cute_assess_expr_ops;
	assess->check.expr = expr;
}

static struct cute_text_block *
cute_assess_desc_assert(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	unsigned int             nr = (unsigned int)!!assess->expect.expr +
	                              (unsigned int)!!assess->func;
	struct cute_text_block * blk;

	blk = cute_assess_desc_source(assess, 1 + nr);

	if (assess->expect.expr)
		cute_text_asprintf(blk, "assert: %s", assess->expect.expr);

	return blk;
}

static const struct cute_assess_ops cute_assess_assert_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_assert,
	.release = cute_assess_release_null
};

void
cute_assess_build_assert(struct cute_assess * assess, const char * expr)
{
	cute_assert_intern(assess);
	cute_assert_intern(!expr || expr[0]);

	assess->ops = &cute_assess_assert_ops;
	assess->expect.expr = expr;
}

static struct cute_text_block *
cute_assess_desc_excp(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->file);
	cute_assert_intern(assess->file[0]);
	cute_assert_intern(assess->line >= 0);
	cute_assert_intern(!assess->func || assess->func[0]);
	cute_assert_intern((int)assess->check.sint.value);
	cute_assert_intern((int)assess->check.sint.value);
	cute_assert_intern((int)assess->check.sint.value != SIGKILL);
	cute_assert_intern((int)assess->check.sint.value != SIGSTOP);
	cute_assert_intern((int)assess->check.sint.value <= NSIG ||
	                   ((int)assess->check.sint.value >= SIGRTMIN &&
	                    (int)assess->check.sint.value <= SIGRTMAX));

	struct cute_text_block * blk;
	int                      sig = (int)assess->check.sint.value;

	blk = cute_text_create(2);

	cute_text_asprintf(blk, "source: %s:%d", assess->file, assess->line);
	cute_text_asprintf(blk, "except: %s (%d)", strsignal(sig), sig);

	return blk;
}

static const struct cute_assess_ops cute_assess_excp_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_excp,
	.release = cute_assess_release_null
};

void
cute_assess_build_excp(struct cute_assess * assess, int sig)
{
	cute_assert_intern(assess);
	cute_assert_intern(sig);
	cute_assert_intern(sig != SIGKILL);
	cute_assert_intern(sig != SIGSTOP);
	cute_assert_intern(sig <= NSIG || (sig >= SIGRTMIN && sig <= SIGRTMAX));

	assess->ops = &cute_assess_excp_ops;
	assess->check.sint.value = sig;
}

/******************************************************************************
 * Signed integer numbers handling
 ******************************************************************************/

bool
cute_assess_cmp_sint_equal(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	return check->sint.value == assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_unequal(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return check->sint.value != assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_greater(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return check->sint.value > assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_greater_equal(const struct cute_assess *      assess,
                                   const union cute_assess_value * check)
{
	return check->sint.value >= assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_lower(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	return check->sint.value < assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_lower_equal(const struct cute_assess *      assess,
                                 const union cute_assess_value * check)
{
	return check->sint.value <= assess->expect.sint.scal.value;
}

bool
cute_assess_cmp_sint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->sint.value >= assess->expect.sint.range.min) &&
	       (check->sint.value <= assess->expect.sint.range.max);
}

bool
cute_assess_cmp_sint_not_in_range(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return !cute_assess_cmp_sint_in_range(assess, check);
}

bool
cute_assess_cmp_sint_in_set(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	unsigned int cnt;

	for (cnt = 0; cnt < assess->expect.sint.set.count; cnt++) {
		if (check->sint.value == assess->expect.sint.set.items[cnt])
			return true;
	}

	return false;
}

bool
cute_assess_cmp_sint_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_sint_in_set(assess, value);
}

/******************************************************************************
 * Unsigned integer nubmers handling
 ******************************************************************************/

bool
cute_assess_cmp_uint_equal(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	return check->uint.value == assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_unequal(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return check->uint.value != assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_greater(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return check->uint.value > assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_greater_equal(const struct cute_assess *      assess,
                                   const union cute_assess_value * check)
{
	return check->uint.value >= assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_lower(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	return check->uint.value < assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_lower_equal(const struct cute_assess *      assess,
                                 const union cute_assess_value * check)
{
	return check->uint.value <= assess->expect.uint.scal.value;
}

bool
cute_assess_cmp_uint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->uint.value >= assess->expect.uint.range.min) &&
	       (check->uint.value <= assess->expect.uint.range.max);
}

bool
cute_assess_cmp_uint_not_in_range(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return !cute_assess_cmp_uint_in_range(assess, check);
}

bool
cute_assess_cmp_uint_in_set(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	unsigned int cnt;

	for (cnt = 0; cnt < assess->expect.uint.set.count; cnt++) {
		if (check->uint.value == assess->expect.uint.set.items[cnt])
			return true;
	}

	return false;
}

bool
cute_assess_cmp_uint_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_uint_in_set(assess, value);
}

/******************************************************************************
 * Floating point numbers handling
 ******************************************************************************/

bool
cute_assess_cmp_flt_equal(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	return check->flt.value == assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_unequal(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return check->flt.value != assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_greater(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return check->flt.value > assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_greater_equal(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return check->flt.value >= assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_lower(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	return check->flt.value < assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_lower_equal(const struct cute_assess *      assess,
                                const union cute_assess_value * check)
{
	return check->flt.value <= assess->expect.flt.scal.value;
}

bool
cute_assess_cmp_flt_in_range(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return (check->flt.value >= assess->expect.flt.range.min) &&
	       (check->flt.value <= assess->expect.flt.range.max);
}

bool
cute_assess_cmp_flt_not_in_range(const struct cute_assess *      assess,
                                 const union cute_assess_value * check)
{
	return !cute_assess_cmp_flt_in_range(assess, check);
}

bool
cute_assess_cmp_flt_in_set(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	unsigned int cnt;

	for (cnt = 0; cnt < assess->expect.flt.set.count; cnt++) {
		if (check->flt.value == assess->expect.flt.set.items[cnt])
			return true;
	}

	return false;
}

bool
cute_assess_cmp_flt_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_flt_in_set(assess, value);
}

/******************************************************************************
 * Top-level generic assess handling
 ******************************************************************************/

void
cute_assess_update_source(struct cute_assess * assess,
                          const char *         file,
                          int                  line,
                          const char *         function)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(!function || function[0]);
	cute_assert_intern(!assess->file);
	cute_assert_intern(assess->line < 0);
	cute_assert_intern(!assess->func);

	assess->file = file;
	assess->line = line;
	assess->func = function;
}

bool
cute_assess_check(struct cute_assess *            assess,
                  const union cute_assess_value * check)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(check);
	cute_assert_intern(check->expr);

	if (assess->ops->cmp(assess, check)) {
		/*
		 * Release allocated resources when check passed since report
		 * does not access results when test is successful.
		 * Clear operation structure so that cute_assess_release() may
		 * safely skip release operation at cute_run_fini() time.
		 */
		cute_assess_release(assess);
		assess->ops = NULL;

		return true;
	}

	assess->check = *check;

	return false;
}

struct cute_text_block *
cute_assess_desc(const struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);

	return assess->ops->desc(assess);
}

void
cute_assess_release(struct cute_assess * assess)
{
	/*
	 * Skip release operation if resources have already been released thanks
	 * to cute_assess_check().
	 */
	if (assess->ops) {
		cute_assess_assert_intern(assess);

		return assess->ops->release(assess);
	}
}
