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

#define __CUTE_MEM(_ptr, _sz) \
	((const struct cute_mem) { \
		.expr = "{@ " # _ptr ":" # _sz "}", \
		.ptr  = _ptr, \
		.size = _sz \
	 })

#endif /* _CUTE_PRIV_TYPES_H */
