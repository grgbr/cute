#ifndef _CUTE_CHECK_H
#define _CUTE_CHECK_H

#include <cute/cute.h>
#include <stdint.h>
#include <sys/types.h>

#define cute_typeof_member(_type, _member) \
	typeof(((_type *)0)->_member)

#define __CUTE_VALUE(_type, _val) \
	((const struct cute_ ## _type) { \
		.expr  = # _val, \
		.value = _val \
	 })

#define __CUTE_CHECK_VALUE(_type, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op(__FILE__, \
	                                 __LINE__, \
	                                 __func__, \
	                                 &__CUTE_VALUE(_type, _chk), \
	                                 &__CUTE_VALUE(_type, _xpct))

#define __CUTE_RANGE_STR(_min, _max) \
	"{" # _min " ... " # _max "}"

#define __CUTE_RANGE(_type, _min, _max) \
	((const struct cute_ ## _type ## _range) { \
		.expr = __CUTE_RANGE_STR(_min, _max), \
		.min  = _min, \
		.max  = _max \
	 })

#define __CUTE_CHECK_RANGE(_type, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op ## _range(__FILE__, \
	                                           __LINE__, \
	                                           __func__, \
	                                           &__CUTE_VALUE(_type, _chk), \
	                                           &(_xpct))

#define __CUTE_SET_STR(...) \
	"{" # __VA_ARGS__ "}"

#define __CUTE_SET_TYPE(_type) \
	cute_typeof_member(struct cute_ ## _type ## _set, items[0])

#define __CUTE_SET_COUNT(_type, ...) \
	(sizeof((__CUTE_SET_TYPE(_type) []){ __VA_ARGS__ }) / \
	 sizeof(__CUTE_SET_TYPE(_type)))

#define __CUTE_SET(_type, ...) \
	((const struct cute_ ## _type ## _set) { \
		.expr  = __CUTE_SET_STR(__VA_ARGS__), \
		.count = __CUTE_SET_COUNT(_type, __VA_ARGS__), \
		.items = (__CUTE_SET_TYPE(_type) []){ __VA_ARGS__ } \
	 })

#define __CUTE_CHECK_SET(_type, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op ## _set(__FILE__, \
	                                         __LINE__, \
	                                         __func__, \
	                                         &__CUTE_VALUE(_type, _chk), \
	                                         &(_xpct))

/******************************************************************************
 * Assertion checking
 ******************************************************************************/

extern void
__cute_check_assert(bool         fail,
                    const char * file,
                    int          line,
                    const char * function,
                    const char * expr) __cute_export;

#define cute_check_assert(_expr) \
	__cute_check_assert(!(_expr), __FILE__, __LINE__, __func__, # _expr)

/******************************************************************************
 * Signed integral numbers checking
 ******************************************************************************/

struct cute_sint {
	const char * expr;
	intmax_t     value;
};

#define cute_check_sint(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(sint, _chk, _op, _xpct)

struct cute_sint_range {
	const char * expr;
	intmax_t     min;
	intmax_t     max;
};

#define CUTE_SINT_RANGE(_min, _max) \
	__CUTE_RANGE(sint, _min, _max)

#define cute_check_sint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(sint, _chk, _op, _xpct)

struct cute_sint_set {
	const char *     expr;
	unsigned int     count;
	const intmax_t * items;
};

#define CUTE_SINT_SET(...) \
	__CUTE_SET(sint, __VA_ARGS__)

#define cute_check_sint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(sint, _chk, _op, _xpct)

extern void
cute_check_sint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * check,
                              const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check,
                            const struct cute_sint * expect) __cute_export;

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

struct cute_uint {
	const char * expr;
	uintmax_t    value;
};

#define cute_check_uint(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(uint, _chk, _op, _xpct)

struct cute_uint_range {
	const char * expr;
	uintmax_t    min;
	uintmax_t    max;
};

#define CUTE_UINT_RANGE(_min, _max) \
	__CUTE_RANGE(uint, _min, _max)

#define cute_check_uint_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(uint, _chk, _op, _xpct)

struct cute_uint_set {
	const char *      expr;
	unsigned int      count;
	const uintmax_t * items;
};

#define CUTE_UINT_SET(...) \
	__CUTE_SET(uint, __VA_ARGS__)

#define cute_check_uint_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(uint, _chk, _op, _xpct)

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
 * Floating point numbers checking
 ******************************************************************************/

struct cute_flt {
	const char * expr;
	long double  value;
};

#define cute_check_flt(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(flt, _chk, _op, _xpct)

struct cute_flt_range {
	const char * expr;
	long double  min;
	long double  max;
};

#define CUTE_FLT_RANGE(_min, _max) \
	__CUTE_RANGE(flt, _min, _max)

#define cute_check_flt_range(_chk, _op, _xpct) \
	__CUTE_CHECK_RANGE(flt, _chk, _op, _xpct)

struct cute_flt_set {
	const char *        expr;
	unsigned int        count;
	const long double * items;
};

#define CUTE_FLT_SET(...) \
	__CUTE_SET(flt, __VA_ARGS__)

#define cute_check_flt_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(flt, _chk, _op, _xpct)

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

struct cute_str {
	const char * expr;
	const char * value;
};

#define cute_check_str(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(str, _chk, _op, _xpct)

struct cute_str_set {
	const char *  expr;
	unsigned int  count;
	const char ** items;
};

#define CUTE_STR_SET(...) \
	__CUTE_SET(str, __VA_ARGS__)

#define cute_check_str_set(_chk, _op, _xpct) \
	__CUTE_CHECK_SET(str, _chk, _op, _xpct)

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

/******************************************************************************
 * Pointer checking
 ******************************************************************************/

struct cute_ptr {
	const char * expr;
	const void * value;
};

#define cute_check_ptr(_chk, _op, _xpct) \
	__CUTE_CHECK_VALUE(ptr, _chk, _op, _xpct)

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

/******************************************************************************
 * Memory area checking
 ******************************************************************************/

struct cute_mem {
	const char * expr;
	const void * value;
	size_t       size;
};

#define __CUTE_MEM_VALUE(_type, _ptr, _sz) \
	((const struct cute_mem) { \
		.expr  = "@" # _ptr "/" # _sz, \
		.value = _ptr, \
		.size  = _sz \
	 })

#define cute_check_mem(_chk, _op, _xpct, _sz) \
	cute_check_mem_ ## _op(__FILE__, \
	                       __LINE__, \
	                       __func__, \
	                       &__CUTE_VALUE(ptr, _chk), \
	                       &__CUTE_MEM_VALUE(mem, _xpct, _sz))

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
