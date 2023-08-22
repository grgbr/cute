/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_PRIV_TYPES_H
#define _CUTE_PRIV_TYPES_H

#include <stdint.h>
#include <sys/types.h>

#define cute_typeof_member(_type, _member) \
	typeof(((_type *)0)->_member)

struct cute_sint {
	const char * expr;
	intmax_t     value;
};

struct cute_uint {
	const char * expr;
	uintmax_t    value;
};

struct cute_flt {
	const char * expr;
	long double  value;
};

struct cute_str {
	const char * expr;
	const char * value;
};

struct cute_ptr {
	const char * expr;
	const void * value;
};

#define __CUTE_VALUE(_type, _expr, _val) \
	((const struct cute_ ## _type) { \
		.expr  = _expr, \
		.value = _val \
	 })

#define __CUTE_CHECK_VALUE(_type, _cexpr, _chk, _op, _xexpr, _xpct) \
	cute_check_ ## _type ## _ ## _op(__FILE__, \
	                                 __LINE__, \
	                                 __func__, \
	                                 &__CUTE_VALUE(_type, _cexpr, _chk), \
	                                 &__CUTE_VALUE(_type, _xexpr, _xpct))

#define __CUTE_CHECK_HEX(_cexpr, _chk, _op, _xexpr, _xpct) \
	cute_check_hex_ ## _op(__FILE__, \
	                       __LINE__, \
	                       __func__, \
	                       &__CUTE_VALUE(uint, _cexpr, _chk), \
	                       &__CUTE_VALUE(uint, _xexpr, _xpct))

#define __CUTE_RANGE(_type, _expr, _min, _max) \
	((const struct cute_ ## _type ## _range) { \
		.expr = _expr, \
		.min  = _min, \
		.max  = _max \
	 })

#define __CUTE_CHECK_RANGE(_type, _cexpr, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op ## _range(__FILE__, \
	                                           __LINE__, \
	                                           __func__, \
	                                           &__CUTE_VALUE(_type, \
	                                                         _cexpr, \
	                                                         _chk), \
	                                           &(_xpct))

#define __CUTE_CHECK_HEX_RANGE(_cexpr, _chk, _op, _xpct) \
	cute_check_hex_ ## _op ## _range(__FILE__, \
	                                 __LINE__, \
	                                 __func__, \
	                                 &__CUTE_VALUE(uint, \
	                                               _cexpr, \
	                                               _chk), \
	                                 &(_xpct))

#define __CUTE_SET_TYPE(_type) \
	cute_typeof_member(struct cute_ ## _type ## _set, items[0])

#define __CUTE_SET_COUNT(_type, ...) \
	(sizeof((__CUTE_SET_TYPE(_type) []){ __VA_ARGS__ }) / \
	 sizeof(__CUTE_SET_TYPE(_type)))

#define __CUTE_SET(_type, _expr, ...) \
	((const struct cute_ ## _type ## _set) { \
		.expr  = _expr, \
		.count = __CUTE_SET_COUNT(_type, __VA_ARGS__), \
		.items = (__CUTE_SET_TYPE(_type) []){ __VA_ARGS__ } \
	 })

#define __CUTE_CHECK_SET(_type, _cexpr, _chk, _op, _xpct) \
	cute_check_ ## _type ## _ ## _op ## _set(__FILE__, \
	                                         __LINE__, \
	                                         __func__, \
	                                         &__CUTE_VALUE(_type, \
	                                                       _cexpr, \
	                                                       _chk), \
	                                         &(_xpct))

#define __CUTE_CHECK_HEX_SET(_cexpr, _chk, _op, _xpct) \
	cute_check_hex_ ## _op ## _set(__FILE__, \
	                               __LINE__, \
	                               __func__, \
	                               &__CUTE_VALUE(uint, \
	                                             _cexpr, \
	                                             _chk), \
	                               &(_xpct))

struct cute_mem {
	const char * expr;
	const void * ptr;
	size_t       size;
};

#define __CUTE_MEM(_expr, _ptr, _sz) \
	((const struct cute_mem) { \
		.expr = _expr, \
		.ptr  = _ptr, \
		.size = _sz \
	 })

#endif /* _CUTE_PRIV_TYPES_H */
