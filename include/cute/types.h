/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Public types
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      25 Jul 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
 */

#ifndef _CUTE_TYPES_H
#define _CUTE_TYPES_H

#include <cute/priv/types.h>

struct cute_sint {
	const char * expr;
	intmax_t     value;
};

struct cute_sint_range {
	const char * expr;
	intmax_t     min;
	intmax_t     max;
};

#define CUTE_SINT_RANGE(_min, _max) \
	__CUTE_RANGE(sint, _min, _max)

struct cute_sint_set {
	const char *     expr;
	unsigned int     count;
	const intmax_t * items;
};

#define CUTE_SINT_SET(...) \
	__CUTE_SET(sint, __VA_ARGS__)

struct cute_uint {
	const char * expr;
	uintmax_t    value;
};

struct cute_uint_range {
	const char * expr;
	uintmax_t    min;
	uintmax_t    max;
};

#define CUTE_UINT_RANGE(_min, _max) \
	__CUTE_RANGE(uint, _min, _max)

struct cute_uint_set {
	const char *      expr;
	unsigned int      count;
	const uintmax_t * items;
};

#define CUTE_UINT_SET(...) \
	__CUTE_SET(uint, __VA_ARGS__)

struct cute_flt {
	const char * expr;
	long double  value;
};

struct cute_flt_range {
	const char * expr;
	long double  min;
	long double  max;
};

#define CUTE_FLT_RANGE(_min, _max) \
	__CUTE_RANGE(flt, _min, _max)

struct cute_flt_set {
	const char *        expr;
	unsigned int        count;
	const long double * items;
};

#define CUTE_FLT_SET(...) \
	__CUTE_SET(flt, __VA_ARGS__)

struct cute_str {
	const char * expr;
	const char * value;
};

struct cute_str_set {
	const char *  expr;
	unsigned int  count;
	const char ** items;
};

#define CUTE_STR_SET(...) \
	__CUTE_SET(str, __VA_ARGS__)

struct cute_ptr {
	const char * expr;
	const void * value;
};

struct cute_ptr_range {
	const char * expr;
	const void * min;
	const void * max;
};

#define CUTE_PTR_RANGE(_min, _max) \
	__CUTE_RANGE(ptr, _min, _max)

struct cute_ptr_set {
	const char *  expr;
	unsigned int  count;
	const void ** items;
};

#define CUTE_PTR_SET(...) \
	__CUTE_SET(ptr, __VA_ARGS__)

struct cute_mem {
	const char * expr;
	const void * ptr;
	size_t       size;
};

#endif /* _CUTE_TYPES_H */
