/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "assess.h"
#include "run.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>

/*
 * Number of digits required to show an integral number encoded over _bits bits
 * in base 8.
 * This is _bits / log2(8), i.e., _bits / 3 rounded to the upper integer.
 */
#define BASE8_DIGITS(_bits)  ((_bits + 2U) / 3U)
/*
 * Number of digits required to show an integral number encoded over _bits bits
 * in base 10.
 * This is _bits / log2(10), i.e., _bits / 3.32 rounded to the upper integer.
 */
#define BASE10_DIGITS(_bits) (((_bits * 100U) + 331U) / 332U)
/*
 * Number of digits required to show an integral number encoded over _bits bits
 * in base 16.
 * This is _bits / log2(16), i.e., _bits / 4 rounded to the upper integer.
 */
#define BASE16_DIGITS(_bits) ((_bits + 3U) / 4U)

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

#define INTMAX_BITS          ((unsigned int)(sizeof(intmax_t) * CHAR_BIT) - 1U)
#define INTMAX_BASE10_DIGITS BASE10_DIGITS(INTMAX_BITS)

char *
cute_assess_sint_set_str(const intmax_t * items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	/* Compute space needed to show something like: "-2, 0, 4, -100"... */
	unsigned int len = (1 +                    /* sign char */
	                    INTMAX_BASE10_DIGITS)  /* number of digits */
	                   +
	                   ((1 +                   /* comma char */
	                     1 +                   /* space char */
	                     1 +                   /* sign char */
	                     INTMAX_BASE10_DIGITS) /* number of digits */
	                    *                      /* number of integer */
	                    (count - 1));          /* prefixed with ", " */
	char *       str;
	unsigned int i;
	int          sz;

	str = cute_malloc(len + 1);

	sz = sprintf(str, "%" PRIdMAX, items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", %" PRIdMAX, items[i]);
	}

	return str;
}

void
cute_assess_release_sint_set(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->expect.sint.set.expr);
	cute_assert_intern(assess->expect.sint.set.expr[0]);
	cute_assert_intern(!assess->expect.sint.set.count ||
	                    assess->expect.sint.set.items);

	if (assess->expect.sint.set.count) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_free((void *)assess->expect.sint.set.items);
#pragma GCC diagnostic pop
	}
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

#define UINTMAX_BITS          ((unsigned int)sizeof(uintmax_t) * CHAR_BIT)
#define UINTMAX_BASE10_DIGITS BASE10_DIGITS(UINTMAX_BITS)

char *
cute_assess_uint_set_str(const uintmax_t * items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	/* Compute space needed to show something like: "-2, 0, 4, -100"... */
	unsigned int len = UINTMAX_BASE10_DIGITS    /* number of digits */
	                   +
	                   ((1 +                    /* comma char */
	                     1 +                    /* space char */
	                     UINTMAX_BASE10_DIGITS) /* number of digits */
	                    *                       /* number of integer */
	                    (count - 1));           /* prefixed with ", " */
	char *       str;
	unsigned int i;
	int          sz;

	str = cute_malloc(len + 1);

	sz = sprintf(str, "%" PRIuMAX, items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", %" PRIuMAX, items[i]);
	}

	return str;
}

void
cute_assess_release_uint_set(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->expect.uint.set.expr);
	cute_assert_intern(assess->expect.uint.set.expr[0]);
	cute_assert_intern(!assess->expect.uint.set.count ||
	                    assess->expect.uint.set.items);

	if (assess->expect.uint.set.count) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_free((void *)assess->expect.uint.set.items);
#pragma GCC diagnostic pop
	}
}

/******************************************************************************
 * Unsigned integer hexadecimal numbers handling
 ******************************************************************************/

#define UINTMAX_BASE16_DIGITS \
	((unsigned int)sizeof("0x") - 1 + BASE16_DIGITS(UINTMAX_BITS))

char *
cute_assess_hex_set_str(const uintmax_t * items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	/* Compute space needed to show something like: "-2, 0, 4, -100"... */
	unsigned int len = UINTMAX_BASE16_DIGITS    /* number of digits */
	                   +
	                   ((1 +                    /* comma char */
	                     1 +                    /* space char */
	                     UINTMAX_BASE16_DIGITS) /* number of digits */
	                    *                       /* number of integer */
	                    (count - 1));           /* prefixed with ", " */
	char *       str;
	unsigned int i;
	int          sz;

	str = cute_malloc(len + 1);

	sz = sprintf(str, "0x%" PRIxMAX, items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", 0x%" PRIxMAX, items[i]);
	}

	return str;
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

/*
 * Maximum number of decimal digits used to express a floating point number's
 * exponent as text.
 * Basically, the maximum exponent value is LDBL_MAX since we convert all
 * floating point numbers to long double. On x86_64, this is something like
 * 16384.
 * To derive the number of decimal digits we first need to know how many bits
 * are required to encode LDBL_MAX. This is something quite difficult to compute
 * at compile time. For sake of simplicity, simply use the number of bits
 * required to encode an unsigned short integer.
 */
#define CUTE_FLT_EXP_DIGITS  BASE10_DIGITS(USHRT_MAX)

/*
 * Maximum number of characters used to express a floating point number as
 * text according to the following format:
 *     <mant_sign><mant_integ>.<mant_frac>e<exp_sign><exp_val>
 * Where:
 *     mant_sign:  sign of mantissa
 *     mant_integ: integral part of mantissa
 *     mant_frac:  fractional part of mantissa
 *     exp_sign:   sign of exponent
 *     exp_val:    exponent value.
 * Examples:
 *     2.003
 *     2.1021352
 *     1.107e-05
 *     3.362103143e-4932
 *
 * Remind that CUTE_FLT_MANT_DIGITS denotes the number of digits required to
 * express both the integral and fractional parts of the mantissa, fractional
 * dot '.' excluded.
 */
#define CUTE_FLT_CHAR_NR \
	(1 + 1 + CUTE_FLT_MANT_DIGITS + 1 + 1 + CUTE_FLT_EXP_DIGITS)

char *
cute_assess_flt_set_str(const long double * items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	/* Compute space needed to show something like: "1.003, -7.02e+05"... */
	unsigned int len = CUTE_FLT_CHAR_NR +
	                   ((1 +               /* comma char */
	                     1 +               /* space char */
	                     CUTE_FLT_CHAR_NR)
	                    *                  /* number of floats prefixed */
	                    (count - 1));      /* with ", " */
	char *       str;
	unsigned int i;
	int          sz;

	str = cute_malloc(len + 1);

	sz = sprintf(str, CUTE_FLT_FORMAT_STR, items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", " CUTE_FLT_FORMAT_STR, items[i]);
	}

	return str;
}

void
cute_assess_release_flt_set(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->expect.flt.set.expr);
	cute_assert_intern(assess->expect.flt.set.expr[0]);
	cute_assert_intern(!assess->expect.flt.set.count ||
	                    assess->expect.flt.set.items);

	if (assess->expect.flt.set.count) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_free((void *)assess->expect.flt.set.items);
#pragma GCC diagnostic pop
	}
}

/******************************************************************************
 * Strings checking
 ******************************************************************************/

bool
cute_assess_cmp_str_equal(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	if (!check->str.value)
		return false;

	return !strcmp(check->str.value, assess->expect.str.sole.value);
}

bool
cute_assess_cmp_str_unequal(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return !cute_assess_cmp_str_equal(assess, check);
}

bool
cute_assess_cmp_str_begin(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	const char * ref = assess->expect.str.sole.value;
	size_t       len;

	if (!check->str.value)
		return false;

	len = strlen(ref);

	return !memcmp(check->str.value, ref, len);
}

bool
cute_assess_cmp_str_not_begin(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return !cute_assess_cmp_str_begin(assess, check);
}

bool
cute_assess_cmp_str_end(const struct cute_assess *      assess,
                        const union cute_assess_value * check)
{
	const char * chk = check->str.value;
	const char * ref = assess->expect.str.sole.value;
	size_t       clen;
	size_t       rlen;

	if (!check->str.value)
		return false;

	clen = strlen(chk);
	rlen = strlen(ref);

	if (rlen > clen)
		return false;

	return !memcmp(&chk[clen - rlen], ref, rlen + 1);
}

bool
cute_assess_cmp_str_not_end(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return !cute_assess_cmp_str_end(assess, check);
}

bool
cute_assess_cmp_str_contain(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	if (!check->str.value)
		return false;

	return !!strstr(check->str.value, assess->expect.str.sole.value);
}

bool
cute_assess_cmp_str_not_contain(const struct cute_assess *      assess,
                                const union cute_assess_value * check)
{
	return !cute_assess_cmp_str_contain(assess, check);
}

bool
cute_assess_cmp_str_in_set(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	unsigned int cnt;

	if (!check->str.value)
		return false;

	for (cnt = 0; cnt < assess->expect.str.set.count; cnt++) {
		if (!strcmp(check->str.value,
		            assess->expect.str.set.items[cnt]))
			return true;
	}

	return false;
}

bool
cute_assess_cmp_str_not_in_set(const struct cute_assess *      assess,
                               const union cute_assess_value * value)
{
	return !cute_assess_cmp_str_in_set(assess, value);
}

char *
cute_assess_str_set_str(const char ** items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	size_t       len;
	char *       str;
	unsigned int i;
	int          sz;

	/* Compute space needed to show something like: "str0", "str1"... */
	cute_assert_intern(items[0]);
	len = 1 +                /* double quotes */
	      strlen(items[0]) + /* item length */
	      1;                 /* double quotes */
	/* For each item in set... */
	for (i = 1; i < count; i++) {
		cute_assert_intern(items[i]);
		len += 2 +                /* comma + space char */
		       1 +                /* double quotes */
		       strlen(items[i]) + /* item length */
		       1;                 /* double quotes */
	}

	str = cute_malloc(len + 1);

	sz = sprintf(str, "\"%s\"", items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", \"%s\"", items[i]);
	}

	return str;
}

void
cute_assess_release_str_set(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->expect.str.set.expr);
	cute_assert_intern(assess->expect.str.set.expr[0]);
	cute_assert_intern(!assess->expect.str.set.count ||
	                    assess->expect.str.set.items);

	if (assess->expect.str.set.count) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_free((void *)assess->expect.str.set.items);
#pragma GCC diagnostic pop
	}
}

/******************************************************************************
 * Unsigned integer nubmers handling
 ******************************************************************************/

bool
cute_assess_cmp_ptr_equal(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	return check->ptr.value == assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_unequal(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return check->ptr.value != assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_greater(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return check->ptr.value > assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_greater_equal(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return check->ptr.value >= assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_lower(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	return check->ptr.value < assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_lower_equal(const struct cute_assess *      assess,
                                const union cute_assess_value * check)
{
	return check->ptr.value <= assess->expect.ptr.scal.value;
}

bool
cute_assess_cmp_ptr_in_range(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return (check->ptr.value >= assess->expect.ptr.range.min) &&
	       (check->ptr.value <= assess->expect.ptr.range.max);
}

bool
cute_assess_cmp_ptr_not_in_range(const struct cute_assess *      assess,
                                 const union cute_assess_value * check)
{
	return !cute_assess_cmp_ptr_in_range(assess, check);
}

bool
cute_assess_cmp_ptr_in_set(const struct cute_assess *      assess,
                           const union cute_assess_value * check)
{
	unsigned int cnt;

	for (cnt = 0; cnt < assess->expect.ptr.set.count; cnt++) {
		if (check->ptr.value == assess->expect.ptr.set.items[cnt])
			return true;
	}

	return false;
}

bool
cute_assess_cmp_ptr_not_in_set(const struct cute_assess *      assess,
                               const union cute_assess_value * value)
{
	return !cute_assess_cmp_ptr_in_set(assess, value);
}

#define PTRMAX_DIGITS \
	((unsigned int)sizeof("0x") - 1 + \
	 ((unsigned int)sizeof(void *) * 2))

char *
cute_assess_ptr_set_str(const void ** items, unsigned int count)
{
	cute_assert_intern(items);
	cute_assert_intern(count);

	/* Compute space needed to show something like: "-2, 0, 4, -100"... */
	unsigned int len = PTRMAX_DIGITS    /* number of digits */
	                   +
	                   ((1 +            /* comma char */
	                     1 +            /* space char */
	                     PTRMAX_DIGITS) /* number of digits */
	                    *               /* number of integer */
	                    (count - 1));   /* prefixed with ", " */
	char *       str;
	unsigned int i;
	int          sz;

	str = cute_malloc(len + 1);

	sz = sprintf(str, "%p", items[0]);
	for (i = 1; i < count; i++) {
		cute_assert_intern((unsigned int)sz < (len + 1));
		sz += sprintf(&str[sz], ", %p", items[i]);
	}

	return str;
}

void
cute_assess_release_ptr_set(struct cute_assess * assess)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->expect.ptr.set.expr);
	cute_assert_intern(assess->expect.ptr.set.expr[0]);
	cute_assert_intern(!assess->expect.ptr.set.count ||
	                    assess->expect.ptr.set.items);

	if (assess->expect.ptr.set.count) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
		cute_free((void *)assess->expect.ptr.set.items);
#pragma GCC diagnostic pop
	}
}

/******************************************************************************
 * Memory area checking
 ******************************************************************************/

bool
cute_assess_cmp_mem_equal(const struct cute_assess *      assess,
                          const union cute_assess_value * check)
{
	if (!check->ptr.value)
		return false;

	return !memcmp(check->ptr.value,
	               assess->expect.mem.area.ptr,
	               assess->expect.mem.area.size);
}

bool
cute_assess_cmp_mem_unequal(const struct cute_assess *      assess,
                            const union cute_assess_value * check)
{
	return !cute_assess_cmp_mem_equal(assess, check);
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

	assess->file = file;
	assess->line = line;
	assess->func = function;
}

void
cute_assess_check(struct cute_assess *            result,
                  struct cute_assess *            assess,
                  const union cute_assess_value * check,
                  const char *                    why)
{
	cute_assess_assert_intern(result);
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->file);
	cute_assert_intern(assess->file[0]);
	cute_assert_intern(assess->line >= 0);
	cute_assert_intern(assess->func);
	cute_assert_intern(assess->func[0]);
	cute_assert_intern(check);
	cute_assert_intern(check->expr);
	cute_assert_intern(why);
	cute_assert_intern(why[0]);

	if (!assess->ops->cmp(assess, check)) {
		sigset_t sigs;

		cute_run_block_sigs(&sigs);

		cute_assess_release(result);
		*result = *assess;
		result->check = *check;
		cute_break(CUTE_FAIL_ISSUE,
		           assess->file,
		           assess->line,
		           assess->func,
		           why);

		cute_run_unblock_sigs(&sigs);

		siglongjmp(cute_jmp_env, CUTE_FAIL_ISSUE);
	}

	/*
	 * Release allocated resources when check passed since report
	 * does not access results when test is successful.
	 * Clear operation structure so that cute_assess_release() may
	 * safely skip release operation at cute_run_fini() time.
	 */
	cute_assess_release(assess);
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

		assess->ops->release(assess);
	}
}
