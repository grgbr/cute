/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Mock expectation assertion public interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      23 Jun 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _CUTE_EXPECT_H
#define _CUTE_EXPECT_H

#include <cute/priv/expect.h>

/******************************************************************************
 * Assertion mock expectation handling
 ******************************************************************************/

/**
 * Check that an assertion failure has been thrown.
 *
 * @param[in] _call function call expression
 *
 * Abort current test and mark it as @rstsubst{failed} if @p _call expression
 * does not call cute_mock_assert() to ensure that an assertion failure has been
 * thrown.
 *
 * As shown in the example below, the @rstlnk{glibc}'s `__assert_fail()`
 * function is overridden using cute_mock_assert() so that a call to
 * cute_expect_assertion() may check that `function_to_test()` function causes
 * an @man{assert(3)} failure.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Example**
 * @code{.c}
 * #include <assert.h>
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 *
 * void  __attribute__((noreturn))
 * __assert_fail(const char * expression,
 *               const char * file,
 *               unsigned int line,
 *               const char * function)
 * {
 *      cute_mock_assert(expression, file, line, function);
 * }
 *
 * static void
 * function_to_test(void)
 * {
 *      assert(0);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_assertion(function_to_test());
 * }
 * @endcode
 *
 * @see
 * - cute_mock_assert()
 * - #CUTE_TEST
 */
#define cute_expect_assertion(_call) \
	{ \
		cute_expect_assert = true; \
		if (!sigsetjmp(cute_expect_assert_env, 1)) { \
			_call; \
			cute_expect_fail_assert(__FILE__, __LINE__, __func__); \
		} \
	}

/**
 * Catch an assertion failure.
 *
 * @param[in] expression assertion expression
 * @param[in] file       assertion source file pathname
 * @param[in] line       assertion source file line
 * @param[in] function   assertion function calling function name
 *
 * Use this in combination with cute_expect_assertion() to test assertion
 * failures.
 *
 * @see
 * - cute_expect_assertion()
 * - #CUTE_TEST
 */
extern void
cute_mock_assert(const char * expression,
                 const char * file,
                 unsigned int line,
                 const char * function)
	__cute_noreturn __cute_export;

/******************************************************************************
 * Mock call expectation handling
 ******************************************************************************/

/**
 * Schedule a function call mock expectation.
 *
 * @param[in] _func function name
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_call() is called.
 * The check performed ensures that the function which name is given as @p _func
 * argument is called according to the expected schedule order.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 *
 * static void
 * callee(void)
 * {
 *      cute_mock_call();
 * }
 *
 * static void
 * caller(void)
 * {
 *      callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_call(callee);
 *      caller();
 * }
 * @endcode
 *
 * @see
 * - cute_mock_call()
 * - #CUTE_TEST
 */
#define cute_expect_call(_func) \
	cute_expect_sched_call(__FILE__, __LINE__, # _func)

/**
 * Check a function call mock expectation.
 *
 * Ensure that a function call mock @rstsubst{expectation} scheduled by
 * cute_expect_call() is called according to the expected schedule order.
 *
 * Current test is aborted and marked as @rstsubst{failed} when the check fails.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * @see
 * - cute_expect_call()
 * - #CUTE_TEST
 */
#define cute_mock_call() \
	cute_expect_check_call(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Signed integer mock parameter expectation handling
 ******************************************************************************/

/**
 * Schedule a signed integer function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected signed integer value to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_sint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed} if
 * the comparison of @p _parm against @p _xpct using the @p _op comparison
 * operator results in a failure. Comparison is performed according to the
 * following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _parm == @p _xpct ;
 * - `unequal` to ensure that @p _parm != @p _xpct ;
 * - `greater`, to ensure that @p _parm > @p _xpct ;
 * - `greater_equal`, to ensure that @p _parm >= @p _xpct ;
 * - `lower`, to ensure that @p _parm < @p _xpct ;
 * - `lower_equal`, to ensure that @p _parm <= @p _xpct.
 *
 * Both @p _parm and @p _xpct *MUST* be signed integers, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_sint_parm(word);
 *      cute_mock_sint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_sint_parm(callee, word, equal,   INT_MAX);
 *      cute_expect_sint_parm(callee, half, greater, SHRT_MAX / 2);
 *      caller(INT_MAX, SHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_sint_parm()
 * - #CUTE_TEST
 */
#define cute_expect_sint_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op(__FILE__, \
	                                     __LINE__, \
	                                     # _func, \
	                                     # _parm, \
	                                     &__CUTE_VALUE(sint, \
	                                                   # _xpct, \
	                                                   _xpct))

/**
 * Check a signed integer function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the value of a signed integer function parameter expectation scheduled
 * using cute_expect_sint_parm(), cute_expect_sint_range() or
 * cute_expect_sint_set().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_sint_parm()
 * - cute_expect_sint_range()
 * - cute_expect_sint_set()
 * - #CUTE_TEST
 */
#define cute_mock_sint_parm(_parm) \
	cute_expect_check_sint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(sint, # _parm, _parm))

/**
 * Schedule a signed integer range function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference signed integer range to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_sint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct range using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _parm <= @p _xpct.max ;
 * - `not_in` to ensure that @p _parm < @p _xpct.min or @p _parm > @p _xpct.max.
 *
 * @p _parm *MUST* be a signed integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_sint_parm(word);
 *      cute_mock_sint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_sint_range range = CUTE_SINT_RANGE(0, SHRT_MAX);
 *
 *      cute_expect_sint_range(callee, word, in,     CUTE_SINT_RANGE(-10, 10));
 *      cute_expect_sint_range(callee, half, not_in, range);
 *      caller(-5, SHRT_MIN);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_sint_parm()
 * - #CUTE_SINT_RANGE
 * - cute_sint_range
 * - #CUTE_TEST
 */
#define cute_expect_sint_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op ## _range(__FILE__, \
	                                               __LINE__, \
	                                               # _func, \
	                                               # _parm, \
	                                               &(_xpct))

/**
 * Schedule a signed integer set function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference signed integer set to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_sint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct set using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _parm equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _parm equals to none of the @p _xpct set values.
 *
 * @p _parm *MUST* be a signed integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_sint_parm(word);
 *      cute_mock_sint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_sint_set set = CUTE_SINT_SET(0, 10, SHRT_MAX);
 *
 *      cute_expect_sint_set(callee, word, in,     CUTE_SINT_SET(-10, 0, 10));
 *      cute_expect_sint_set(callee, half, not_in, set);
 *      caller(10, SHRT_MIN);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_sint_parm()
 * - #CUTE_SINT_SET
 * - cute_sint_set
 * - #CUTE_TEST
 */
#define cute_expect_sint_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op ## _set(__FILE__, \
	                                             __LINE__, \
	                                             # _func, \
	                                             # _parm, \
	                                             &(_xpct))

/******************************************************************************
 * Signed integer return value expectation handling
 ******************************************************************************/

/**
 * Schedule a signed integer function return value mock expectation.
 *
 * @param[in] _func   function name
 * @param[in] _retval value to be returned from mocked function
 *
 * Define and schedule an @rstsubst{expectation} which value is returned when
 * cute_mock_sint_retval() is called.
 *
 * @p _retval *MUST* be a signed integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static int
 * callee(void)
 * {
 *      return (int)cute_mock_sint_retval();
 * }
 *
 * static int
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_sint_retval(callee, 1);
 *      cute_check_sint(caller(), equal, 1);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_sint_retval()
 * - #CUTE_TEST
 */
#define cute_expect_sint_retval(_func, _retval) \
	cute_expect_sched_sint_retval(__FILE__, \
	                              __LINE__, \
	                              # _func, \
	                              &__CUTE_VALUE(sint, # _retval, _retval))

/**
 * Return a signed integer function return value mock expectation.
 *
 * @return scheduled return value cast as an `intmax_t`
 *
 * Extract and return a a signed integer value scheduled using
 * cute_expect_sint_retval().
 *
 * This macro must be called from within the mocked function that returns the
 * scheduled value.
 *
 * @see
 * - cute_expect_sint_retval()
 * - #CUTE_TEST
 */
#define cute_mock_sint_retval() \
	cute_expect_check_sint_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Unsigned integer mock parameter expectation handling
 ******************************************************************************/

/**
 * Schedule an unsigned integer function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected unsigned integer value to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_uint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed} if
 * the comparison of @p _parm against @p _xpct using the @p _op comparison
 * operator results in a failure. Comparison is performed according to the
 * following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _parm == @p _xpct ;
 * - `unequal` to ensure that @p _parm != @p _xpct ;
 * - `greater`, to ensure that @p _parm > @p _xpct ;
 * - `greater_equal`, to ensure that @p _parm >= @p _xpct ;
 * - `lower`, to ensure that @p _parm < @p _xpct ;
 * - `lower_equal`, to ensure that @p _parm <= @p _xpct.
 *
 * Both @p _parm and @p _xpct *MUST* be unsigned integers, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_uint_parm(word);
 *      cute_mock_uint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_uint_parm(callee, word, equal,   UINT_MAX);
 *      cute_expect_uint_parm(callee, half, greater, USHRT_MAX / 2);
 *      caller(UINT_MAX, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_uint_parm()
 * - #CUTE_TEST
 */
#define cute_expect_uint_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op(__FILE__, \
	                                     __LINE__, \
	                                     # _func, \
	                                     # _parm, \
	                                     &__CUTE_VALUE(uint, \
	                                                   # _xpct, \
	                                                   _xpct))

/**
 * Check an unsigned integer function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the value of an unsigned integer function parameter expectation
 * scheduled using cute_expect_uint_parm(), cute_expect_uint_range() or
 * cute_expect_uint_set().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_uint_parm()
 * - cute_expect_uint_range()
 * - cute_expect_uint_set()
 * - #CUTE_TEST
 */
#define cute_mock_uint_parm(_parm) \
	cute_expect_check_uint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(uint, # _parm, _parm))

/**
 * Schedule an unsigned integer range function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference unsigned integer range to perform the check
 *                  against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_uint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct range using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _parm <= @p _xpct.max ;
 * - `not_in` to ensure that @p _parm < @p _xpct.min or @p _parm > @p _xpct.max.
 *
 * @p _parm *MUST* be an unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_range unsigned integer range as defined by the
 * #CUTE_UINT_RANGE macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_uint_parm(word);
 *      cute_mock_uint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_range range = CUTE_UINT_RANGE(0, USHRT_MAX / 2);
 *
 *      cute_expect_uint_range(callee, word, in,     CUTE_UINT_RANGE(0, 10));
 *      cute_expect_uint_range(callee, half, not_in, range);
 *      caller(5, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_uint_parm()
 * - #CUTE_UINT_RANGE
 * - cute_uint_range
 * - #CUTE_TEST
 */
#define cute_expect_uint_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op ## _range(__FILE__, \
	                                               __LINE__, \
	                                               # _func, \
	                                               # _parm, \
	                                               &(_xpct))

/**
 * Schedule an unsigned integer set function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference unsigned integer set to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_uint_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct set using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _parm equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _parm equals to none of the @p _xpct set values.
 *
 * @p _parm *MUST* be an unsigned integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_uint_parm(word);
 *      cute_mock_uint_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_set set = CUTE_UINT_SET(0, 10, USHRT_MAX / 2);
 *
 *      cute_expect_uint_set(callee, word, in,     CUTE_UINT_SET(0, 5, 10));
 *      cute_expect_uint_set(callee, half, not_in, set);
 *      caller(10, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_uint_parm()
 * - #CUTE_UINT_SET
 * - cute_uint_set
 * - #CUTE_TEST
 */
#define cute_expect_uint_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op ## _set(__FILE__, \
	                                             __LINE__, \
	                                             # _func, \
	                                             # _parm, \
	                                             &(_xpct))

/******************************************************************************
 * Unsigned integer return value expectation handling
 ******************************************************************************/

/**
 * Schedule an unsigned integer function return value mock expectation.
 *
 * @param[in] _func   function name
 * @param[in] _retval value to be returned from mocked function
 *
 * Define and schedule an @rstsubst{expectation} which value is returned when
 * cute_mock_uint_retval() is called.
 *
 * @p _retval *MUST* be an unsigned integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static int
 * callee(void)
 * {
 *      return (int)cute_mock_uint_retval();
 * }
 *
 * static int
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_uint_retval(callee, 1);
 *      cute_check_uint(caller(), equal, 1);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_uint_retval()
 * - #CUTE_TEST
 */
#define cute_expect_uint_retval(_func, _retval) \
	cute_expect_sched_uint_retval(__FILE__, \
	                              __LINE__, \
	                              # _func, \
	                              &__CUTE_VALUE(uint, # _retval, _retval))

/**
 * Return an unsigned integer function return value mock expectation.
 *
 * @return scheduled return value cast as an `uintmax_t`
 *
 * Extract and return an unsigned integer value scheduled using
 * cute_expect_uint_retval().
 *
 * This macro must be called from within the mocked function that returns the
 * scheduled value.
 *
 * @see
 * - cute_expect_uint_retval()
 * - #CUTE_TEST
 */
#define cute_mock_uint_retval() \
	cute_expect_check_uint_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Unsigned hexadecimal integer mock parameter expectation handling
 ******************************************************************************/

/**
 * Schedule an hexadecimal unsigned integer function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected unsigned integer value to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_hex_parm() is called.
 * It is similar to the #cute_expect_uint_parm macro with the ability to
 * **display values as hexadecimal numbers** when producing failure
 * @rstsubst{report}.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed} if
 * the comparison of @p _parm against @p _xpct using the @p _op comparison
 * operator results in a failure. Comparison is performed according to the
 * following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _parm == @p _xpct ;
 * - `unequal` to ensure that @p _parm != @p _xpct ;
 * - `greater`, to ensure that @p _parm > @p _xpct ;
 * - `greater_equal`, to ensure that @p _parm >= @p _xpct ;
 * - `lower`, to ensure that @p _parm < @p _xpct ;
 * - `lower_equal`, to ensure that @p _parm <= @p _xpct.
 *
 * Both @p _parm and @p _xpct *MUST* be unsigned integers, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_hex_parm(word);
 *      cute_mock_hex_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_hex_parm(callee, word, equal,   UINT_MAX);
 *      cute_expect_hex_parm(callee, half, greater, USHRT_MAX / 2);
 *      caller(UINT_MAX, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_hex_parm()
 * - #CUTE_TEST
 */
#define cute_expect_hex_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(uint, \
	                                                  # _xpct, \
	                                                  _xpct))

/**
 * Check an hexadecimal unsigned integer function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the value of an unsigned integer function parameter expectation
 * scheduled using cute_expect_hex_parm(), cute_expect_hex_range() or
 * cute_expect_hex_set().
 * It is identical to the #cute_expect_uint_parm macro and both may be used
 * *interchangeably*.
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_hex_parm()
 * - cute_expect_hex_range()
 * - cute_expect_hex_set()
 * - #CUTE_TEST
 */
#define cute_mock_hex_parm(_parm) \
	cute_expect_check_uint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(uint, # _parm, _parm))

/**
 * Schedule an hexadecimal unsigned integer range function parameter mock
 * expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference unsigned integer range to perform the check
 *                  against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_hex_parm() is called.
 * It is similar to the #cute_expect_uint_range macro with the ability to
 * **display values as hexadecimal numbers** when producing failure
 * @rstsubst{report}.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct range using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _xpct.min <= @p _parm <= @p _xpct.max ;
 * - `not_in` to ensure that @p _parm < @p _xpct.min or @p _parm > @p _xpct.max.
 *
 * @p _parm *MUST* be an unsigned integer, i.e., either :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @p _xpct *MUST* be a cute_uint_range unsigned integer range as defined by the
 * #CUTE_UINT_RANGE macro.
 *
 * This macro may be used from within @rstsubst{fixture functions} as well as
 * @rstsubst{test functions}.
 *
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_hex_parm(word);
 *      cute_mock_hex_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_range range = CUTE_UINT_RANGE(0, USHRT_MAX / 2);
 *
 *      cute_expect_hex_range(callee, word, in,     CUTE_UINT_RANGE(0, 10));
 *      cute_expect_hex_range(callee, half, not_in, range);
 *      caller(5, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_hex_parm()
 * - #CUTE_UINT_RANGE
 * - cute_uint_range
 * - #CUTE_TEST
 */
#define cute_expect_hex_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

/**
 * Schedule an hexadecimal unsigned integer set function parameter mock
 * expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference unsigned integer set to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_hex_parm() is called.
 * It is similar to the #cute_expect_uint_set macro with the ability to
 * **display values as hexadecimal numbers** when producing failure
 * @rstsubst{report}.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed}
 * if the comparison of @p _parm against the @p _xpct set using the @p _op
 * comparison operator results in a failure. Comparison is performed according
 * to the following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `in`, to ensure that @p _parm equals to one the @p _xpct set values ;
 * - `not_in` to ensure that @p _parm equals to none of the @p _xpct set values.
 *
 * @p _parm *MUST* be an unsigned integer, i.e., either :
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
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 * #include <cute/expect.h>
 * #include <limits.h>
 *
 * static void
 * callee(int word, short half)
 * {
 *      cute_mock_hex_parm(word);
 *      cute_mock_hex_parm(half);
 * }
 *
 * static void
 * caller(int word, short half)
 * {
 *      callee(word, half);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_set set = CUTE_UINT_SET(0, 10, USHRT_MAX / 2);
 *
 *      cute_expect_hex_set(callee, word, in,     CUTE_UINT_SET(0, 5, 10));
 *      cute_expect_hex_set(callee, half, not_in, set);
 *      caller(10, USHRT_MAX);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_hex_parm()
 * - #CUTE_UINT_SET
 * - cute_uint_set
 * - #CUTE_TEST
 */
#define cute_expect_hex_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

/******************************************************************************
 * Floating point number mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_flt_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(flt, # _xpct, _xpct))

#define cute_mock_flt_parm(_parm) \
	cute_expect_check_flt_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(flt, # _parm, _parm))

extern void
cute_expect_check_flt_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check)
	__cute_export;

extern void
cute_expect_sched_flt_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_greater(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_greater_equal(const char *            file,
                                         int                     line,
                                         const char *            function,
                                         const char *            parm,
                                         const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_lower(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_lower_equal(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_flt * expect)
	__cute_export;

#define cute_expect_flt_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

extern void
cute_expect_sched_flt_parm_in_range(const char *                  file,
                                    int                           line,
                                    const char *                  function,
                                    const char *                  parm,
                                    const struct cute_flt_range * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_not_in_range(
	const char *                  file,
	int                           line,
	const char *                  function,
	const char *                  parm,
	const struct cute_flt_range * expect)
	__cute_export;

#define cute_expect_flt_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

extern void
cute_expect_sched_flt_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_flt_set * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_flt_set * expect)
	__cute_export;

/******************************************************************************
 * Floating point number return value expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_flt_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * retval)
	__cute_export;

#define cute_expect_flt_retval(_func, _retval) \
	cute_expect_sched_flt_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(flt, # _retval, _retval))

extern long double
cute_expect_check_flt_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

#define cute_mock_flt_retval() \
	cute_expect_check_flt_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * String parameter expectation handling
 ******************************************************************************/

#define cute_expect_str_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_str_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
		                            &__CUTE_VALUE(str, # _xpct, _xpct))

#define cute_mock_str_parm(_parm) \
	cute_expect_check_str_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(str, # _parm, _parm))

extern void
cute_expect_check_str_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_str * check)
	__cute_export;

extern void
cute_expect_sched_str_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_begin(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_begin(const char *            file,
                                     int                     line,
                                     const char *            function,
                                     const char *            parm,
                                     const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_end(const char *            file,
                               int                     line,
                               const char *            function,
                               const char *            parm,
                               const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_end(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_contain(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_contain(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_str * expect)
	__cute_export;

#define cute_expect_str_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_str_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

extern void
cute_expect_sched_str_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_str_set * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_str_set * expect)
	__cute_export;

/******************************************************************************
 * String return value expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_str_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_str * retval)
	__cute_export;

#define cute_expect_str_retval(_func, _retval) \
	cute_expect_sched_str_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(str, # _retval, _retval))

extern char *
cute_expect_check_str_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

#define cute_mock_str_retval() \
	cute_expect_check_str_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Pointer mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_ptr_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(ptr, # _xpct, _xpct))

#define cute_mock_ptr_parm(_parm) \
	cute_expect_check_ptr_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(ptr, # _parm, _parm))

extern void
cute_expect_check_ptr_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_ptr * check)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_greater(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_greater_equal(const char *            file,
                                         int                     line,
                                         const char *            function,
                                         const char *            parm,
                                         const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_lower(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_lower_equal(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_ptr * expect)
	__cute_export;

#define cute_expect_ptr_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

extern void
cute_expect_sched_ptr_parm_in_range(const char *                  file,
                                    int                           line,
                                    const char *                  function,
                                    const char *                  parm,
                                    const struct cute_ptr_range * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_ptr_range * expect)
	__cute_export;

#define cute_expect_ptr_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op ## _set(__FILE__, \
                                                    __LINE__, \
                                                    # _func, \
                                                    # _parm, \
                                                    &(_xpct))

extern void
cute_expect_sched_ptr_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_ptr_set * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_ptr_set * expect)
	__cute_export;

/******************************************************************************
 * Pointer return value expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_ptr_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_ptr * retval)
	__cute_export;

#define cute_expect_ptr_retval(_func, _retval) \
	cute_expect_sched_ptr_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(ptr, # _retval, _retval))

extern void *
cute_expect_check_ptr_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

#define cute_mock_ptr_retval() \
	cute_expect_check_ptr_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Memory area parameter expectation handling
 ******************************************************************************/

#define cute_expect_mem_parm(_func, _parm, _op, _ptr, _sz) \
	cute_expect_sched_mem_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_MEM("{@ " # _ptr \
	                                                ":" # _sz "}", \
	                                                _ptr, \
	                                                _sz))

#define cute_mock_mem_parm(_parm) \
	cute_expect_check_mem_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(ptr, # _parm, _parm))

extern void
cute_expect_check_mem_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_ptr * check)
	__cute_export;

extern void
cute_expect_sched_mem_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_mem * expect)
	__cute_export;

extern void
cute_expect_sched_mem_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_mem * expect)
	__cute_export;

#endif /* _CUTE_EXPECT_H */
