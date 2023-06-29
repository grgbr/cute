#ifndef _CUTE_ASSESS_H
#define _CUTE_ASSESS_H

#include "common.h"
#include <stdint.h>
#include <stdio.h>

union cute_assess_member {
	intmax_t    i;
	uintmax_t   u;
	long double f;
	void *      p;
	char *      s;
};

struct cute_assess_range {
	union cute_assess_member min;
	union cute_assess_member max;
};

struct cute_assess_set {
	unsigned int               count;
	union cute_assess_member * items;
};

enum cute_assess_desc {
	CUTE_ASSESS_EXPECT_DESC = 0,
	CUTE_ASSESS_FOUND_DESC,
	CUTE_ASSESS_DESC_NR
};

struct cute_assess;

typedef bool (cute_assess_cmp_fn)(const struct cute_assess * assess,
                                  union cute_assess_member   value);

typedef char * (cute_assess_desc_fn)(const struct cute_assess * assess,
                                     enum cute_assess_desc      desc);

struct cute_assess {
	cute_assess_cmp_fn *             cmp;
	cute_assess_desc_fn *            desc;
	const char *                     chk_expr;
	const char *                     xpct_expr;
	union {
		union cute_assess_member member;
		struct cute_assess_range range;
		struct cute_assess_set   set;
	}                                xpct_val;
	union cute_assess_member         chk_val;
};

extern void
cute_assess_build_null(struct cute_assess * assess);

extern void
cute_assess_build_expr(struct cute_assess * assess,
                       const char *         chk_expr,
                       const char *         xpct_expr);

extern void
cute_assess_build_excp(struct cute_assess * assess, int sig);

extern bool
cute_assess_check(struct cute_assess * assess, union cute_assess_member value);

extern char *
cute_assess_desc(const struct cute_assess * assess, enum cute_assess_desc desc);

extern bool
cute_assess_sint_equal(struct cute_assess * assess,
                       const char *         chk_expr,
                       intmax_t             chk_val,
                       const char *         xpct_expr,
                       intmax_t             xpct_val);

extern bool
cute_assess_sint_unequal(struct cute_assess * assess,
                         const char *         chk_expr,
                         intmax_t             chk_val,
                         const char *         xpct_expr,
                         intmax_t             xpct_val);

extern bool
cute_assess_sint_greater(struct cute_assess * assess,
                         const char *         chk_expr,
                         intmax_t             chk_val,
                         const char *         xpct_expr,
                         intmax_t             xpct_val);

extern bool
cute_assess_sint_greater_or_equal(struct cute_assess * assess,
                                  const char *         chk_expr,
                                  intmax_t             chk_val,
                                  const char *         xpct_expr,
                                  intmax_t             xpct_val);

extern bool
cute_assess_sint_lower(struct cute_assess * assess,
                       const char *         chk_expr,
                       intmax_t             chk_val,
                       const char *         xpct_expr,
                       intmax_t             xpct_val);

extern bool
cute_assess_sint_lower_or_equal(struct cute_assess * assess,
                                const char *         chk_expr,
                                intmax_t             chk_val,
                                const char *         xpct_expr,
                                intmax_t             xpct_val);

extern bool
cute_assess_sint_in_range(struct cute_assess * assess,
                          const char *         chk_expr,
                          intmax_t             chk_val,
                          const char *         xpct_expr,
                          intmax_t             xpct_min,
                          intmax_t             xpct_max);

#endif /* _CUTE_ASSESS_H */
