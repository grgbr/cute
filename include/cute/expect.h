#ifndef _CUTE_EXPECT_H
#define _CUTE_EXPECT_H

/******************************************************************************
 * Mock call expectation handling
 ******************************************************************************/

extern void
cute_expect_push_call(const char * file, int line, const char * function)
	__cute_export;

#define cute_expect_call(_func) \
	cute_expect_push_call(__FILE__, __LINE__, # _func)

extern void
cute_expect_check_call(const char * file, int line, const char * function)
	__cute_export;

#define cute_mock_call() \
	cute_expect_check_call(__FILE__, __LINE__, __func__)

/******************************************************************************
 * Mock parameter expectation handling
 ******************************************************************************/

extern void
cute_expect_push_parm(const char * file,
                      int          line,
                      const char * function,
                      const char * parameter,
                      uintmax_t    value)
	__cute_export;

#define cute_expect_parm(_func, _parm) \
	cute_expect_push_parm(__FILE__, \
	                      __LINE__, \
	                      # _func, \
	                      # _parm, \
	                      (uintmax_t)(_parm))

extern void
cute_expect_check_parm(const char * file,
                       int          line,
                       const char * function,
                       const char * parameter,
                       uintmax_t    value)
	__cute_export;

#define cute_mock_parm(_parm) \
	cute_expect_check_parm(__FILE__, \
	                       __LINE__, \
	                       __func__, \
	                       # _parm, \
	                       (uintmax_t)(_parm))

/******************************************************************************
 * Mock return expectation handling
 ******************************************************************************/

extern void
cute_expect_push_retval(const char * file,
                        int          line,
                        const char * function,
                        uintmax_t    retval)
	__cute_export;

#define cute_expect_retval(_func, _retval) \
	cute_expect_push_retval(__FILE__, \
	                        __LINE__, \
	                        # _func, \
	                        (uintmax_t)(_retval))

extern uintmax_t
cute_expect_check_retval(const char * file, int line, const char * function)
	__cute_export;

#define cute_mock_retval() \
	cute_expect_check_retval(__FILE__, __LINE__, __func__)

#endif /* _CUTE_EXPECT_H */
