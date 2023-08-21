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
 * @code{.c}
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
 * - `equal` to ensure that @p _chk == @p _xpct ;
 * - `unequal` to ensure that @p _chk != @p _xpct ;
 * - `greater`, to ensure that @p _chk > @p _xpct ;
 * - `greater_equal`, to ensure that @p _chk >= @p _xpct ;
 * - `lower`, to ensure that @p _chk < @p _xpct ;
 * - `lower_equal`, to ensure that @p _chk <= @p _xpct.
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
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, equal, 0);
 * }
 * @endcode
 *
 * **Ensure that 0 unequals 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, unequal, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than 0** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(1, greater, 0);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than or equal to 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(1, greater_equal, 1);
 * }
 * @endcode
 *
 * **Ensure that 0 is lower than 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint(0, lower, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is lower than or equal to 1** :
 * @code{.c}
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

/**
 * Check a signed integer against a range.
 *
 * @param[in] _chk  signed integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference signed integer range to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct range using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _chk <= @p _xpct.max ;
 * - `not_in` to ensure that @p _chk < @p _xpct.min or @p _chk > @p _xpct.max.
 *
 * @p _chk *MUST* be a signed integer, i.e., either :
 * - `signed char`,
 * - `signed short`,
 * - `signed int`,
 * - `signed long`,
 * - `signed long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_sint_range signed integer range as defined by the
 * #CUTE_SINT_RANGE macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 is included withint the [-10, 10] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint_range(0, in, CUTE_SINT_RANGE(-10, 10));
 * }
 * @endcode
 *
 * **Ensure that -10 is not included within the [0, 10] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_sint_range range = CUTE_SINT_RANGE(0, 10);
 *
 *      cute_check_sint_range(-10, not_in, range);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_SINT_RANGE
 * - cute_sint_range
 * - #CUTE_TEST
 */
#define cute_check_sint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(sint, # _chk, _chk, _op, _xpct)

/**
 * Check a signed integer against a set.
 *
 * @param[in] _chk  signed integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference set of signed integers to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct set using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _chk equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _chk equals to none of the @p _xpct set values.
 *
 * @p _chk *MUST* be a signed integer, i.e., either :
 * - `signed char`,
 * - `signed short`,
 * - `signed int`,
 * - `signed long`,
 * - `signed long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_sint_set signed integer set as defined by
 * the #CUTE_SINT_SET macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 is included withint the {-10, -5, 0, 5, 10} set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_sint_set(0, in, CUTE_SINT_SET(-10, -5, 0, 5, 10));
 * }
 * @endcode
 *
 * **Ensure that 1 is not included within the {-5, 0, 5} set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_sint_set set = CUTE_SINT_SET(-5, 0, 5);
 *
 *      cute_check_sint_set(1, not_in, set);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_SINT_SET
 * - cute_sint_set
 * - #CUTE_TEST
 */
#define cute_check_sint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(sint, # _chk, _chk, _op, _xpct)

/******************************************************************************
 * Unsigned integral numbers checking
 ******************************************************************************/

/**
 * Check an unsigned integer.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct expected unsigned integer value to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk and @p _xpct using the @p _op comparison operator results in a
 * failure. Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _chk == @p _xpct ;
 * - `unequal` to ensure that @p _chk != @p _xpct ;
 * - `greater`, to ensure that @p _chk > @p _xpct ;
 * - `greater_equal`, to ensure that @p _chk >= @p _xpct ;
 * - `lower`, to ensure that @p _chk < @p _xpct ;
 * - `lower_equal`, to ensure that @p _chk <= @p _xpct.
 *
 * Both @p _chk and @p _xpct *MUST* be unsigned integers, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 equals 0** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(0, equal, 0);
 * }
 * @endcode
 *
 * **Ensure that 0 unequals 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(0, unequal, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than 0** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(1, greater, 0);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than or equal to 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(1, greater_equal, 1);
 * }
 * @endcode
 *
 * **Ensure that 0 is lower than 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(0, lower, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is lower than or equal to 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint(1, lower_equal, 1);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 */
#define cute_check_uint(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(uint, # _chk, _chk, _op, # _xpct, _xpct)

/**
 * Check a unsigned integer against a range.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference unsigned integer range to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct range using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _chk <= @p _xpct.max ;
 * - `not_in` to ensure that @p _chk < @p _xpct.min or @p _chk > @p _xpct.max.
 *
 * @p _chk *MUST* be a unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_range unsigned integer range as defined by
 * the #CUTE_UINT_RANGE macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 is included withint the [0, 10] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint_range(0, in, CUTE_UINT_RANGE(0, 10));
 * }
 * @endcode
 *
 * **Ensure that 11 is not included within the [0, 10] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_range range = CUTE_UINT_RANGE(0, 10);
 *
 *      cute_check_uint_range(11, not_in, range);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_UINT_RANGE
 * - cute_uint_range
 * - #CUTE_TEST
 */
#define cute_check_uint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(uint, # _chk, _chk, _op, _xpct)

/**
 * Check a unsigned integer against a set.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference set of unsigned integers to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct set using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _chk equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _chk equals to none of the @p _xpct set values.
 *
 * @p _chk *MUST* be a unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_set unsigned integer set as defined by
 * the #CUTE_UINT_SET macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 is included withint the {0, 1, 2, 3, 5, 8} set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_uint_set(0, in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
 * }
 * @endcode
 *
 * **Ensure that 4 is not included within the {0, 1, 2, 3, 5, 8} set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_set set = CUTE_UINT_SET(0, 1, 2, 3, 5, 8);
 *
 *      cute_check_uint_set(4, not_in, set);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_UINT_SET
 * - cute_uint_set
 * - #CUTE_TEST
 */
#define cute_check_uint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(uint, # _chk, _chk, _op, _xpct)

/******************************************************************************
 * Unsigned integral hexadecimal numbers checking
 ******************************************************************************/

/**
 * Check an hexadecimal unsigned integer.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct expected unsigned integer value to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk and @p _xpct using the @p _op comparison operator results in a
 * failure. It is similar to the #cute_check_uint macro with the ability to
 * **display values as hexadecimal numbers** when producing failure
 * @rstsubst{report}.
 *
 * Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _chk == @p _xpct ;
 * - `unequal` to ensure that @p _chk != @p _xpct ;
 * - `greater`, to ensure that @p _chk > @p _xpct ;
 * - `greater_equal`, to ensure that @p _chk >= @p _xpct ;
 * - `lower`, to ensure that @p _chk < @p _xpct ;
 * - `lower_equal`, to ensure that @p _chk <= @p _xpct.
 *
 * Both @p _chk and @p _xpct *MUST* be unsigned integers, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0 equals 0** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(0, equal, 0);
 * }
 * @endcode
 *
 * **Ensure that 0 unequals 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(0, unequal, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than 0** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(1, greater, 0);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than or equal to 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(1, greater_equal, 1);
 * }
 * @endcode
 *
 * **Ensure that 0 is lower than 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(0, lower, 1);
 * }
 * @endcode
 *
 * **Ensure that 1 is lower than or equal to 1** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex(1, lower_equal, 1);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 * - #cute_check_uint
 */
#define cute_check_hex(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX(# _chk, _chk, _op, # _xpct, _xpct)

/**
 * Check an hexadecimal unsigned integer against a range.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference unsigned integer range to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct range using the @p _op comparison operator
 * results in a failure. It is similar to the #cute_check_uint_range macro with
 * the ability to **display values as hexadecimal numbers** when producing
 * failure @rstsubst{report}.
 *
 * Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _chk <= @p _xpct.max ;
 * - `not_in` to ensure that @p _chk < @p _xpct.min or @p _chk > @p _xpct.max.
 *
 * @p _chk *MUST* be a unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_range unsigned integer range as defined by
 * the #CUTE_UINT_RANGE macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0xf is included withint the [0x0, 0xff] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex_range(0xf, in, CUTE_UINT_RANGE(0x0, 0xff));
 * }
 * @endcode
 *
 * **Ensure that 0xffff is not included within the [0x0, 0xff] range** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_range range = CUTE_UINT_RANGE(0x0, 0xff);
 *
 *      cute_check_hex_range(0xffff, not_in, range);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_UINT_RANGE
 * - cute_uint_range
 * - #cute_check_uint_range
 * - #CUTE_TEST
 */
#define cute_check_hex_range(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX_RANGE(# _chk, _chk, _op, _xpct)

/**
 * Check an hexadecimal unsigned integer against a set.
 *
 * @param[in] _chk  unsigned integer value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference set of unsigned integers to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct set using the @p _op comparison operator
 * results in a failure. It is similar to the #cute_check_uint_set macro with
 * the ability to **display values as hexadecimal numbers** when producing
 * failure @rstsubst{report}.
 *
 * Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _chk equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _chk equals to none of the @p _xpct set values.
 *
 * @p _chk *MUST* be a unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_set unsigned integer set as defined by
 * the #CUTE_UINT_SET macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0xbeef is included withint the {0xdead, 0xbeef, 0xbabe, 0xcode}
 * set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_hex_set(0,
 *                         in,
 *                         CUTE_UINT_SET(0xdead, 0xbeef, 0xbabe, 0xcode));
 * }
 * @endcode
 *
 * **Ensure that 0xf is not included within the {0xdead, 0xbeef, 0xbabe, 0xcode}
 * set** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_set set = CUTE_UINT_SET(0xdead,
 *                                                     0xbeef,
 *                                                     0xbabe,
 *                                                     0xcode);
 *
 *      cute_check_hex_set(0xf, not_in, set);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_UINT_SET
 * - cute_uint_set
 * - #cute_check_uint_set
 * - #CUTE_TEST
 */
#define cute_check_hex_set(_chk, _op, _xpct) \
	__CUTE_CHECK_HEX_SET(# _chk, _chk, _op, _xpct)

/******************************************************************************
 * Floating point numbers checking
 ******************************************************************************/

/**
 * Check a floating point number.
 *
 * @param[in] _chk  floating point value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct expected floating point value to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk and @p _xpct using the @p _op comparison operator results in a
 * failure. Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _chk == @p _xpct ;
 * - `unequal` to ensure that @p _chk != @p _xpct ;
 * - `greater`, to ensure that @p _chk > @p _xpct ;
 * - `greater_equal`, to ensure that @p _chk >= @p _xpct ;
 * - `lower`, to ensure that @p _chk < @p _xpct ;
 * - `lower_equal`, to ensure that @p _chk <= @p _xpct.
 *
 * Both @p _chk and @p _xpct *MUST* be floating point numbers, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or equivalent *typedef*'ed types.
 *
 * @note
 * Quad-precision floating point numbers (`__float128`) are not yet supported.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0.5 equals 0.5 (single precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(0.5f, equal, 0.5f);
 * }
 * @endcode
 *
 * **Ensure that 0.5 unequals 1.5 (single precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(0.5f, unequal, 1.5f);
 * }
 * @endcode
 *
 * **Ensure that 1.5 is greater than 0.5 (double precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(1.5, greater, 0.5);
 * }
 * @endcode
 *
 * **Ensure that 1 is greater than or equal to 1 (double precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(1.5, greater_equal, 1.5);
 * }
 * @endcode
 *
 * **Ensure that 0.5 is lower than 1.5 (extended precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(0.5L, lower, 1.5L);
 * }
 * @endcode
 *
 * **Ensure that 1.5 is lower than or equal to 1.5 (extended precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt(1.5L, lower_equal, 1.5L);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 */
#define cute_check_flt(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(flt, # _chk, _chk, _op, # _xpct, _xpct)

/**
 * Check a floating point number against a range.
 *
 * @param[in] _chk  floating point value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference floating point range to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct range using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _chk <= @p _xpct.max ;
 * - `not_in` to ensure that @p _chk < @p _xpct.min or @p _chk > @p _xpct.max.
 *
 * @p _chk *MUST* be a floating point number, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_flt_range floating point number range as defined by
 * the #CUTE_FLT_RANGE macro.
 *
 * @note
 * Quad-precision floating point numbers (`__float128`) are not yet supported.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0.5 is included withint the [-10.5, 10.5] range
 * (single precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt_range(0.5f, in, CUTE_FLT_RANGE(-10.5f, 10.5f));
 * }
 * @endcode
 *
 * **Ensure that -10 is not included within the [0.5, 10.5] range
 * (double precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_range range = CUTE_FLT_RANGE(0.5, 10.5);
 *
 *      cute_check_flt_range(-10, not_in, range);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_FLT_RANGE
 * - cute_flt_range
 * - #CUTE_TEST
 */
#define cute_check_flt_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(flt, # _chk, _chk, _op, _xpct)

/**
 * Check a floating point number against a set.
 *
 * @param[in] _chk  floating point value to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference set of floating point numbers to perform the check
 *                  against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct set using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _chk equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _chk equals to none of the @p _xpct set values.
 *
 * @p _chk *MUST* be a floating point number, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or equivalent *typedef*'ed types.
 *
 * @note
 * Quad-precision floating point numbers (`__float128`) are not yet supported.
 *
 * @p _xpct *MUST* be a cute_flt_set floating point number set as defined by
 * the #CUTE_FLT_SET macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that 0.5 is included withint the {0.5, 1.5, 2.0, 3.5} set
 * (single precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_flt_set(0.5f, in, CUTE_FLT_SET(0.5f, 1.5f, 2.0f, 3.5f));
 * }
 * @endcode
 *
 * **Ensure that 4.5 is not included within the {0.5, 1.5, 2.0, 3.5, 5.5} set
 * (double precision)** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_set set = CUTE_FLT_SET(0.5, 1.5, 2.0, 3.5, 5.5);
 *
 *      cute_check_flt_set(4.5, not_in, set);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_FLT_SET
 * - cute_flt_set
 * - #CUTE_TEST
 */
#define cute_check_flt_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(flt, # _chk, _chk, _op, _xpct)

/******************************************************************************
 * Strings checking
 ******************************************************************************/

/**
 * Check string content.
 *
 * @param[in] _chk  string content to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct expected string content to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk and @p _xpct using the @p _op comparison operator results in a
 * failure. Comparison is performed according to the following formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _chk content equals to @p _xpct content;
 * - `unequal` to ensure that @p _chk content differs from @p _xpct content;
 * - `begin`, to ensure that @p _chk content begins with @p _xpct content;
 * - `end`, to ensure that @p _chk content ends with @p _xpct content;
 * - `contain`, to ensure that @p _chk content contains @p _xpct content, i.e.,
 *   @p _xpct is a substring of @p _chk;
 * - `not_contain`, to ensure that @p _chk content does not containt @p _xpct
 *   content, i.e., @p _xpct is not a substring of @p _chk.
 *
 * Both @p _chk and @p _xpct *MUST* be `NULL` terminated C strings.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that "zero" string equals to "zero" string** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str("zero", equal, "zero");
 * }
 * @endcode
 *
 * **Ensure that "zero" string begins with "ze" string** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str("zero", begin, "ze");
 * }
 * @endcode
 *
 * **Ensure that "zero" string ends with "ro" string** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str("zero", end, "ro");
 * }
 * @endcode
 *
 * **Ensure that "er" is a substring of "zero" string** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str("zero", contain, "er");
 * }
 * @endcode
 *
 * **Ensure that "era" is not a substring of "zero" string** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str("zero", not_contain, "era");
 * }
 * @endcode
 *
 * @see
 * - #CUTE_TEST
 */
#define cute_check_str(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(str, # _chk, _chk, _op, # _xpct, _xpct)

/**
 * Check string content against a set of strings.
 *
 * @param[in] _chk  string content to check
 * @param[in] _op   constraint operation used to perform the check
 * @param[in] _xpct reference set of strings to perform the check against
 *
 * Abort current test and mark it as @rstsubst{failed} if the comparison of
 * @p _chk against the @p _xpct set using the @p _op comparison operator
 * results in a failure. Comparison is performed according to the following
 * formula :
 *
 *     _chk <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _chk equals to one the @p _xpct set strings ;
 * - `not_in` to ensure that @p _chk equals to none of the @p _xpct set strings.
 *
 * @p _chk *MUST* be a `NULL` terminated C string.
 *
 * @p _xpct *MUST* be a cute_str_set set of `NULL` terminated C strings as
 * defined by the #CUTE_STR_SET macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Ensure that "one" string is included withint the {"zero", "one", "two"} set
 * of strings** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      cute_check_str_set("one", in, CUTE_STR_SET("zero", "one", "two"));
 * }
 * @endcode
 *
 * **Ensure that "three" string is not included withint the
 * {"zero", "one", "two"} set of strings** :
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_str_set set = CUTE_STR_SET("zero", "one", "two");
 *
 *      cute_check_str_set("three", not_in, set);
 * }
 * @endcode
 *
 * @see
 * - #CUTE_STR_SET
 * - cute_str_set
 * - #CUTE_TEST
 */
#define cute_check_str_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(str, # _chk, _chk, _op, _xpct)

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
