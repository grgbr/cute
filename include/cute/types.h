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

/**
 * Signed integer range descriptor.
 *
 * Use cute_sint_range to define a range of signed integers in combination
 * with the #CUTE_SINT_RANGE macro.
 *
 * The range is defined by minimum and maximum signed integer values
 * (inclusive). Suitable value types are one of :
 * - `signed char`,
 * - `signed short`,
 * - `signed int`,
 * - `signed long`,
 * - `signed long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @see
 * - #CUTE_SINT_RANGE
 * - cute_check_sint_range
*/
struct cute_sint_range {
	/** Range description string. */
	const char * expr;
	/** Range's minimum value. */
	intmax_t     min;
	/** Range's maximum value. */
	intmax_t     max;
};

/**
 * Define a signed integer range.
 *
 * @param[in] _min range's minimum value (inclusive)
 * @param[in] _max range's maximum value (inclusive)
 *
 * Initialize a cute_sint_range variable that defines a range of signed
 * integers.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      struct cute_sint_range range = CUTE_SINT_RANGE(-5, 20);
 *
 *      cute_check_sint_range(0,  in,     range);
 *      cute_check_sint_range(10, not_in, CUTE_SINT_RANGE(-5, 5));
 * }
 * @endcode
 *
 * @see
 * - cute_check_sint_range
 * - cute_sint_range
 */
#define CUTE_SINT_RANGE(_min, _max) \
	__CUTE_RANGE(sint, "{" # _min " ... " # _max "}", _min, _max)

/**
 * Signed integer set descriptor.
 *
 * Use cute_sint_set to define a set of signed integers in combination
 * with the #CUTE_SINT_SET macro.
 *
 * The set is defined by an unordered list of signed integer values. Suitable
 * value types are one of :
 * - `signed char`,
 * - `signed short`,
 * - `signed int`,
 * - `signed long`,
 * - `signed long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @see
 * - #CUTE_SINT_SET
 * - cute_check_sint_set
*/
struct cute_sint_set {
	/** Set description string. */
	const char *     expr;
	/** Count of set values stored into cute_sint_set::items */
	unsigned int     count;
	/** Array of set values */
	const intmax_t * items;
};

/**
 * Define a signed integer set.
 *
 * @param[in] ... list of set values
 *
 * Initialize a cute_sint_set variable that defines a set of signed
 * integers.
 *
 * The `...` variable argument list is an unordered list of signed integer
 * values separated by commas that composes the set.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      struct cute_sint_set set = CUTE_SINT_SET(-5, 0, 5);
 *
 *      cute_check_sint_set(0, in,     set);
 *      cute_check_sint_set(7, not_in, CUTE_SINT_SET(-10, -5, 0, 5, 10));
 * }
 * @endcode
 *
 * @see
 * - cute_check_sint_set
 * - cute_sint_set
 */
#define CUTE_SINT_SET(...) \
	__CUTE_SET(sint, "{" # __VA_ARGS__ "}", __VA_ARGS__)

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
	__CUTE_RANGE(uint, "{" # _min " ... " # _max "}", _min, _max)

struct cute_uint_set {
	const char *      expr;
	unsigned int      count;
	const uintmax_t * items;
};

#define CUTE_UINT_SET(...) \
	__CUTE_SET(uint, "{" # __VA_ARGS__ "}", __VA_ARGS__)

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
	__CUTE_RANGE(flt, "{" # _min " ... " # _max "}", _min, _max)

struct cute_flt_set {
	const char *        expr;
	unsigned int        count;
	const long double * items;
};

#define CUTE_FLT_SET(...) \
	__CUTE_SET(flt, "{" # __VA_ARGS__ "}", __VA_ARGS__)

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
	__CUTE_SET(str, "{" # __VA_ARGS__ "}", __VA_ARGS__)

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
	__CUTE_RANGE(ptr, "{" # _min " ... " # _max "}", _min, _max)

struct cute_ptr_set {
	const char *  expr;
	unsigned int  count;
	const void ** items;
};

#define CUTE_PTR_SET(...) \
	__CUTE_SET(ptr, "{" # __VA_ARGS__ "}", __VA_ARGS__)

struct cute_mem {
	const char * expr;
	const void * ptr;
	size_t       size;
};

#endif /* _CUTE_TYPES_H */
