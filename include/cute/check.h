#ifndef _CUTE_CHECK_H
#define _CUTE_CHECK_H

#include <stdint.h>

#define cute_typeof_member(_type, _member) \
	typeof(((_type *)0)->_member)

#define __CUTE_SCALAR(_type, _val) \
	((const struct cute_ ## _type) { \
		.expr  = # _val, \
		.value = _val \
	 })

#define __CUTE_CHECK_SCALAR(_type, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op(__FILE__, \
	                                 __LINE__, \
	                                 __func__, \
	                                 &__CUTE_SCALAR(_type, _chk), \
	                                 &__CUTE_SCALAR(_type, _xpct))

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
	                                    &__CUTE_SCALAR(_type, _chk), \
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
		.items = (const __CUTE_SET_TYPE(_type) []){ __VA_ARGS__ } \
	 })

#define __CUTE_CHECK_SET(_type, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op ## _set(__FILE__, \
	                                         __LINE__, \
	                                         __func__, \
	                                         &__CUTE_SCALAR(_type, _chk), \
	                                         &(_xpct))

struct cute_sint {
	const char * expr;
	intmax_t     value;
};

#define cute_check_sint(_chk, _op, _xpct) \
	__CUTE_CHECK_SCALAR(sint, _chk, _op, _xpct)

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

#endif /* _CUTE_CHECK_H */
