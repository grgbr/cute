#ifndef _CUTE_ENSURE_H
#define _CUTE_ENSURE_H

#include <cute/cute.h>
#include <stdint.h>

extern void
_cute_ensure(bool         fail,
             const char * reason,
             const char * file,
             int          line,
             const char * function) __cute_export;

#define cute_ensure(_expr) \
	_cute_ensure(!(_expr), # _expr, __FILE__, __LINE__, __func__)

extern void
cute_ensure_sint_equal(const char * chk_expr,
                       intmax_t     chk_val,
                       const char * xpct_expr,
                       intmax_t     xpct_val,
                       const char * file,
                       int          line,
                       const char * function) __cute_export;

#define cute_ensure_sint(_a, _op, _b) \
	cute_ensure_sint_ ## _op (# _a, \
	                          _a, \
	                          # _b, \
	                          _b, \
	                          __FILE__, \
	                          __LINE__, \
	                          __func__)

#endif /* _CUTE_ENSURE_H */
