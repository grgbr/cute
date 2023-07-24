#ifndef _CUTE_PRIV_TYPES_H
#define _CUTE_PRIV_TYPES_H

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

struct cute_sint {
	const char * expr;
	intmax_t     value;
};

struct cute_sint_range {
	const char * expr;
	intmax_t     min;
	intmax_t     max;
};

struct cute_sint_set {
	const char *     expr;
	unsigned int     count;
	const intmax_t * items;
};

struct cute_uint {
	const char * expr;
	uintmax_t    value;
};

struct cute_uint_range {
	const char * expr;
	uintmax_t    min;
	uintmax_t    max;
};

struct cute_uint_set {
	const char *      expr;
	unsigned int      count;
	const uintmax_t * items;
};

struct cute_flt {
	const char * expr;
	long double  value;
};

struct cute_flt_range {
	const char * expr;
	long double  min;
	long double  max;
};

struct cute_flt_set {
	const char *        expr;
	unsigned int        count;
	const long double * items;
};

struct cute_str {
	const char * expr;
	const char * value;
};

struct cute_str_set {
	const char *  expr;
	unsigned int  count;
	const char ** items;
};

struct cute_ptr {
	const char * expr;
	const void * value;
};

struct cute_ptr_range {
	const char * expr;
	const void * min;
	const void * max;
};

struct cute_ptr_set {
	const char *  expr;
	unsigned int  count;
	const void ** items;
};

struct cute_mem {
	const char * expr;
	const void * ptr;
	size_t       size;
};

#define __CUTE_MEM(_ptr, _sz) \
	((const struct cute_mem) { \
		.expr = "{@ " # _ptr ":" # _sz "}", \
		.ptr  = _ptr, \
		.size = _sz \
	 })

#endif /* _CUTE_PRIV_TYPES_H */
