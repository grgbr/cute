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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * static unsigned int
 * callee(void)
 * {
 *      return (unsigned int)cute_mock_uint_retval();
 * }
 *
 * static unsigned int
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_uint_retval(callee, 1U);
 *      cute_check_uint(caller(), equal, 1U);
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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
 * - or an equivalent *typedef*'ed type.
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

/**
 * Schedule a floating point number function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected floating point value to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_flt_parm() is called.
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
 * Both @p _parm and @p _xpct *MUST* be floating point numbers, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or an equivalent *typedef*'ed type.
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
 * callee(float float_value, double double_value)
 * {
 *      cute_mock_flt_parm(float_value);
 *      cute_mock_flt_parm(double_value);
 * }
 *
 * static void
 * caller(float float_value, double double_value)
 * {
 *      callee(float_value, double_value);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_flt_parm(callee, float_value, equal,    1.5f);
 *      cute_expect_flt_parm(callee, double_value, greater, 10.005);
 *      caller(1.5f, 5.7);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_flt_parm()
 * - #CUTE_TEST
 */
#define cute_expect_flt_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(flt, # _xpct, _xpct))

/**
 * Check a floating point number function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the value of a floating point number function parameter expectation scheduled
 * using cute_expect_flt_parm(), cute_expect_flt_range() or
 * cute_expect_flt_set().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_flt_parm()
 * - cute_expect_flt_range()
 * - cute_expect_flt_set()
 * - #CUTE_TEST
 */
#define cute_mock_flt_parm(_parm) \
	cute_expect_check_flt_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(flt, # _parm, _parm))

/**
 * Schedule a floating point number range function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference floating point number range to perform the check
 *                  against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_flt_parm() is called.
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
 * @p _parm *MUST* be a floating point number, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or an equivalent *typedef*'ed type.
 *
 * @p _xpct *MUST* be a cute_flt_range floating point number range as defined
 * by the #CUTE_FLT_RANGE macro.
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
 * callee(float float_value, double double_value)
 * {
 *      cute_mock_flt_parm(float_value);
 *      cute_mock_flt_parm(double_value);
 * }
 *
 * static void
 * caller(float float_value, double double_value)
 * {
 *      callee(float_value, double_value);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_range range = CUTE_FLT_RANGE(-5.005, 5.005);
 *
 *      cute_expect_flt_range(callee,
 *                            float_value,
 *                            in,
 *                            CUTE_FLT_RANGE(0.005f, 9.005f));
 *      cute_expect_flt_range(callee,
 *                            double_value,
 *                            not_in,
 *                            range);
 *      caller(5.0025f, -7.725);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_flt_parm()
 * - #CUTE_FLT_RANGE
 * - cute_flt_range
 * - #CUTE_TEST
 */
#define cute_expect_flt_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

/**
 * Schedule a floating point number set function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference floating point number set to perform the check
 *                  against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_flt_parm() is called.
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
 * @p _parm *MUST* be a floating point number, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or an equivalent *typedef*'ed type.
 *
 * @p _xpct *MUST* be a cute_flt_set floating point number set as defined by
 * the #CUTE_FLT_SET macro.
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
 * callee(float float_value, double double_value)
 * {
 *      cute_mock_flt_parm(float_value);
 *      cute_mock_flt_parm(double_value);
 * }
 *
 * static void
 * caller(float float_value, double double_value)
 * {
 *      callee(float_value, double_value);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_set set = CUTE_FLT_SET(-10.005, 5.005, 10.005);
 *
 *      cute_expect_flt_set(callee,
 *                          float_value,
 *                          in,
 *                          CUTE_FLT_SET(0.5f, 5.5f, 10.5f));
 *      cute_expect_flt_set(callee,
 *                          double_value,
 *                          not_in,
 *                          set);
 *      caller(5.5f, 7.005);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_flt_parm()
 * - #CUTE_FLT_SET
 * - cute_flt_set
 * - #CUTE_TEST
 */
#define cute_expect_flt_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

/******************************************************************************
 * Floating point number return value expectation handling
 ******************************************************************************/

/**
 * Schedule a floating point number function return value mock expectation.
 *
 * @param[in] _func   function name
 * @param[in] _retval value to be returned from mocked function
 *
 * Define and schedule an @rstsubst{expectation} which value is returned when
 * cute_mock_flt_retval() is called.
 *
 * @p _retval *MUST* be a floating point number, i.e., either :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or an equivalent *typedef*'ed type.
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
 * static float
 * callee(void)
 * {
 *      return (float)cute_mock_flt_retval();
 * }
 *
 * static float
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_flt_retval(callee, 1.5f);
 *      cute_check_flt(caller(), equal, 1.5f);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_flt_retval()
 * - #CUTE_TEST
 */
#define cute_expect_flt_retval(_func, _retval) \
	cute_expect_sched_flt_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(flt, # _retval, _retval))

/**
 * Return a floating point number function return value mock expectation.
 *
 * @return scheduled return value cast as an `long double`
 *
 * Extract and return a a floating point number value scheduled using
 * cute_expect_flt_retval().
 *
 * This macro must be called from within the mocked function that returns the
 * scheduled value.
 *
 * @see
 * - cute_expect_flt_retval()
 * - #CUTE_TEST
 */
#define cute_mock_flt_retval() \
	cute_expect_check_flt_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * String parameter expectation handling
 ******************************************************************************/

/**
 * Schedule a string function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected string content to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_str_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed} if
 * the comparison of @p _parm against @p _xpct using the @p _op comparison
 * operator results in a failure. Comparison is performed according to the
 * following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that @p _parm content equals to @p _xpct content;
 * - `unequal` to ensure that @p _parm content differs from @p _xpct content;
 * - `begin`, to ensure that @p _parm content begins with @p _xpct content;
 * - `end`, to ensure that @p _parm content ends with @p _xpct content;
 * - `contain`, to ensure that @p _parm content contains @p _xpct content, i.e.,
 *   @p _xpct is a substring of @p _parm;
 * - `not_contain`, to ensure that @p _parm content does not containt @p _xpct
 *   content, i.e., @p _xpct is not a substring of @p _parm.
 *
 * Both @p _parm and @p _xpct *MUST* be `NULL` terminated C strings.
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
 * callee(const char * string)
 * {
 *      cute_mock_str_parm(string);
 * }
 *
 * static void
 * caller(const char * string)
 * {
 *      callee(string);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_str_parm(callee, string, equal, "a test string");
 *      cute_expect_str_parm(callee, string, begin, "another");
 *
 *      caller("a test string");
 *      caller("another test string");
 * }
 * @endcode
 *
 * @see
 * - cute_mock_str_parm()
 * - #CUTE_TEST
 */
#define cute_expect_str_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_str_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(str, # _xpct, _xpct))

/**
 * Check a string function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the content of a string function parameter expectation scheduled
 * using cute_expect_str_parm() or cute_expect_str_set().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_str_parm()
 * - cute_expect_str_set()
 * - #CUTE_TEST
 */
#define cute_mock_str_parm(_parm) \
	cute_expect_check_str_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(str, # _parm, _parm))

/**
 * Schedule a string set function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference string set to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_str_parm() is called.
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
 * @p _parm *MUST* be a `NULL` terminated C string.
 *
 * @p _xpct *MUST* be a cute_str_set string set as defined by
 * the #CUTE_STR_SET macro.
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
 * callee(const char * string)
 * {
 *      cute_mock_str_parm(string);
 * }
 *
 * static void
 * caller(const char * string)
 * {
 *      callee(string);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_str_set set = CUTE_STR_SET("zero", "one", "two");
 *
 *      cute_expect_str_set(callee,
 *                          string,
 *                          in,
 *                          CUTE_STR_SET("Lorem", "ipsum");
 *      cute_expect_str_set(callee,
 *                          string,
 *                          not_in,
 *                          set);
 *
 *      caller("one");
 *      caller("dolor");
 * }
 * @endcode
 *
 * @see
 * - cute_mock_str_parm()
 * - #CUTE_STR_SET
 * - cute_str_set
 * - #CUTE_TEST
 */
#define cute_expect_str_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_str_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

/******************************************************************************
 * String return value expectation handling
 ******************************************************************************/

/**
 * Schedule a string function return value mock expectation.
 *
 * @param[in] _func   function name
 * @param[in] _retval value to be returned from mocked function
 *
 * Define and schedule an @rstsubst{expectation} which value is returned when
 * cute_mock_str_retval() is called.
 *
 * @p _retval *MUST* be a `NULL` terminated C string.
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
 * static const char *
 * callee(void)
 * {
 *      return cute_mock_str_retval();
 * }
 *
 * static const char *
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_str_retval(callee, "a test string");
 *      cute_check_str(caller(), contain, "test");
 * }
 * @endcode
 *
 * @see
 * - cute_mock_str_retval()
 * - #CUTE_TEST
 */
#define cute_expect_str_retval(_func, _retval) \
	cute_expect_sched_str_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(str, # _retval, _retval))

/**
 * Return a string function return value mock expectation.
 *
 * @return scheduled return value cast as a `char *`
 *
 * Extract and return a pointer to a `NULL` terminated C string scheduled using
 * cute_expect_str_retval().
 *
 * This macro must be called from within the mocked function that returns the
 * scheduled value.
 *
 * @see
 * - cute_expect_str_retval()
 * - #CUTE_TEST
 */
#define cute_mock_str_retval() \
	cute_expect_check_str_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Pointer mock parameter expectation handling
 ******************************************************************************/

/**
 * Schedule a pointer function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct expected pointer value to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_ptr_parm() is called.
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
 * Both @p _parm and @p _xpct *MUST* be pointers, i.e., either `void *` or an
 * equivalent *typedef*'ed type.
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
 * callee(const void * ptr)
 * {
 *      cute_mock_ptr_parm(ptr);
 * }
 *
 * static void
 * caller(const void * ptr)
 * {
 *      callee(ptr);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      int val = 0;
 *
 *      cute_expect_ptr_parm(callee, ptr, equal, (const void *)&val);
 *      caller((const void *)&val);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_ptr_parm()
 * - #CUTE_TEST
 */
#define cute_expect_ptr_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(ptr, # _xpct, _xpct))

/**
 * Check a pointer function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the value of a pointer function parameter expectation scheduled using
 * cute_expect_ptr_parm(), cute_expect_ptr_range() or cute_expect_ptr_set().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_ptr_parm()
 * - cute_expect_ptr_range()
 * - cute_expect_ptr_set()
 * - #CUTE_TEST
 */
#define cute_mock_ptr_parm(_parm) \
	cute_expect_check_ptr_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(ptr, # _parm, _parm))

/**
 * Schedule a pointer range function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference pointer range to perform the check
 *                  against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_ptr_parm() is called.
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
 * @p _parm *MUST* be a pointer, i.e., either `void *` or an equivalent
 * *typedef*'ed type.
 *
 * @p _xpct *MUST* be a cute_ptr_range pointer range as defined by the
 * #CUTE_PTR_RANGE macro.
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
 * callee(const void * ptr)
 * {
 *      cute_mock_ptr_parm(ptr);
 * }
 *
 * static void
 * caller(const void * ptr)
 * {
 *      callee(ptr);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const int                   array[2] = { 0, 1, 2 };
 *      const struct cute_ptr_range range =
 *              CUTE_PTR_RANGE((const void *)&array[0],
 *                             (const void *)&array[1]);
 *
 *      cute_expect_ptr_range(callee,
 *                            ptr,
 *                            in,
 *                            CUTE_PTR_RANGE((const void *)&array[0],
 *                                           (const void *)&array[2]));
 *      cute_expect_ptr_range(callee,
 *                            ptr,
 *                            not_in,
 *                            range);
 *
 *      caller((const void *)&array[1]);
 *      caller((const void *)&array[2]);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_ptr_parm()
 * - #CUTE_PTR_RANGE
 * - cute_ptr_range
 * - #CUTE_TEST
 */
#define cute_expect_ptr_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

/**
 * Schedule a pointer set function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _xpct reference pointer set to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_ptr_parm() is called.
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
 * @p _parm *MUST* be a pointer, i.e., either `void *` or an equivalent
 * *typedef*'ed type.
 *
 * @p _xpct *MUST* be a cute_ptr_set pointer set as defined by the #CUTE_PTR_SET
 * macro.
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
 * callee(const void * ptr)
 * {
 *      cute_mock_ptr_parm(ptr);
 * }
 *
 * static void
 * caller(const void * ptr)
 * {
 *      callee(ptr);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      int                       array[2] = { 0, 1, 2 };
 *      const struct cute_ptr_set set =
 *              CUTE_PTR_SET((const void *)&array[2],
 *                           (const void *)&array[0]);
 *
 *      cute_expect_ptr_set(callee,
 *                          ptr,
 *                          in,
 *                          CUTE_PTR_SET((const void *)&array[2],
 *                                       (const void *)&array[0],
 *                                       (const void *)&array[1]);
 *      cute_expect_ptr_set(callee,
 *                          ptr,
 *                          not_in,
 *                          set);
 *
 *      caller((const void *)&array[0]);
 *      caller((const void *)&array[1]);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_ptr_parm()
 * - #CUTE_PTR_SET
 * - cute_ptr_set
 * - #CUTE_TEST
 */
#define cute_expect_ptr_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_ptr_parm_ ##  _op ## _set(__FILE__, \
                                                    __LINE__, \
                                                    # _func, \
                                                    # _parm, \
                                                    &(_xpct))

/******************************************************************************
 * Pointer return value expectation handling
 ******************************************************************************/

/**
 * Schedule a pointer function return value mock expectation.
 *
 * @param[in] _func   function name
 * @param[in] _retval value to be returned from mocked function
 *
 * Define and schedule an @rstsubst{expectation} which value is returned when
 * cute_mock_ptr_retval() is called.
 *
 * @p _retval *MUST* be a pointer, i.e., either `void *` or an equivalent
 * *typedef*'ed type.
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
 * static const void *
 * callee(void)
 * {
 *      return cute_mock_ptr_retval();
 * }
 *
 * static const void *
 * caller(void)
 * {
 *      return callee();
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      cute_expect_ptr_retval(callee, NULL);
 *      cute_check_ptr(caller(), equal, NULL);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_ptr_retval()
 * - #CUTE_TEST
 */
#define cute_expect_ptr_retval(_func, _retval) \
	cute_expect_sched_ptr_retval(__FILE__, \
	                             __LINE__, \
	                             # _func, \
	                             &__CUTE_VALUE(ptr, # _retval, _retval))

/**
 * Return a pointer function return value mock expectation.
 *
 * @return scheduled return value cast as an `void *`
 *
 * Extract and return a pointer value scheduled using cute_expect_ptr_retval().
 *
 * This macro must be called from within the mocked function that returns the
 * scheduled value.
 *
 * @see
 * - cute_expect_ptr_retval()
 * - #CUTE_TEST
 */
#define cute_mock_ptr_retval() \
	cute_expect_check_ptr_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Memory area parameter expectation handling
 ******************************************************************************/

/**
 * Schedule a memory area function parameter mock expectation.
 *
 * @param[in] _func function name
 * @param[in] _parm function parameter name
 * @param[in] _op   constraint operation used to check the expectation
 * @param[in] _ptr  address of expected memory area to perform the check against
 * @param[in] _sz   size of expected memory area to perform the check against
 *
 * Define and schedule an @rstsubst{expectation} that is checked when
 * cute_mock_mem_parm() is called.
 *
 * The check performed aborts current test and marks it as @rstsubst{failed} if
 * the comparison of @p _parm against @p _xpct using the @p _op comparison
 * operator results in a failure. Comparison is performed according to the
 * following formula :
 *
 *     _parm <_op> _xpct
 *
 * Where @p _op *MUST* be one of :
 * - `equal` to ensure that content of @p _parm == @p _xpct content;
 * - `unequal` to ensure that content of @p _parm != @p _xpct content.
 *
 * Both @p _parm and @p _xpct *MUST* be pointers, i.e., either `void *` or an
 * equivalent *typedef*'ed type.
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
 * callee(const int * area)
 * {
 *      cute_mock_mem_parm(area);
 * }
 *
 * static void
 * caller(const int * area)
 * {
 *      callee(area);
 * }
 *
 * CUTE_TEST(mytest)
 * {
 *      const int const array[2] = { 0, 1, 2 };
 *
 *      cute_expect_mem_parm(callee, area, equal,   array, sizeof(array));
 *      cute_expect_mem_parm(callee, area, unequal, array, 4U);
 *
 *      caller(array);
 *      caller(&array[1]);
 * }
 * @endcode
 *
 * @see
 * - cute_mock_mem_parm()
 * - #CUTE_TEST
 */
#define cute_expect_mem_parm(_func, _parm, _op, _ptr, _sz) \
	cute_expect_sched_mem_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_MEM("{@ " # _ptr \
	                                                ":" # _sz "}", \
	                                                _ptr, \
	                                                _sz))

/**
 * Check a pointer function parameter mock expectation.
 *
 * @param[in] _parm function parameter name
 *
 * Check the content pointed to by a pointer function parameter expectation
 * scheduled using cute_expect_mem_parm().
 *
 * This macro must be called from within the mocked function which is given the
 * parameter to verify.
 *
 * @see
 * - cute_expect_mem_parm()
 * - #CUTE_TEST
 */
#define cute_mock_mem_parm(_parm) \
	cute_expect_check_mem_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(ptr, # _parm, _parm))

#endif /* _CUTE_EXPECT_H */
