#ifndef _CUTE_ASSESS_H
#define _CUTE_ASSESS_H

#include "common.h"
#include "cute/check.h"
#include <stdint.h>
#include <stdio.h>

struct cute_assess;
union cute_assess_value;

typedef bool (cute_assess_cmp_fn)(const struct cute_assess *      assess,
                                  const union cute_assess_value * value);

typedef struct cute_text_block * (cute_assess_desc_fn)
                                 (const struct cute_assess * assess);

typedef void (cute_assess_release_fn)(struct cute_assess * assess);

struct cute_assess_ops {
	cute_assess_cmp_fn *     cmp;
	cute_assess_desc_fn *    desc;
	cute_assess_release_fn * release;
};

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

union cute_assess_value {
	const char *      expr;
	struct cute_sint  sint;
	struct cute_uint  uint;
	struct cute_flt   flt;
	struct cute_str   str;
	struct cute_ptr   ptr;
};

union cute_sint_assess {
	struct cute_sint       scal;
	struct cute_sint_range range;
	struct cute_sint_set   set;
};

union cute_uint_assess {
	struct cute_uint       scal;
	struct cute_uint_range range;
	struct cute_uint_set   set;
};

union cute_flt_assess {
	struct cute_flt       scal;
	struct cute_flt_range range;
	struct cute_flt_set   set;
};

union cute_str_assess {
	struct cute_str value;
};

union cute_ptr_assess {
	struct cute_ptr value;
};

union cute_mem_assess {
	struct cute_mem value;
};

struct cute_assess {
	const struct cute_assess_ops * ops;
	const char *                   file;
	int                            line;
	const char *                   func;
	union cute_assess_value        check;
	union {
		const char *           expr;
		union cute_sint_assess sint;
		union cute_uint_assess uint;
		union cute_flt_assess  flt;
		union cute_str_assess  str;
		union cute_ptr_assess  ptr;
		union cute_mem_assess  mem;
	}                              expect;
};

#define cute_assess_assert(_assess) \
	cute_assert(_assess); \
	cute_assess_assert_ops((_assess)->ops)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_intern(_assess) \
	cute_assess_assert(_assess)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_intern(_assess)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline bool
cute_assess_cmp_null(const struct cute_assess *      assess __cute_unused,
                     const union cute_assess_value * check __cute_unused)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(check);

	return true;
}

static inline void
cute_assess_release_null(struct cute_assess * assess __cute_unused)
{
	cute_assess_assert_intern(assess);
}

extern const struct cute_assess_ops cute_assess_null_ops;

extern void
cute_assess_build_null(struct cute_assess * assess);

extern void
cute_assess_build_expr(struct cute_assess * assess, const char * expr);

extern void
cute_assess_build_assert(struct cute_assess * assess, const char * expr);

extern void
cute_assess_build_excp(struct cute_assess * assess, int sig);

/******************************************************************************
 * Signed integer value handling
 ******************************************************************************/

struct cute_sint;

extern bool
cute_assess_sint_equal(struct cute_assess *     assess,
                       const struct cute_sint * check,
                       const struct cute_sint * expect);

extern bool
cute_assess_sint_unequal(struct cute_assess *     assess,
                         const struct cute_sint * check,
                         const struct cute_sint * expect);

extern bool
cute_assess_sint_greater(struct cute_assess *     assess,
                         const struct cute_sint * check,
                         const struct cute_sint * expect);

extern bool
cute_assess_sint_greater_equal(struct cute_assess *     assess,
                               const struct cute_sint * check,
                               const struct cute_sint * expect);
extern bool
cute_assess_sint_lower(struct cute_assess *     assess,
                       const struct cute_sint * check,
                       const struct cute_sint * expect);

extern bool
cute_assess_sint_lower_equal(struct cute_assess *     assess,
                             const struct cute_sint * check,
                             const struct cute_sint * expect);

/******************************************************************************
 * Signed integer range handling
 ******************************************************************************/

struct cute_sint_range;

extern bool
cute_assess_sint_in_range(struct cute_assess *           assess,
                          const struct cute_sint *       check,
                          const struct cute_sint_range * expect);

extern bool
cute_assess_sint_not_in_range(struct cute_assess *           assess,
                              const struct cute_sint *       check,
                              const struct cute_sint_range * expect);

/******************************************************************************
 * Signed integer set handling
 ******************************************************************************/

struct cute_sint_set;

extern bool
cute_assess_sint_in_set(struct cute_assess *         assess,
                        const struct cute_sint *     check,
                        const struct cute_sint_set * expect);

extern bool
cute_assess_sint_not_in_set(struct cute_assess *         assess,
                            const struct cute_sint *     check,
                            const struct cute_sint_set * expect);

/******************************************************************************
 * Unsigned integer value handling
 ******************************************************************************/

struct cute_uint;

extern bool
cute_assess_uint_equal(struct cute_assess *     assess,
                       const struct cute_uint * check,
                       const struct cute_uint * expect);

extern bool
cute_assess_uint_unequal(struct cute_assess *     assess,
                         const struct cute_uint * check,
                         const struct cute_uint * expect);

extern bool
cute_assess_uint_greater(struct cute_assess *     assess,
                         const struct cute_uint * check,
                         const struct cute_uint * expect);

extern bool
cute_assess_uint_greater_equal(struct cute_assess *     assess,
                               const struct cute_uint * check,
                               const struct cute_uint * expect);
extern bool
cute_assess_uint_lower(struct cute_assess *     assess,
                       const struct cute_uint * check,
                       const struct cute_uint * expect);

extern bool
cute_assess_uint_lower_equal(struct cute_assess *     assess,
                             const struct cute_uint * check,
                             const struct cute_uint * expect);

/******************************************************************************
 * Unsigned integer range handling
 ******************************************************************************/

struct cute_uint_range;

extern bool
cute_assess_uint_in_range(struct cute_assess *           assess,
                          const struct cute_uint *       check,
                          const struct cute_uint_range * expect);

extern bool
cute_assess_uint_not_in_range(struct cute_assess *           assess,
                              const struct cute_uint *       check,
                              const struct cute_uint_range * expect);

/******************************************************************************
 * Unsigned integer set handling
 ******************************************************************************/

struct cute_uint_set;

extern bool
cute_assess_uint_in_set(struct cute_assess *         assess,
                        const struct cute_uint *     check,
                        const struct cute_uint_set * expect);

extern bool
cute_assess_uint_not_in_set(struct cute_assess *         assess,
                            const struct cute_uint *     check,
                            const struct cute_uint_set * expect);

/******************************************************************************
 * Floating point value handling
 ******************************************************************************/

struct cute_flt;

extern bool
cute_assess_flt_equal(struct cute_assess *    assess,
                      const struct cute_flt * check,
                      const struct cute_flt * expect);

extern bool
cute_assess_flt_unequal(struct cute_assess *    assess,
                        const struct cute_flt * check,
                        const struct cute_flt * expect);

extern bool
cute_assess_flt_greater(struct cute_assess *    assess,
                        const struct cute_flt * check,
                        const struct cute_flt * expect);

extern bool
cute_assess_flt_greater_equal(struct cute_assess *    assess,
                              const struct cute_flt * check,
                              const struct cute_flt * expect);
extern bool
cute_assess_flt_lower(struct cute_assess *    assess,
                      const struct cute_flt * check,
                      const struct cute_flt * expect);

extern bool
cute_assess_flt_lower_equal(struct cute_assess *    assess,
                            const struct cute_flt * check,
                            const struct cute_flt * expect);

/******************************************************************************
 * Floating point number range handling
 ******************************************************************************/

struct cute_flt_range;

extern bool
cute_assess_flt_in_range(struct cute_assess *          assess,
                         const struct cute_flt *       check,
                         const struct cute_flt_range * expect);

extern bool
cute_assess_flt_not_in_range(struct cute_assess *          assess,
                             const struct cute_flt *       check,
                             const struct cute_flt_range * expect);

/******************************************************************************
 * Floating point number set handling
 ******************************************************************************/

struct cute_flt_set;

extern bool
cute_assess_flt_in_set(struct cute_assess *        assess,
                       const struct cute_flt *     check,
                       const struct cute_flt_set * expect);

extern bool
cute_assess_flt_not_in_set(struct cute_assess *        assess,
                           const struct cute_flt *     check,
                           const struct cute_flt_set * expect);

/******************************************************************************
 * Top-level generic assess handling
 ******************************************************************************/

static inline bool
cute_assess_has_source(const struct cute_assess * assess)
{
	cute_assert_intern(assess);

	return assess->file || (assess->line >= 0) || assess->func;
}

extern void
cute_assess_update_source(struct cute_assess * assess,
                          const char *         file,
                          int                  line,
                          const char *         function);

extern bool
cute_assess_check(struct cute_assess *            assess,
                  const union cute_assess_value * value);

extern struct cute_text_block *
cute_assess_desc(const struct cute_assess * assess);

extern void
cute_assess_release(struct cute_assess * assess);

#endif /* _CUTE_ASSESS_H */
