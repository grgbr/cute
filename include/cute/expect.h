#ifndef _CUTE_EXPECT_H
#define _CUTE_EXPECT_H

#include <cute/check.h>

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
 * Mock parameter expectation handling
 ******************************************************************************/

#define cute_expect_sint(_func, _parm, _op, _xpct) \
	cute_expect_sched_sint_parm_ ##  _op( \
		__FILE__, \
		__LINE__, \
	        # _func, \
	        # _parm, \
		&__CUTE_VALUE(sint, _xpct))

#define cute_mock_sint(_parm) \
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

#if 0
/******************************************************************************
 * Mock return expectation handling
 ******************************************************************************/

extern void
cute_expect_sched_retval(const char * file,
                         int          line,
                         const char * function,
                         uintmax_t    retval)
	__cute_export;

#define cute_expect_retval(_func, _retval) \
	cute_expect_sched_retval(__FILE__, \
	                         __LINE__, \
	                         # _func, \
	                         (uintmax_t)(_retval))

extern uintmax_t
cute_expect_check_retval(const char * file, int line, const char * function)
	__cute_export;

#define cute_mock_retval() \
	cute_expect_check_retval(__FILE__, __LINE__, __func__)
#endif
#endif /* _CUTE_EXPECT_H */
