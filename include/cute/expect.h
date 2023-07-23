#ifndef _CUTE_EXPECT_H
#define _CUTE_EXPECT_H

#include <cute/check.h>
#include <setjmp.h>

/******************************************************************************
 * Assertion mock expectation handling
 ******************************************************************************/

extern sigjmp_buf cute_expect_assert_env __cute_export;
extern bool       cute_expect_assert __cute_export;

extern void
cute_expect_fail_assert(const char * file, int line, const char * function)
	__cute_noreturn __cute_export;

#define cute_expect_assertion(_call) \
	{ \
		cute_expect_assert = true; \
		if (!sigsetjmp(cute_expect_assert_env, 1)) { \
			_call; \
			cute_expect_fail_assert(__FILE__, __LINE__, __func__); \
		} \
	}

extern void
cute_mock_assert(const char * expression,
                 const char * file,
                 unsigned int line,
                 const char * function)
	__cute_noreturn __cute_export;

/******************************************************************************
 * Mock call expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_call(const char * file, int line, const char * function)
	__cute_export;

#define cute_expect_call(_func) \
	cute_expect_sched_call(__FILE__, __LINE__, # _func)

extern void
cute_expect_check_call(const char * file, int line, const char * function)
	__cute_export;

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
		                             &__CUTE_VALUE(sint, _xpct))

#define cute_mock_sint_parm(_parm) \
	cute_expect_check_sint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(sint, _parm))

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
	                              &__CUTE_VALUE(sint, _retval))

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
	                                     &__CUTE_VALUE(uint, _xpct))

#define cute_mock_uint_parm(_parm) \
	cute_expect_check_uint_parm(__FILE__, \
	                            __LINE__, \
	                            __func__, \
	                            &__CUTE_VALUE(uint, _parm))

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
	                              &__CUTE_VALUE(uint, _retval))

extern uintmax_t
cute_expect_check_uint_retval(const char * file,
                              int          line,
                              const char * function)
	__cute_export;

#define cute_mock_uint_retval() \
	cute_expect_check_uint_retval(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Floating point number mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_flt_parm(_func, _parm, _op, _xpct) \
	cute_expect_sched_flt_parm_ ##  _op(__FILE__, \
	                                    __LINE__, \
	                                    # _func, \
	                                    # _parm, \
	                                    &__CUTE_VALUE(flt, _xpct))

#define cute_mock_flt_parm(_parm) \
	cute_expect_check_flt_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(flt, _parm))

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
	                             &__CUTE_VALUE(flt, _retval))

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
		                            &__CUTE_VALUE(str, _xpct))

#define cute_mock_str_parm(_parm) \
	cute_expect_check_str_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(str, _parm))

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
	                             &__CUTE_VALUE(str, _retval))

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
	                                    &__CUTE_VALUE(ptr, _xpct))

#define cute_mock_ptr_parm(_parm) \
	cute_expect_check_ptr_parm(__FILE__, \
	                           __LINE__, \
	                           __func__, \
	                           &__CUTE_VALUE(ptr, _parm))

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
	                             &__CUTE_VALUE(ptr, _retval))

extern void *
cute_expect_check_ptr_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

#define cute_mock_ptr_retval() \
	cute_expect_check_ptr_retval(__FILE__, __LINE__, __func__)


#endif /* _CUTE_EXPECT_H */
