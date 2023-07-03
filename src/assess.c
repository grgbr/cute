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

#define cute_assess_assert_desc(_desc) \
	cute_assert((_desc) >= 0); \
	cute_assert((_desc) < CUTE_ASSESS_DESC_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_desc_intern(_desc) \
	cute_assess_assert_desc(_desc)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_desc_intern(_desc)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_ops(_ops) \
	cute_assert(_ops); \
	cute_assert((_ops)->cmp); \
	cute_assert((_ops)->desc); \
	cute_assert((_ops)->release)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_ops_intern(_assess) \
	cute_assess_assert_ops(_assess)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_ops_intern(_assess)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert(_assess) \
	cute_assert(_assess); \
	cute_assess_assert_ops((_assess)->ops)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_intern(_assess) \
	cute_assess_assert(_assess)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_intern(_assess)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static bool
cute_assess_cmp_null(const struct cute_assess *      assess __cute_unused,
                     const union cute_assess_value * check __cute_unused)
{
	return true;
}

static char *
cute_assess_desc_null(const struct cute_assess * assess __cute_unused,
                      enum cute_assess_desc      desc __cute_unused)
{
	cute_assess_assert_desc_intern(desc);

	return NULL;
}

static void
cute_assess_release_null(struct cute_assess * assess __cute_unused)
{
}

static const struct cute_assess_ops cute_assess_null_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_null,
	.release = cute_assess_release_null
};

void
cute_assess_build_null(struct cute_assess * assess)
{
	cute_assert_intern(assess);

	assess->ops = &cute_assess_null_ops;
}

static char *
cute_assess_desc_expr(const struct cute_assess * assess,
                      enum cute_assess_desc      desc)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern(assess->check.expr || assess->expect.expr);

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		return assess->expect.expr ? cute_dup(assess->expect.expr)
		                           : NULL;

	case CUTE_ASSESS_FOUND_DESC:
		return assess->check.expr ? cute_dup(assess->check.expr)
		                          : NULL;

	default:
		__cute_unreachable();
	}
}

static const struct cute_assess_ops cute_assess_expr_ops = {
	.cmp     = cute_assess_cmp_null,
	.desc    = cute_assess_desc_expr,
	.release = cute_assess_release_null
};

void
cute_assess_build_expr(struct cute_assess * assess,
                       const char *         check,
                       const char *         expect)
{
	cute_assert_intern(assess);
	cute_assert_intern(!check || check[0]);
	cute_assert_intern(!expect || expect[0]);

	assess->ops = &cute_assess_expr_ops;
	assess->check.expr = check;
	assess->expect.expr = expect;
}

static char *
cute_assess_desc_excp(const struct cute_assess * assess,
                      enum cute_assess_desc      desc)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern((int)assess->check.sint.value);
	cute_assert_intern((int)assess->check.sint.value != SIGKILL);
	cute_assert_intern((int)assess->check.sint.value != SIGSTOP);
	cute_assert_intern((int)assess->check.sint.value <= NSIG ||
	                   ((int)assess->check.sint.value >= SIGRTMIN &&
	                    (int)assess->check.sint.value <= SIGRTMAX));

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		return NULL;

	case CUTE_ASSESS_FOUND_DESC:
		return cute_asprintf("%s (%d)",
		                     strsignal((int)assess->check.sint.value),
		                     (int)assess->check.sint.value);

	default:
		__cute_unreachable();
	}
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
 * Common scalar value handling
 ******************************************************************************/

#define CUTE_ASSESS_DESC_TYPED_VALUE(_name, _type, _format) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc, \
	      const char *               oper, \
	      const char *               inv) \
	{ \
		cute_assess_assert_intern(assess); \
		cute_assert_intern(assess->check._type.expr); \
		cute_assert_intern(assess->check._type.expr[0]); \
		cute_assert_intern(assess->expect._type.scal.expr); \
		cute_assert_intern(assess->expect._type.scal.expr[0]); \
		cute_assess_assert_desc_intern(desc); \
		cute_assert_intern(oper); \
		cute_assert_intern(oper[0]); \
		cute_assert_intern(inv); \
		cute_assert_intern(inv[0]); \
		\
		switch (desc) { \
		case CUTE_ASSESS_EXPECT_DESC: \
			return cute_asprintf( \
				"%s %s %s", \
				assess->check._type.expr, \
				oper, \
				assess->expect._type.scal.expr); \
		\
		case CUTE_ASSESS_FOUND_DESC: \
			return cute_asprintf( \
				"[%" _format "] %s [%" _format "]", \
				assess->check._type.value, \
				inv, \
				assess->expect._type.scal.value); \
		\
		default: \
			__cute_unreachable(); \
		} \
	}

#define CUTE_ASSESS_BUILD_TYPED_VALUE(_name, _type) \
	static void \
	_name(struct cute_assess *           assess, \
	      const struct cute_assess_ops * ops, \
	      const struct cute_ ## _type *  expect) \
	{ \
		cute_assert_intern(assess); \
		cute_assess_assert_ops_intern(ops); \
		cute_assert_intern(expect->expr); \
		cute_assert_intern(expect->expr[0]); \
		\
		assess->ops = ops; \
		assess->expect._type.scal = *expect; \
	}

#define CUTE_ASSESS_TYPED_VALUE(_name, _type) \
	static bool \
	_name(struct cute_assess *           assess, \
	      const struct cute_assess_ops * ops, \
	      const struct cute_ ## _type *  check, \
	      const struct cute_ ## _type *  expect) \
	{ \
		cute_assert_intern(assess); \
		cute_assess_assert_ops_intern(ops); \
		cute_assert_intern(check->expr); \
		cute_assert_intern(check->expr[0]); \
		cute_assert_intern(expect->expr); \
		cute_assert_intern(expect->expr[0]); \
		\
		const union cute_assess_value chk = { ._type = *check }; \
		\
		cute_assess_build_ ## _type ## _value(assess, ops, expect); \
		\
		return cute_assess_check(assess, &chk); \
	}

#define CUTE_ASSESS_DEFINE_TYPED_VALUE(_type, _assess, _build, _desc, _format) \
	CUTE_ASSESS_DESC_TYPED_VALUE(_desc, _type, _format) \
	CUTE_ASSESS_BUILD_TYPED_VALUE(_build, _type) \
	CUTE_ASSESS_TYPED_VALUE(_assess, _type)

#define CUTE_ASSESS_CMP_VALUE(_name, _type, _op) \
	static bool \
	_name(const struct cute_assess *      assess, \
	      const union cute_assess_value * check) \
	{ \
		return check->_type.value _op assess->expect._type.scal.value; \
	}

#define CUTE_ASSESS_DESC_VALUE(_name, _type, _op, _inv) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc) \
	{ \
		return cute_assess_desc_ ## _type ## _value(assess, \
		                                            desc, \
		                                            # _op, \
		                                            # _inv); \
	}

#define CUTE_ASSESS_OPS(_name, _cmp, _desc, _release) \
	static const struct cute_assess_ops _name = { \
		.cmp     = _cmp, \
		.desc    = _desc, \
		.release = _release \
	}

#define CUTE_ASSESS_VALUE(_name, _type, _ops) \
	bool \
	_name(struct cute_assess *          assess, \
	      const struct cute_ ## _type * check, \
	      const struct cute_ ## _type * expect) \
	{ \
		return cute_assess_ ## _type ## _value(assess, \
		                                       _ops, \
		                                       check, \
		                                       expect); \
	}

#define CUTE_ASSESS_DEFINE_VALUE(_type, _assess, _ops, _cmp, _desc, _op, _inv) \
	CUTE_ASSESS_CMP_VALUE(_cmp, _type, _op) \
	CUTE_ASSESS_DESC_VALUE(_desc, _type, _op, _inv) \
	CUTE_ASSESS_OPS(_ops, _cmp, _desc, cute_assess_release_null); \
	CUTE_ASSESS_VALUE(_assess, _type, &(_ops))

/******************************************************************************
 * Signed integer value handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_VALUE(sint,
                               cute_assess_sint_value,
                               cute_assess_build_sint_value,
                               cute_assess_desc_sint_value,
                               PRIdMAX)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_equal,
                         cute_assess_sint_equal_ops,
                         cute_assess_cmp_sint_equal,
                         cute_assess_desc_sint_equal,
                         ==,
                         !=)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_unequal,
                         cute_assess_sint_unequal_ops,
                         cute_assess_cmp_sint_unequal,
                         cute_assess_desc_sint_unequal,
                         !=,
                         ==)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_greater,
                         cute_assess_sint_greater_ops,
                         cute_assess_cmp_sint_greater,
                         cute_assess_desc_sint_greater,
                         >,
                         <=)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_greater_equal,
                         cute_assess_sint_greater_equal_ops,
                         cute_assess_cmp_sint_greater_equal,
                         cute_assess_desc_sint_greater_equal,
                         >=,
                         <)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_lower,
                         cute_assess_sint_lower_ops,
                         cute_assess_cmp_sint_lower,
                         cute_assess_desc_sint_lower,
                         <,
                         >=)

CUTE_ASSESS_DEFINE_VALUE(sint,
                         cute_assess_sint_lower_equal,
                         cute_assess_sint_lower_equal_ops,
                         cute_assess_cmp_sint_lower_equal,
                         cute_assess_desc_sint_lower_equal,
                         <=,
                         >)

/******************************************************************************
 * Common range handling
 ******************************************************************************/

#define CUTE_ASSESS_DESC_TYPED_RANGE(_name, _type, _format) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc, \
	      const char *               oper, \
	      const char *               inv) \
	{ \
		cute_assess_assert_intern(assess); \
		cute_assert_intern(assess->check._type.expr); \
		cute_assert_intern(assess->check._type.expr[0]); \
		cute_assert_intern(assess->expect._type.range.expr); \
		cute_assert_intern(assess->expect._type.range.expr[0]); \
		cute_assert_intern(assess->expect._type.range.min < \
		                   assess->expect._type.range.max); \
		cute_assess_assert_desc_intern(desc); \
		cute_assert_intern(oper); \
		cute_assert_intern(oper[0]); \
		cute_assert_intern(inv); \
		cute_assert_intern(inv[0]); \
		\
		switch (desc) { \
		case CUTE_ASSESS_EXPECT_DESC: \
			return cute_asprintf("%s %s range %s", \
			                     assess->check._type.expr, \
			                     oper, \
			                     assess->expect._type.range.expr); \
		\
		case CUTE_ASSESS_FOUND_DESC: \
			return cute_asprintf("[%" _format "] %s " \
			                     "range {%" _format \
			                     " ... %" _format "}", \
			                     assess->check._type.value, \
			                     inv, \
			                     assess->expect._type.range.min, \
			                     assess->expect._type.range.max); \
		\
		default: \
			__cute_unreachable(); \
		} \
	}

#define CUTE_ASSESS_BUILD_TYPED_RANGE(_name, _type) \
	static void \
	_name(struct cute_assess *                    assess, \
	      const struct cute_assess_ops *          ops, \
	      const struct cute_ ## _type ## _range * expect) \
	{ \
		cute_assert_intern(assess); \
		cute_assess_assert_ops_intern(ops); \
		cute_assert_intern(expect->expr); \
		cute_assert_intern(expect->expr[0]); \
		cute_assert_intern(expect->min < expect->max); \
		\
		assess->ops = ops; \
		assess->expect._type.range = *expect; \
	}

#define CUTE_ASSESS_TYPED_RANGE(_name, _type) \
	static bool \
	_name(struct cute_assess *                    assess, \
	      const struct cute_assess_ops *          ops, \
	      const struct cute_ ## _type *           check, \
	      const struct cute_ ## _type ## _range * expect) \
	{ \
		cute_assert_intern(assess); \
		cute_assess_assert_ops_intern(ops); \
		cute_assert_intern(check->expr); \
		cute_assert_intern(check->expr[0]); \
		cute_assert_intern(expect->expr); \
		cute_assert_intern(expect->expr[0]); \
		cute_assert_intern(expect->min < expect->max); \
		\
		const union cute_assess_value chk = { ._type = *check }; \
		\
		cute_assess_build_ ## _type ## _range(assess, ops, expect); \
		\
		return cute_assess_check(assess, &chk); \
	}

#define CUTE_ASSESS_DEFINE_TYPED_RANGE(_type, _assess, _build, _desc, _format) \
	CUTE_ASSESS_DESC_TYPED_RANGE(_desc, _type, _format) \
	CUTE_ASSESS_BUILD_TYPED_RANGE(_build, _type) \
	CUTE_ASSESS_TYPED_RANGE(_assess, _type)

#define CUTE_ASSESS_DESC_RANGE(_name, _type, _op, _inv) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc) \
	{ \
		return cute_assess_desc_ ## _type ## _range(assess, \
		                                            desc, \
		                                            _op, \
		                                            _inv); \
	}

#define CUTE_ASSESS_RANGE(_name, _type, _ops) \
	bool \
	_name(struct cute_assess *                    assess, \
	      const struct cute_ ## _type *           check, \
	      const struct cute_ ## _type ## _range * expect) \
	{ \
		return cute_assess_ ## _type ## _range(assess, \
		                                       _ops, \
		                                       check, \
		                                       expect); \
	}

#define CUTE_ASSESS_DEFINE_RANGE(_type, \
                                 _assess, \
                                 _ops, \
                                 _cmp, \
                                 _desc, \
                                 _op, \
                                 _inv) \
	CUTE_ASSESS_DESC_RANGE(_desc, _type, _op, _inv) \
	CUTE_ASSESS_OPS(_ops, _cmp, _desc, cute_assess_release_null); \
	CUTE_ASSESS_RANGE(_assess, _type, &(_ops))

CUTE_ASSESS_DEFINE_TYPED_RANGE(sint,
                               cute_assess_sint_range,
                               cute_assess_build_sint_range,
                               cute_assess_desc_sint_range,
                               PRIdMAX)

/******************************************************************************
 * Signed integer range handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->sint.value >= assess->expect.sint.range.min) &&
	       (check->sint.value <= assess->expect.sint.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(sint,
                         cute_assess_sint_in_range,
                         cute_assess_sint_in_range_ops,
                         cute_assess_cmp_sint_in_range,
                         cute_assess_desc_sint_in_range,
                         "in",
                         "not in")

static bool
cute_assess_cmp_sint_not_in_range(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return !cute_assess_cmp_sint_in_range(assess, check);
}

CUTE_ASSESS_DEFINE_RANGE(sint,
                         cute_assess_sint_not_in_range,
                         cute_assess_sint_not_in_range_ops,
                         cute_assess_cmp_sint_not_in_range,
                         cute_assess_desc_sint_not_in_range,
                         "not in",
                         "in")

/******************************************************************************
 * Signed integer set handling
 ******************************************************************************/

#define INTMAX_BITS          ((unsigned int)(sizeof(intmax_t) * CHAR_BIT) - 1U)
#define INTMAX_BASE8_DIGITS  BASE8_DIGITS(INTMAX_BITS)
#define INTMAX_BASE10_DIGITS BASE10_DIGITS(INTMAX_BITS)
#define INTMAX_BASE16_DIGITS BASE16_DIGITS(INTMAX_BITS)

static char *
cute_assess_build_sint_item_str(const intmax_t * items, unsigned int count)
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

static char *
cute_assess_build_sint_set_desc(intmax_t                     value,
                                const char *                 inv,
                                const struct cute_sint_set * set)
{
	if (set->count) {
		char * items;
		char * str;

		items = cute_assess_build_sint_item_str(set->items, set->count);
		str = cute_asprintf("[%" PRIdMAX "] %s set {%s}",
		                    value,
		                    inv,
		                    items);
		cute_free(items);

		return str;
	}
	else
		return cute_asprintf("[%" PRIdMAX "] %s set {}", value, inv);
}

static char *
cute_assess_desc_sint_set(const struct cute_assess * assess,
                          enum cute_assess_desc      desc,
                          const char *               oper,
                          const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(assess->check.sint.expr);
	cute_assert_intern(assess->check.sint.expr[0]);
	cute_assert_intern(assess->expect.sint.set.expr);
	cute_assert_intern(assess->expect.sint.set.expr[0]);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		return cute_asprintf("%s %s set %s",
		                     assess->check.sint.expr,
		                     oper,
		                     assess->expect.sint.set.expr);

	case CUTE_ASSESS_FOUND_DESC:
		return cute_assess_build_sint_set_desc(
			assess->check.sint.value,
			inv,
			&assess->expect.sint.set);

	default:
		__cute_unreachable();
	}
}

static void
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

static void
cute_assess_build_sint_set(struct cute_assess *           assess,
                           const struct cute_assess_ops * ops,
                           const struct cute_sint_set *   expect)
{
	cute_assert_intern(assess);
	cute_assess_assert_ops_intern(ops);
	cute_assert_intern(expect->expr);
	cute_assert_intern(expect->expr[0]);

	unsigned int i;
	intmax_t *   items;

	if (expect->count) {
		items = cute_malloc(expect->count * sizeof(items[0]));
		for (i = 0; i < expect->count; i++)
			items[i] = expect->items[i];
	}
	else
		items = NULL;

	assess->ops = ops;
	assess->expect.sint.set = *expect;
	assess->expect.sint.set.items = items;
}

static bool
cute_assess_sint_set(struct cute_assess *           assess,
                     const struct cute_assess_ops * ops,
                     const struct cute_sint *       check,
                     const struct cute_sint_set *   expect)
{
	cute_assert_intern(assess);
	cute_assess_assert_ops_intern(ops);
	cute_assert_intern(check->expr);
	cute_assert_intern(check->expr[0]);
	cute_assert_intern(expect->expr);
	cute_assert_intern(expect->expr[0]);

	const union cute_assess_value chk = { .sint = *check };

	cute_assess_build_sint_set(assess, ops, expect);

	return cute_assess_check(assess, &chk);
}

/******************************************************************************
 * Signed integer set inclusion handling
 ******************************************************************************/

static bool
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

static char *
cute_assess_desc_sint_in_set(const struct cute_assess * assess,
                               enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_set(assess, desc, "in", "not in");
}

static const struct cute_assess_ops cute_assess_sint_in_set_ops = {
	.cmp     = cute_assess_cmp_sint_in_set,
	.desc    = cute_assess_desc_sint_in_set,
	.release = cute_assess_release_sint_set
};

bool
cute_assess_sint_in_set(struct cute_assess *         assess,
                        const struct cute_sint *     check,
                        const struct cute_sint_set * expect)
{
	return cute_assess_sint_set(assess,
	                            &cute_assess_sint_in_set_ops,
	                            check,
	                            expect);
}

/******************************************************************************
 * Signed integer set exclusion handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_sint_in_set(assess, value);
}

static char *
cute_assess_desc_sint_not_in_set(const struct cute_assess * assess,
                                 enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_set(assess, desc, "not in", "in");
}

static const struct cute_assess_ops cute_assess_sint_not_in_set_ops = {
	.cmp     = cute_assess_cmp_sint_not_in_set,
	.desc    = cute_assess_desc_sint_not_in_set,
	.release = cute_assess_release_sint_set
};

bool
cute_assess_sint_not_in_set(struct cute_assess *         assess,
                            const struct cute_sint *     check,
                            const struct cute_sint_set * expect)
{
	return cute_assess_sint_set(assess,
	                            &cute_assess_sint_not_in_set_ops,
	                            check,
	                            expect);
}

#if 0
#define UINTMAX_BITS              ((unsigned int)sizeof(uintmax_t) * CHAR_BIT)
#define UINTMAX_BASE8_DIGITS      BASE8_DIGITS(UINTMAX_BITS)
#define UINTMAX_BASE10_DIGITS     BASE10_DIGITS(UINTMAX_BITS)
#define UINTMAX_BASE16_DIGITS     BASE16_DIGITS(UINTMAX_BITS)
#endif

/******************************************************************************
 * Unsigned integer value handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_VALUE(uint,
                               cute_assess_uint_value,
                               cute_assess_build_uint_value,
                               cute_assess_desc_uint_value,
                               PRIuMAX)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_equal,
                         cute_assess_uint_equal_ops,
                         cute_assess_cmp_uint_equal,
                         cute_assess_desc_uint_equal,
                         ==,
                         !=)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_unequal,
                         cute_assess_uint_unequal_ops,
                         cute_assess_cmp_uint_unequal,
                         cute_assess_desc_uint_unequal,
                         !=,
                         ==)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_greater,
                         cute_assess_uint_greater_ops,
                         cute_assess_cmp_uint_greater,
                         cute_assess_desc_uint_greater,
                         >,
                         <=)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_greater_equal,
                         cute_assess_uint_greater_equal_ops,
                         cute_assess_cmp_uint_greater_equal,
                         cute_assess_desc_uint_greater_equal,
                         >=,
                         <)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_lower,
                         cute_assess_uint_lower_ops,
                         cute_assess_cmp_uint_lower,
                         cute_assess_desc_uint_lower,
                         <,
                         >=)

CUTE_ASSESS_DEFINE_VALUE(uint,
                         cute_assess_uint_lower_equal,
                         cute_assess_uint_lower_equal_ops,
                         cute_assess_cmp_uint_lower_equal,
                         cute_assess_desc_uint_lower_equal,
                         <=,
                         >)

/******************************************************************************
 * Unsigned integer range handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_RANGE(uint,
                               cute_assess_uint_range,
                               cute_assess_build_uint_range,
                               cute_assess_desc_uint_range,
                               PRIuMAX)

static bool
cute_assess_cmp_uint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->uint.value >= assess->expect.uint.range.min) &&
	       (check->uint.value <= assess->expect.uint.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(uint,
                         cute_assess_uint_in_range,
                         cute_assess_uint_in_range_ops,
                         cute_assess_cmp_uint_in_range,
                         cute_assess_desc_uint_in_range,
                         "in",
                         "not in")

static bool
cute_assess_cmp_uint_not_in_range(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return !cute_assess_cmp_uint_in_range(assess, check);
}

CUTE_ASSESS_DEFINE_RANGE(uint,
                         cute_assess_uint_not_in_range,
                         cute_assess_uint_not_in_range_ops,
                         cute_assess_cmp_uint_not_in_range,
                         cute_assess_desc_uint_not_in_range,
                         "not in",
                         "in")

/******************************************************************************
 * Floating point number value handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_VALUE(flt,
                               cute_assess_flt_value,
                               cute_assess_build_flt_value,
                               cute_assess_desc_flt_value,
                               "Lf")

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_equal,
                         cute_assess_flt_equal_ops,
                         cute_assess_cmp_flt_equal,
                         cute_assess_desc_flt_equal,
                         ==,
                         !=)

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_unequal,
                         cute_assess_flt_unequal_ops,
                         cute_assess_cmp_flt_unequal,
                         cute_assess_desc_flt_unequal,
                         !=,
                         ==)

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_greater,
                         cute_assess_flt_greater_ops,
                         cute_assess_cmp_flt_greater,
                         cute_assess_desc_flt_greater,
                         >,
                         <=)

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_greater_equal,
                         cute_assess_flt_greater_equal_ops,
                         cute_assess_cmp_flt_greater_equal,
                         cute_assess_desc_flt_greater_equal,
                         >=,
                         <)

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_lower,
                         cute_assess_flt_lower_ops,
                         cute_assess_cmp_flt_lower,
                         cute_assess_desc_flt_lower,
                         <,
                         >=)

CUTE_ASSESS_DEFINE_VALUE(flt,
                         cute_assess_flt_lower_equal,
                         cute_assess_flt_lower_equal_ops,
                         cute_assess_cmp_flt_lower_equal,
                         cute_assess_desc_flt_lower_equal,
                         <=,
                         >)

/******************************************************************************
 * Floating point range handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_RANGE(flt,
                               cute_assess_flt_range,
                               cute_assess_build_flt_range,
                               cute_assess_desc_flt_range,
                               "Lf")

static bool
cute_assess_cmp_flt_in_range(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return (check->flt.value >= assess->expect.flt.range.min) &&
	       (check->flt.value <= assess->expect.flt.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(flt,
                         cute_assess_flt_in_range,
                         cute_assess_flt_in_range_ops,
                         cute_assess_cmp_flt_in_range,
                         cute_assess_desc_flt_in_range,
                         "in",
                         "not in")

static bool
cute_assess_cmp_flt_not_in_range(const struct cute_assess *      assess,
                                  const union cute_assess_value * check)
{
	return !cute_assess_cmp_flt_in_range(assess, check);
}

CUTE_ASSESS_DEFINE_RANGE(flt,
                         cute_assess_flt_not_in_range,
                         cute_assess_flt_not_in_range_ops,
                         cute_assess_cmp_flt_not_in_range,
                         cute_assess_desc_flt_not_in_range,
                         "not in",
                         "in")

/******************************************************************************
 * Top-level generic assess handling
 ******************************************************************************/

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

char *
cute_assess_desc(const struct cute_assess * assess,
                 enum cute_assess_desc      desc)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_desc_intern(desc);

	return assess->ops->desc(assess, desc);
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
