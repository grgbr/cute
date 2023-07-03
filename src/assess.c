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
				"[" _format "] %s [" _format "]", \
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

#define CUTE_ASSESS_DEFINE_TYPED_VALUE(_type, _format) \
	CUTE_ASSESS_DESC_TYPED_VALUE(cute_assess_desc_ ## _type ## _value, \
	                             _type, \
	                             _format) \
	CUTE_ASSESS_BUILD_TYPED_VALUE(cute_assess_build_ ## _type ## _value, \
	                              _type) \
	CUTE_ASSESS_TYPED_VALUE(cute_assess_ ## _type ## _value, \
	                        _type)

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

#define CUTE_ASSESS_DEFINE_VALUE(_type, _op_name, _op, _inv) \
	CUTE_ASSESS_CMP_VALUE(cute_assess_cmp_ ## _type ## _op_name, \
	                      _type, \
	                      _op) \
	CUTE_ASSESS_DESC_VALUE(cute_assess_desc_ ## _type ## _op_name, \
	                       _type, \
	                       _op, \
	                       _inv) \
	CUTE_ASSESS_OPS(cute_assess_ ## _type ## _op_name ## _ops, \
	                cute_assess_cmp_ ## _type ## _op_name, \
	                cute_assess_desc_ ## _type ## _op_name, \
	                cute_assess_release_null); \
	CUTE_ASSESS_VALUE(cute_assess_ ## _type ## _ ## _op_name, \
	                  _type, \
	                  &(cute_assess_ ## _type ## _op_name ## _ops))

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
			return cute_asprintf("[" _format "] %s " \
			                     "range {" _format \
			                     " ... " _format "}", \
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

#define CUTE_ASSESS_DEFINE_TYPED_RANGE(_type, _format) \
	CUTE_ASSESS_DESC_TYPED_RANGE(cute_assess_desc_ ## _type ## _range, \
	                             _type, \
	                             _format) \
	CUTE_ASSESS_BUILD_TYPED_RANGE(cute_assess_build_ ## _type ## _range, \
	                              _type) \
	CUTE_ASSESS_TYPED_RANGE(cute_assess_ ## _type ## _range, \
	                        _type)

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

#define CUTE_ASSESS_DEFINE_RANGE(_type, _assess, _cmp, _op, _op_str, _inv_str) \
	CUTE_ASSESS_DESC_RANGE( \
		cute_assess_desc_ ## _type ## _ ## _op ## _range, \
		_type, \
		_op_str, \
		_inv_str) \
	CUTE_ASSESS_OPS(cute_assess_ ## _type ## _ ## _op ## _range_ops, \
	                _cmp, \
	                cute_assess_desc_ ## _type ## _ ## _op ## _range, \
	                cute_assess_release_null); \
	CUTE_ASSESS_RANGE(_assess, \
	                  _type, \
	                  &(cute_assess_ ## _type ## _ ## _op ## _range_ops))

/******************************************************************************
 * Common set handling
 ******************************************************************************/

#define CUTE_ASSESS_DESC_TYPED_SET(_name, _type, _build_desc) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc, \
	      const char *               oper, \
	      const char *               inv) \
	{ \
		cute_assess_assert_intern(assess); \
		cute_assert_intern(assess->check._type.expr); \
		cute_assert_intern(assess->check._type.expr[0]); \
		cute_assert_intern(assess->expect._type.set.expr); \
		cute_assert_intern(assess->expect._type.set.expr[0]); \
		cute_assess_assert_desc_intern(desc); \
		cute_assert_intern(oper); \
		cute_assert_intern(oper[0]); \
		cute_assert_intern(inv); \
		cute_assert_intern(inv[0]); \
		\
		switch (desc) { \
		case CUTE_ASSESS_EXPECT_DESC: \
			return cute_asprintf("%s %s set %s", \
			                     assess->check._type.expr, \
			                     oper, \
			                     assess->expect._type.set.expr); \
		\
		case CUTE_ASSESS_FOUND_DESC: \
			return _build_desc(assess->check._type.value, \
			                   inv, \
			                   &assess->expect._type.set); \
		\
		default: \
			__cute_unreachable(); \
		} \
	}

#define CUTE_ASSESS_RELEASE_TYPED_SET(_name, _type) \
	static void \
	_name(struct cute_assess * assess) \
	{ \
		cute_assess_assert_intern(assess); \
		cute_assert_intern(assess->expect._type.set.expr); \
		cute_assert_intern(assess->expect._type.set.expr[0]); \
		cute_assert_intern(!assess->expect._type.set.count || \
		                    assess->expect._type.set.items); \
		\
		if (assess->expect._type.set.count) { \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wcast-qual\"") \
			cute_free((void *)assess->expect._type.set.items); \
_Pragma("GCC diagnostic pop") \
		} \
	}

#define CUTE_ASSESS_BUILD_TYPED_SET(_name, _type) \
	static void \
	_name(struct cute_assess *                  assess, \
	      const struct cute_assess_ops *        ops, \
	      const struct cute_ ## _type ## _set * expect) \
	{ \
		cute_assert_intern(assess); \
		cute_assess_assert_ops_intern(ops); \
		cute_assert_intern(expect->expr); \
		cute_assert_intern(expect->expr[0]); \
		\
		unsigned int                                       i; \
		cute_typeof_member(struct cute_ ## _type, value) * items; \
		\
		if (expect->count) { \
			items = cute_malloc(expect->count * sizeof(items[0])); \
			for (i = 0; i < expect->count; i++) \
				items[i] = expect->items[i]; \
		} \
		else \
			items = NULL; \
		\
		assess->ops = ops; \
		assess->expect._type.set = *expect; \
		assess->expect._type.set.items = items; \
	}

#define CUTE_ASSESS_TYPED_SET(_name, _type) \
	static bool \
	_name(struct cute_assess *                  assess, \
	      const struct cute_assess_ops *        ops, \
	      const struct cute_ ## _type *         check, \
	      const struct cute_ ## _type ## _set * expect) \
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
		cute_assess_build_ ## _type ## _set(assess, ops, expect); \
		\
		return cute_assess_check(assess, &chk); \
	}

#define CUTE_ASSESS_DEFINE_TYPED_SET(_type, _build_desc) \
	CUTE_ASSESS_DESC_TYPED_SET(cute_assess_desc_ ## _type ## _set, \
	                           _type, \
	                           cute_assess_build_ ## _type ## _set_desc) \
	CUTE_ASSESS_RELEASE_TYPED_SET(cute_assess_release_ ## _type ## _set, \
	                              _type) \
	CUTE_ASSESS_BUILD_TYPED_SET(cute_assess_build_ ## _type ## _set, \
	                            _type) \
	CUTE_ASSESS_TYPED_SET(cute_assess_ ## _type ## _set, _type)

#define CUTE_ASSESS_DESC_SET(_name, _type, _op_str, _inv_str) \
	static char * \
	_name(const struct cute_assess * assess, \
	      enum cute_assess_desc      desc) \
	{ \
		return cute_assess_desc_ ## _type ## _set(assess, \
		                                          desc, \
		                                          _op_str, \
		                                          _inv_str); \
	}

#define CUTE_ASSESS_SET(_name, _type, _ops) \
	bool \
	_name(struct cute_assess *                  assess, \
	      const struct cute_ ## _type *         check, \
	      const struct cute_ ## _type ## _set * expect) \
	{ \
		return cute_assess_ ## _type ## _set(assess, \
		                                     _ops, \
		                                     check, \
		                                     expect); \
	}

#define CUTE_ASSESS_DEFINE_SET(_type, _assess, _cmp, _op, _op_str, _inv_str) \
	CUTE_ASSESS_DESC_SET(cute_assess_desc_ ## _type ## _ ## _op ## _set, \
	                     _type, \
	                     _op_str, \
	                     _inv_str) \
	CUTE_ASSESS_OPS(cute_assess_ ## _type ## _ ## _op ## _set_ops, \
	                _cmp, \
	                cute_assess_desc_ ## _type ## _ ## _op ## _set, \
	                cute_assess_release_ ## _type ## _set); \
	CUTE_ASSESS_SET(_assess, \
	                _type, \
	                &(cute_assess_ ## _type ## _ ## _op ## _set_ops))

/******************************************************************************
 * Signed integer value handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_VALUE(sint, "%" PRIdMAX)

CUTE_ASSESS_DEFINE_VALUE(sint, equal,         ==, !=)
CUTE_ASSESS_DEFINE_VALUE(sint, unequal,       !=, ==)
CUTE_ASSESS_DEFINE_VALUE(sint, greater,       >, <=)
CUTE_ASSESS_DEFINE_VALUE(sint, greater_equal, >=, <)
CUTE_ASSESS_DEFINE_VALUE(sint, lower,         <, >=)
CUTE_ASSESS_DEFINE_VALUE(sint, lower_equal,   <=, >)

/******************************************************************************
 * Signed integer range handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_RANGE(sint, "%" PRIdMAX)

static bool
cute_assess_cmp_sint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->sint.value >= assess->expect.sint.range.min) &&
	       (check->sint.value <= assess->expect.sint.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(sint,
                         cute_assess_sint_in_range,
                         cute_assess_cmp_sint_in_range,
                         in,
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
                         cute_assess_cmp_sint_not_in_range,
                         not_in,
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

CUTE_ASSESS_DEFINE_TYPED_SET(sint, cute_assess_build_sint_set_desc)

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

CUTE_ASSESS_DEFINE_SET(sint,
                       cute_assess_sint_in_set,
                       cute_assess_cmp_sint_in_set,
                       in,
                       "in",
                       "not in")

static bool
cute_assess_cmp_sint_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_sint_in_set(assess, value);
}

CUTE_ASSESS_DEFINE_SET(sint,
                       cute_assess_sint_not_in_set,
                       cute_assess_cmp_sint_not_in_set,
                       not_in,
                       "not in",
                       "in")

/******************************************************************************
 * Unsigned integer value handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_VALUE(uint, "%" PRIuMAX)

CUTE_ASSESS_DEFINE_VALUE(uint, equal,         ==, !=)
CUTE_ASSESS_DEFINE_VALUE(uint, unequal,       !=, ==)
CUTE_ASSESS_DEFINE_VALUE(uint, greater,       >, <=)
CUTE_ASSESS_DEFINE_VALUE(uint, greater_equal, >=, <)
CUTE_ASSESS_DEFINE_VALUE(uint, lower,         <, >=)
CUTE_ASSESS_DEFINE_VALUE(uint, lower_equal,   <=, >)

/******************************************************************************
 * Unsigned integer range handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_RANGE(uint, "%" PRIuMAX)

static bool
cute_assess_cmp_uint_in_range(const struct cute_assess *      assess,
                              const union cute_assess_value * check)
{
	return (check->uint.value >= assess->expect.uint.range.min) &&
	       (check->uint.value <= assess->expect.uint.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(uint,
                         cute_assess_uint_in_range,
                         cute_assess_cmp_uint_in_range,
                         in,
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
                         cute_assess_cmp_uint_not_in_range,
                         not_in,
                         "not in",
                         "in")

/******************************************************************************
 * Unsigned integer set handling
 ******************************************************************************/

#define UINTMAX_BITS          ((unsigned int)sizeof(uintmax_t) * CHAR_BIT)
#define UINTMAX_BASE8_DIGITS  BASE8_DIGITS(UINTMAX_BITS)
#define UINTMAX_BASE10_DIGITS BASE10_DIGITS(UINTMAX_BITS)
#define UINTMAX_BASE16_DIGITS BASE16_DIGITS(UINTMAX_BITS)

static char *
cute_assess_build_uint_item_str(const uintmax_t * items, unsigned int count)
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

static char *
cute_assess_build_uint_set_desc(uintmax_t                    value,
                                const char *                 inv,
                                const struct cute_uint_set * set)
{
	if (set->count) {
		char * items;
		char * str;

		items = cute_assess_build_uint_item_str(set->items, set->count);
		str = cute_asprintf("[%" PRIuMAX "] %s set {%s}",
		                    value,
		                    inv,
		                    items);
		cute_free(items);

		return str;
	}
	else
		return cute_asprintf("[%" PRIuMAX "] %s set {}", value, inv);
}

CUTE_ASSESS_DEFINE_TYPED_SET(uint, cute_assess_build_uint_set_desc)

static bool
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

CUTE_ASSESS_DEFINE_SET(uint,
                       cute_assess_uint_in_set,
                       cute_assess_cmp_uint_in_set,
                       in,
                       "in",
                       "not in")

static bool
cute_assess_cmp_uint_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_uint_in_set(assess, value);
}

CUTE_ASSESS_DEFINE_SET(uint,
                       cute_assess_uint_not_in_set,
                       cute_assess_cmp_uint_not_in_set,
                       not_in,
                       "not in",
                       "in")

/******************************************************************************
 * Floating point value handling
 ******************************************************************************/

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
 * The number of digits used to express a floating point number's mantissa as
 * text (without the fractional dot '.').
 */
#define CUTE_FLT_MANT_DIGITS 10

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

#define CUTE_FLT_FORMAT_STR \
	"%." CUTE_STRING(CUTE_FLT_MANT_DIGITS) "Lg"

CUTE_ASSESS_DEFINE_TYPED_VALUE(flt, CUTE_FLT_FORMAT_STR)

CUTE_ASSESS_DEFINE_VALUE(flt, equal,         ==, !=)
CUTE_ASSESS_DEFINE_VALUE(flt, unequal,       !=, ==)
CUTE_ASSESS_DEFINE_VALUE(flt, greater,       >, <=)
CUTE_ASSESS_DEFINE_VALUE(flt, greater_equal, >=, <)
CUTE_ASSESS_DEFINE_VALUE(flt, lower,         <, >=)
CUTE_ASSESS_DEFINE_VALUE(flt, lower_equal,   <=, >)

/******************************************************************************
 * Floating point range handling
 ******************************************************************************/

CUTE_ASSESS_DEFINE_TYPED_RANGE(flt, CUTE_FLT_FORMAT_STR)

static bool
cute_assess_cmp_flt_in_range(const struct cute_assess *      assess,
                             const union cute_assess_value * check)
{
	return (check->flt.value >= assess->expect.flt.range.min) &&
	       (check->flt.value <= assess->expect.flt.range.max);
}

CUTE_ASSESS_DEFINE_RANGE(flt,
                         cute_assess_flt_in_range,
                         cute_assess_cmp_flt_in_range,
                         in,
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
                         cute_assess_cmp_flt_not_in_range,
                         not_in,
                         "not in",
                         "in")

/******************************************************************************
 * Floating point set handling
 ******************************************************************************/

static char *
cute_assess_build_flt_item_str(const long double * items, unsigned int count)
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

static char *
cute_assess_build_flt_set_desc(long double                 value,
                               const char *                inv,
                               const struct cute_flt_set * set)
{
	if (set->count) {
		char * items;
		char * str;

		items = cute_assess_build_flt_item_str(set->items, set->count);
		str = cute_asprintf("[" CUTE_FLT_FORMAT_STR "] %s set {%s}",
		                    value,
		                    inv,
		                    items);
		cute_free(items);

		return str;
	}
	else
		return cute_asprintf("[" CUTE_FLT_FORMAT_STR "] %s set {}",
		                     value,
		                     inv);
}

CUTE_ASSESS_DEFINE_TYPED_SET(flt, cute_assess_build_flt_set_desc)

static bool
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

CUTE_ASSESS_DEFINE_SET(flt,
                       cute_assess_flt_in_set,
                       cute_assess_cmp_flt_in_set,
                       in,
                       "in",
                       "not in")

static bool
cute_assess_cmp_flt_not_in_set(const struct cute_assess *      assess,
                                const union cute_assess_value * value)
{
	return !cute_assess_cmp_flt_in_set(assess, value);
}

CUTE_ASSESS_DEFINE_SET(flt,
                       cute_assess_flt_not_in_set,
                       cute_assess_cmp_flt_not_in_set,
                       not_in,
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
