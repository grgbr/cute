/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Test assertion public interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      30 Jun 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _CUTE_CHECK_H
#define _CUTE_CHECK_H

#include <cute/priv/check.h>

/******************************************************************************
 * Assertion checking
 ******************************************************************************/

/**
 * Check that a test assertion is true.
 *
 * @param[in] _expr expression
 *
 * Abort current test and mark it as @rstsubst{failed} if @p _expr is false
 * (i.e., compares equal to zero).
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Example**
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_assert(0 == 0);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 */
#define cute_check_assert(_expr) \
	__cute_check_assert(!(_expr), __FILE__, __LINE__, __func__, # _expr)

/******************************************************************************
 * Signed integral numbers checking
 ******************************************************************************/

/**
 * Check a signed integer.
 *
 * @param[in] _chk  signed integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct expected signed integer value to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk and @p _xpct using the @p _op comparison operator results in a
 * failure. Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal`,
 * - `unequal`,
 * - `greater`,
 * - `greater_equal`,
 * - `lower`,
 * - `lower_equal`.
 *
 * Both @p _chk and @p _xpct *MUST* be signed integers, i.e., either :
 * - `signed char`,
 * - `signed short`,
 * - `signed int`,
 * - `signed long`,
 * - `signed long long`,
 * - or equivalent *typedef*'ed types.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 equals 0** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, equal, 0);
 * }
 * @endcode
 *
 * **Ensure that 0 unequals 1** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, unequal, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than 0** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(1, greater, 0);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than or equal to 1** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(1, greater_equal, 1);
 * }
 * @endcode
 *
 * **Ensure that 0 is lower than 1** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, lower, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is lower than or equal to 1** :
 * @code
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(1, lower_equal, 1);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 */
#define cute_check_sint(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(sint, # _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_sint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(sint, # _chk, _chk, _op, _xpct)

#define cute_check_sint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(sint, # _chk, _chk, _op, _xpct)

extern void
cute_check_sint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_sint *       check,
                         const struct cute_sint_range * expect) __cute_export;

extern void
cute_check_sint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_sint *       check,
                             const struct cute_sint_range * expect)
	__cute_export;

extern void
cute_check_sint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_sint *     check,
                       const struct cute_sint_set * expect) __cute_export;

extern void
cute_check_sint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_sint *     check,
                           const struct cute_sint_set * expect) __cute_export;

/******************************************************************************
 * Unsigned integral numbers checking
 ******************************************************************************/

#define cute_check_uint(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(uint, # _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_uint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(uint, # _chk, _chk, _op, _xpct)

#define cute_check_uint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(uint, # _chk, _chk, _op, _xpct)

extern void
cute_check_uint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * check,
                              const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check,
                            const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_uint *       check,
                         const struct cute_uint_range * expect) __cute_export;

extern void
cute_check_uint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_uint *       check,
                             const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_check_uint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_uint *     check,
                       const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_uint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_uint *     check,
                           const struct cute_uint_set * expect) __cute_export;

/******************************************************************************
 * Unsigned integral hexadecimal numbers checking
 ******************************************************************************/

#define cute_check_hex(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX(# _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_hex_range(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX_RANGE(# _chk, _chk, _op, _xpct)

#define cute_check_hex_set(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX_SET(# _chk, _chk, _op, _xpct)

extern void
cute_check_hex_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_uint * check,
                     const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_unequal(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_uint * check,
                       const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_greater(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_uint * check,
                       const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_greater_equal(const char *             file,
                             int                      line,
                             const char *             function,
                             const struct cute_uint * check,
                             const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_lower(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_uint * check,
                     const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_lower_equal(const char *             file,
                           int                      line,
                           const char *             function,
                           const struct cute_uint * check,
                           const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_in_range(const char *                   file,
                        int                            line,
                        const char *                   function,
                        const struct cute_uint *       check,
                        const struct cute_uint_range * expect) __cute_export;

extern void
cute_check_hex_not_in_range(const char *                   file,
                            int                            line,
                            const char *                   function,
                            const struct cute_uint *       check,
                            const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_check_hex_in_set(const char *                 file,
                      int                          line,
                      const char *                 function,
                      const struct cute_uint *     check,
                      const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_hex_not_in_set(const char *                 file,
                          int                          line,
                          const char *                 function,
                          const struct cute_uint *     check,
                          const struct cute_uint_set * expect) __cute_export;

/******************************************************************************
 * Floating point numbers checking
 ******************************************************************************/

#define cute_check_flt(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(flt, # _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_flt_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(flt, # _chk, _chk, _op, _xpct)

#define cute_check_flt_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(flt, # _chk, _chk, _op, _xpct)

extern void
cute_check_flt_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_unequal(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_greater(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_greater_equal(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * check,
                             const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_lower(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_lower_equal(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check,
                           const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_in_range(const char *                  file,
                        int                           line,
                        const char *                  function,
                        const struct cute_flt *       check,
                        const struct cute_flt_range * expect) __cute_export;

extern void
cute_check_flt_not_in_range(const char *                  file,
                            int                           line,
                            const char *                  function,
                            const struct cute_flt *       check,
                            const struct cute_flt_range * expect)
	__cute_export;

extern void
cute_check_flt_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_flt *     check,
                      const struct cute_flt_set * expect) __cute_export;

extern void
cute_check_flt_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_flt *     check,
                          const struct cute_flt_set * expect) __cute_export;

/******************************************************************************
 * Strings checking
 ******************************************************************************/

#define cute_check_str(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(str, # _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_str_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(str, # _chk, _chk, _op, _xpct)

extern void
cute_check_str_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_str * check,
                     const struct cute_str * expect) __cute_export;

extern void
cute_check_str_unequal(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_begin(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_str * check,
                     const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_begin(const char *            file,
                         int                     line,
                         const char *            function,
                         const struct cute_str * check,
                         const struct cute_str * expect) __cute_export;

extern void
cute_check_str_end(const char *            file,
                   int                     line,
                   const char *            function,
                   const struct cute_str * check,
                   const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_end(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_contain(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_contain(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_str * check,
                           const struct cute_str * expect) __cute_export;

extern void
cute_check_str_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_str *     check,
                      const struct cute_str_set * expect) __cute_export;

extern void
cute_check_str_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_str *     check,
                          const struct cute_str_set * expect) __cute_export;

/******************************************************************************
 * Pointer checking
 ******************************************************************************/

#define cute_check_ptr(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(ptr, # _chk, _chk, _op, # _xpct, _xpct)

#define cute_check_ptr_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(ptr, # _chk, _chk, _op, _xpct)

#define cute_check_ptr_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(ptr, # _chk, _chk, _op, _xpct)

extern void
cute_check_ptr_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_ptr *  check,
                     const struct cute_ptr *  expect) __cute_export;

extern void
cute_check_ptr_unequal(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_ptr *  check,
                       const struct cute_ptr *  expect) __cute_export;

extern void
cute_check_ptr_greater(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_ptr * check,
                       const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_greater_equal(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_ptr * check,
                             const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_lower(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_ptr * check,
                     const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_lower_equal(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_ptr * check,
                           const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_in_range(const char *                  file,
                        int                           line,
                        const char *                  function,
                        const struct cute_ptr *       check,
                        const struct cute_ptr_range * expect) __cute_export;

extern void
cute_check_ptr_not_in_range(const char *                  file,
                            int                           line,
                            const char *                  function,
                            const struct cute_ptr *       check,
                            const struct cute_ptr_range * expect)
	__cute_export;

extern void
cute_check_ptr_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_ptr *     check,
                      const struct cute_ptr_set * expect) __cute_export;

extern void
cute_check_ptr_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_ptr *     check,
                          const struct cute_ptr_set * expect) __cute_export;

/******************************************************************************
 * Memory area checking
 ******************************************************************************/

#define cute_check_mem(_chk, _op, _xpct, _sz) \
	cute_check_mem_ ## _op(__FILE__, \
	                       __LINE__, \
	                       __func__, \
	                       &__CUTE_VALUE(ptr, # _chk, _chk), \
	                       &__CUTE_MEM("{@ " # _xpct ":" # _sz "}", \
	                                   _xpct, \
	                                   _sz))


extern void
cute_check_mem_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_ptr *  check,
                     const struct cute_mem *  expect) __cute_export;

extern void
cute_check_mem_unequal(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_ptr *  check,
                       const struct cute_mem *  expect) __cute_export;

#endif /* _CUTE_CHECK_H */
