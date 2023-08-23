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

#include <cute/types.h>
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
 * 	cute_mock_assert(expression, file, line, function);
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

#define cute_expect_sint_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op(__FILE__, \
	                                     __LINE__, \
	                                     # _func, \
	                                     # _parm, \
		                             &__CUTE_VALUE(sint, # _xpct, _xpct))

#define cute_mock_sint_parm(_parm) \
	cute_expect_check_sint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(sint, # _parm, _parm))

extern void
cute_expect_check_sint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check)
	__cute_export;

extern void
cute_expect_sched_sint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_sint * expect)
	__cute_export;

#define cute_expect_sint_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op ## _range(__FILE__, \
	                                               __LINE__, \
	                                               # _func, \
	                                               # _parm, \
	                                               &(_xpct))

extern void
cute_expect_sched_sint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_sint_range * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_sint_range * expect)
	__cute_export;

#define cute_expect_sint_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op ## _set(__FILE__, \
	                                             __LINE__, \
	                                             # _func, \
	                                             # _parm, \
	                                             &(_xpct))

extern void
cute_expect_sched_sint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_sint_set * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_sint_set * expect)
	__cute_export;

/******************************************************************************
 * Signed integer return value expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_sint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * retval)
	__cute_export;

#define cute_expect_sint_retval(_func, _retval) \
	cute_expect_sched_sint_retval(__FILE__, \
	                              __LINE__, \
	                              # _func, \
	                              &__CUTE_VALUE(sint, # _retval, _retval))

extern intmax_t
cute_expect_check_sint_retval(const char * file,
                              int          line,
                              const char * function)
	__cute_export;

#define cute_mock_sint_retval() \
	cute_expect_check_sint_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Unsigned integer mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_uint_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op(__FILE__, \
	                                     __LINE__, \
	                                     # _func, \
	                                     # _parm, \
	                                     &__CUTE_VALUE(uint, \
	                                                   # _xpct, \
	                                                   _xpct))

#define cute_mock_uint_parm(_parm) \
	cute_expect_check_uint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(uint, # _parm, _parm))

extern void
cute_expect_check_uint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check)
	__cute_export;

extern void
cute_expect_sched_uint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_uint * expect)
	__cute_export;

#define cute_expect_uint_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op ## _range(__FILE__, \
	                                               __LINE__, \
	                                               # _func, \
	                                               # _parm, \
	                                               &(_xpct))

extern void
cute_expect_sched_uint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_uint_range * expect)
	__cute_export;

#define cute_expect_uint_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_uint_parm_ ##  _op ## _set(__FILE__, \
	                                             __LINE__, \
	                                             # _func, \
	                                             # _parm, \
	                                             &(_xpct))

extern void
cute_expect_sched_uint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_uint_set * expect)
	__cute_export;

/******************************************************************************
 * Unsigned integer return value expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_uint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * retval)
	__cute_export;

#define cute_expect_uint_retval(_func, _retval) \
	cute_expect_sched_uint_retval(__FILE__, \
	                              __LINE__, \
	                              # _func, \
	                              &__CUTE_VALUE(uint, # _retval, _retval))

extern uintmax_t
cute_expect_check_uint_retval(const char * file,
                              int          line,
                              const char * function)
	__cute_export;

#define cute_mock_uint_retval() \
	cute_expect_check_uint_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Unsigned hexadecimal integer mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_hex_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(uint, \
	                                                  # _xpct, \
	                                                  _xpct))

#define cute_mock_hex_parm(_parm) \
	cute_expect_check_uint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(uint, # _parm, _parm))

extern void
cute_expect_sched_hex_parm_equal(const char *             file,
                                 int                      line,
                                 const char *             function,
                                 const char *             parm,
                                 const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_unequal(const char *             file,
                                   int                      line,
                                   const char *             function,
                                   const char *             parm,
                                   const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_greater(const char *             file,
                                   int                      line,
                                   const char *             function,
                                   const char *             parm,
                                   const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_greater_equal(const char *             file,
                                         int                      line,
                                         const char *             function,
                                         const char *             parm,
                                         const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_lower(const char *             file,
                                 int                      line,
                                 const char *             function,
                                 const char *             parm,
                                 const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_lower_equal(const char *             file,
                                       int                      line,
                                       const char *             function,
                                       const char *             parm,
                                       const struct cute_uint * expect)
	__cute_export;

#define cute_expect_hex_range(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op ## _range(__FILE__, \
	                                              __LINE__, \
	                                              # _func, \
	                                              # _parm, \
	                                              &(_xpct))

extern void
cute_expect_sched_hex_parm_in_range(const char *                   file,
                                    int                            line,
                                    const char *                   function,
                                    const char *                   parm,
                                    const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_uint_range * expect)
	__cute_export;

#define cute_expect_hex_set(_func, _parm, _op, _xpct) \
	cute_expect_sched_hex_parm_ ##  _op ## _set(__FILE__, \
	                                            __LINE__, \
	                                            # _func, \
	                                            # _parm, \
	                                            &(_xpct))

extern void
cute_expect_sched_hex_parm_in_set(const char *                 file,
                                  int                          line,
                                  const char *                 function,
                                  const char *                 parm,
                                  const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_not_in_set(const char *                 file,
                                      int                          line,
                                      const char *                 function,
                                      const char *                 parm,
                                      const struct cute_uint_set * expect)
	__cute_export;

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
