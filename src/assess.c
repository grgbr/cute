#include "assess.h"
#include "cute/ensure.h"
#include "run.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>

/*
int / float:
    ==  equals
    !=  unequals
    >   is_greater_than
    >=  greater_or_equal
    <   lower
    <=  lower_or_equal
    in_set
    not_in_set
    in_range
    not_in_range

ptr:
    ==  equals
    !=  unequals
    >   is_greater_than
    >=  greater_or_equal
    <   lower
    <=  lower_or_equal
    in_range
    not_in_range

string:
    starts_with
    ends_with
    contains
    equals
    unequals
    longer
    shorter
    greater
    greater_or_equal
    lower
    lower_or_equal

memory:
    equals
    unequals
*/

#define cute_assess_assert_set(_set) \
	cute_assert(_set); \
	cute_assert((_set)->count); \
	cute_assert((_set)->items)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_set_intern(_set) \
	cute_assess_assert_set(_set)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_set_intern(_set)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_desc(_desc) \
	cute_assert((_desc) >= 0); \
	cute_assert((_desc) < CUTE_ASSESS_DESC_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_desc_intern(_desc) \
	cute_assess_assert_desc(_desc)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_desc_intern(_desc)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert(_assess) \
	cute_assert(_assess); \
	cute_assert((_assess)->cmp); \
	cute_assert((_assess)->desc); \
	cute_assert((_assess)->chk_expr); \
	cute_assert((_assess)->chk_expr[0]); \
	cute_assert((_assess)->xpct_expr); \
	cute_assert((_assess)->xpct_expr[0])

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_intern(_assess) \
	cute_assess_assert(_assess)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_intern(_assess)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static char *
cute_assess_desc_null(const struct cute_assess * assess __cute_unused,
                      enum cute_assess_desc      desc __cute_unused)
{
	cute_assess_assert_desc_intern(desc);

	return NULL;
}

void
cute_assess_build_null(struct cute_assess * assess)
{
	cute_assert_intern(assess);

	assess->desc = cute_assess_desc_null;
}

static char *
cute_assess_desc_expr(const struct cute_assess * assess,
                      enum cute_assess_desc      desc)
{
	cute_assert_intern(assess);
	cute_assert_intern(assess->desc);
	cute_assess_assert_desc_intern(desc);

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		if (!assess->xpct_expr)
			return NULL;
		return cute_dup(assess->xpct_expr);

	case CUTE_ASSESS_FOUND_DESC:
		if (!assess->chk_expr)
			return NULL;
		return cute_dup(assess->chk_expr);

	default:
		__cute_unreachable();
	}
}

void
cute_assess_build_expr(struct cute_assess * assess,
                       const char *         chk_expr,
                       const char *         xpct_expr)
{
	cute_assert_intern(assess);
	cute_assert_intern(!chk_expr || chk_expr[0]);
	cute_assert_intern(!xpct_expr || xpct_expr[0]);

	assess->desc = &cute_assess_desc_expr;
	assess->chk_expr = chk_expr;
	assess->xpct_expr = xpct_expr;
}

static char *
cute_assess_desc_excp(const struct cute_assess * assess,
                      enum cute_assess_desc      desc)
{
	cute_assert_intern(assess);
	cute_assert_intern(assess->desc);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern((int)assess->chk_val.i);
	cute_assert_intern((int)assess->chk_val.i != SIGKILL);
	cute_assert_intern((int)assess->chk_val.i != SIGSTOP);
	cute_assert_intern((int)assess->chk_val.i <= NSIG ||
	                   ((int)assess->chk_val.i >= SIGRTMIN &&
	                    (int)assess->chk_val.i <= SIGRTMAX));

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		return NULL;

	case CUTE_ASSESS_FOUND_DESC:
		return cute_asprintf("%s (%d)",
		                     strsignal((int)assess->chk_val.i),
		                     (int)assess->chk_val.i);

	default:
		__cute_unreachable();
	}
}

void
cute_assess_build_excp(struct cute_assess * assess, int sig)
{
	cute_assert_intern(assess);
	cute_assert_intern(sig);
	cute_assert_intern(sig != SIGKILL);
	cute_assert_intern(sig != SIGSTOP);
	cute_assert_intern(sig <= NSIG || (sig >= SIGRTMIN && sig <= SIGRTMAX));

	assess->desc = &cute_assess_desc_excp;
	assess->chk_val.i = sig;
}

bool
cute_assess_check(struct cute_assess * assess, union cute_assess_member value)
{
	cute_assess_assert_intern(assess);

	if (assess->cmp(assess, value))
		return true;

	assess->chk_val = value;

	return false;
}

char *
cute_assess_desc(const struct cute_assess * assess,
                 enum cute_assess_desc      desc)
{
	cute_assert_intern(assess);
	cute_assert_intern(assess->desc);
	cute_assess_assert_desc_intern(desc);

	return assess->desc(assess, desc);
}

static char *
cute_assess_desc_sint_member(const struct cute_assess * assess,
                             enum cute_assess_desc      desc,
                             const char *               oper,
                             const char *               inv)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern(oper);
	cute_assert_intern(oper[0]);
	cute_assert_intern(inv);
	cute_assert_intern(inv[0]);

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		return cute_asprintf("%s %s %s",
		                     assess->chk_expr,
		                     oper,
		                     assess->xpct_expr);

	case CUTE_ASSESS_FOUND_DESC:
		return cute_asprintf("[%" PRIdMAX "] %s [%" PRIdMAX "]",
		                     assess->chk_val.i,
		                     inv,
		                     assess->xpct_val.member.i);

	default:
		__cute_unreachable();
	}
}

static void
cute_assess_build_sint_member(struct cute_assess *  assess,
                              cute_assess_cmp_fn *  cmp,
                              cute_assess_desc_fn * desc,
                              const char *          chk_expr,
                              const char *          xpct_expr,
                              intmax_t              xpct_val)
{
	cute_assert_intern(assess);
	cute_assert_intern(cmp);
	cute_assert_intern(desc);
	cute_assert_intern(chk_expr);
	cute_assert_intern(chk_expr[0]);
	cute_assert_intern(xpct_expr);
	cute_assert_intern(xpct_expr[0]);

	assess->cmp = cmp;
	assess->desc = desc;
	assess->chk_expr = chk_expr;
	assess->xpct_expr = xpct_expr;
	assess->xpct_val.member.i = xpct_val;
}

static bool
cute_assess_sint_member(struct cute_assess *  assess,
                        cute_assess_cmp_fn *  cmp,
                        cute_assess_desc_fn * desc,
                        const char *          chk_expr,
                        intmax_t              chk_val,
                        const char *          xpct_expr,
                        intmax_t              xpct_val)
{
	cute_assert_intern(assess);
	cute_assert_intern(cmp);
	cute_assert_intern(desc);
	cute_assert_intern(chk_expr);
	cute_assert_intern(chk_expr[0]);
	cute_assert_intern(xpct_expr);
	cute_assert_intern(xpct_expr[0]);

	union cute_assess_member val = { .i = chk_val };

	cute_assess_build_sint_member(assess,
	                              cmp,
	                              desc,
	                              chk_expr,
	                              xpct_expr,
	                              xpct_val);

	return cute_assess_check(assess, val);
}

/******************************************************************************
 * Signed integer equality handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_equal(const struct cute_assess * assess,
                           union cute_assess_member   value)
{
	return value.i == assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_equal(const struct cute_assess * assess,
                            enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, "==", "!=");
}

bool
cute_assess_sint_equal(struct cute_assess * assess,
                       const char *         chk_expr,
                       intmax_t             chk_val,
                       const char *         xpct_expr,
                       intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_equal,
	                               cute_assess_desc_sint_equal,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

/******************************************************************************
 * Signed integer unequality handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_unequal(const struct cute_assess * assess,
                             union cute_assess_member   value)
{
	return value.i != assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_unequal(const struct cute_assess * assess,
                              enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, "!=", "==");
}

bool
cute_assess_sint_unequal(struct cute_assess * assess,
                         const char *         chk_expr,
                         intmax_t             chk_val,
                         const char *         xpct_expr,
                         intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_unequal,
	                               cute_assess_desc_sint_unequal,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

/******************************************************************************
 * Signed integer greater than operation handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_greater(const struct cute_assess * assess,
                             union cute_assess_member   value)
{
	return value.i > assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_greater(const struct cute_assess * assess,
                              enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, ">", "<=");
}

bool
cute_assess_sint_greater(struct cute_assess * assess,
                         const char *         chk_expr,
                         intmax_t             chk_val,
                         const char *         xpct_expr,
                         intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_greater,
	                               cute_assess_desc_sint_greater,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

/******************************************************************************
 * Signed integer greater than or equal to operation handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_greater_or_equal(const struct cute_assess * assess,
                                      union cute_assess_member   value)
{
	return value.i >= assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_greater_or_equal(const struct cute_assess * assess,
                                       enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, ">=", "<");
}

bool
cute_assess_sint_greater_or_equal(struct cute_assess * assess,
                                  const char *         chk_expr,
                                  intmax_t             chk_val,
                                  const char *         xpct_expr,
                                  intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_greater_or_equal,
	                               cute_assess_desc_sint_greater_or_equal,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

/******************************************************************************
 * Signed integer lower than operation handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_lower(const struct cute_assess * assess,
                           union cute_assess_member   value)
{
	return value.i < assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_lower(const struct cute_assess * assess,
                            enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, "<", ">=");
}

bool
cute_assess_sint_lower(struct cute_assess * assess,
                       const char *         chk_expr,
                       intmax_t             chk_val,
                       const char *         xpct_expr,
                       intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_lower,
	                               cute_assess_desc_sint_lower,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

/******************************************************************************
 * Signed integer lower than or equal to operation handling
 ******************************************************************************/

static bool
cute_assess_cmp_sint_lower_or_equal(const struct cute_assess * assess,
                                    union cute_assess_member   value)
{
	return value.i <= assess->xpct_val.member.i;
}

static char *
cute_assess_desc_sint_lower_or_equal(const struct cute_assess * assess,
                                     enum cute_assess_desc      desc)
{
	return cute_assess_desc_sint_member(assess, desc, "<=", ">");
}

bool
cute_assess_sint_lower_or_equal(struct cute_assess * assess,
                                const char *         chk_expr,
                                intmax_t             chk_val,
                                const char *         xpct_expr,
                                intmax_t             xpct_val)
{
	return cute_assess_sint_member(assess,
	                               cute_assess_cmp_sint_lower_or_equal,
	                               cute_assess_desc_sint_lower_or_equal,
	                               chk_expr,
	                               chk_val,
	                               xpct_expr,
	                               xpct_val);
}

#if 0
static bool
cute_assess_cmp_int_range(const struct cute_assess * assess,
                          union cute_assess_member   value)
{
	cute_assess_assert_intern(assess);

	switch (assess->oper) {
	case CUTE_ASSESS_INRANGE_OPER:
		return (value.i >= assess->xpct_val.range.min) &&
		       (value.i <= assess->xpct_val.range.max);

	case CUTE_ASSESS_NOT_INRANGE_OPER:
		return (value.i < assess->xpct_val.range.min) &&
		       (value.i > assess->xpct_val.range.max);

	default:
		__cute_unreachable();
	}
}

static bool
cute_assess_cmp_int_set(const struct cute_assess * assess,
                        union cute_assess_member   value)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_set_intern(&assess->xpct_val.set);

	unsigned int c;

	switch (assess->oper) {
	case CUTE_ASSESS_INSET_OPER:
		for (c = 0; c < assess->xpct_val.set.count; c++)
			if (value == assess->xpct_val.set.items[c].i)
				return true;

		return false;

	case CUTE_ASSESS_NOT_INSET_OPER:
		for (c = 0; c < assess->xpct_val.set.count; c++)
			if (value == assess->xpct_val.set.items[c].i)
				return false;

		return true;

	default:
		__cute_unreachable();
	}
}

void
cute_check_ptr(const void * a, enum cute_oper oper, const void * b)

void
cute_check_str(const char * a, enum cute_oper oper, const char * b)

void
cute_check_mem(const void * a, enum cute_oper oper, const void * b, size_t size)
#endif
