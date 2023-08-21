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
 *      const struct cute_sint_range range = CUTE_SINT_RANGE(-5, 20);
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
 *      const struct cute_sint_set set = CUTE_SINT_SET(-5, 0, 5);
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

/**
 * Unsigned integer range descriptor.
 *
 * Use cute_uint_range to define a range of unsigned integers in combination
 * with the #CUTE_UINT_RANGE macro.
 *
 * The range is defined by minimum and maximum unsigned integer values
 * (inclusive). Suitable value types are one of :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @see
 * - #CUTE_UINT_RANGE
 * - cute_check_uint_range
*/
struct cute_uint_range {
	/** Range description string. */
	const char * expr;
	/** Range's minimum value. */
	uintmax_t    min;
	/** Range's maximum value. */
	uintmax_t    max;
};

/**
 * Define a unsigned integer range.
 *
 * @param[in] _min range's minimum value (inclusive)
 * @param[in] _max range's maximum value (inclusive)
 *
 * Initialize a cute_uint_range variable that defines a range of unsigned
 * integers.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_range range = CUTE_UINT_RANGE(-5, 20);
 *
 *      cute_check_uint_range(0,  in,     range);
 *      cute_check_uint_range(10, not_in, CUTE_UINT_RANGE(-5, 5));
 * }
 * @endcode
 *
 * @see
 * - cute_check_uint_range
 * - cute_uint_range
 */
#define CUTE_UINT_RANGE(_min, _max) \
	__CUTE_RANGE(uint, "{" # _min " ... " # _max "}", _min, _max)

/**
 * Unsigned integer set descriptor.
 *
 * Use cute_uint_set to define a set of unsigned integers in combination
 * with the #CUTE_UINT_SET macro.
 *
 * The set is defined by an unordered list of unsigned integer values. Suitable
 * value types are one of :
 * - `unsigned char`,
 * - `unsigned short`,
 * - `unsigned int`,
 * - `unsigned long`,
 * - `unsigned long long`,
 * - or equivalent *typedef*'ed types.
 *
 * @see
 * - #CUTE_UINT_SET
 * - cute_check_uint_set
 */
struct cute_uint_set {
	/** Set description string. */
	const char *      expr;
	/** Count of set values stored into cute_uint_set::items */
	unsigned int      count;
	/** Array of set values */
	const uintmax_t * items;
};

/**
 * Define a unsigned integer set.
 *
 * @param[in] ... list of set values
 *
 * Initialize a cute_uint_set variable that defines a set of unsigned
 * integers.
 *
 * The `...` variable argument list is an unordered list of unsigned integer
 * values separated by commas that composes the set.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_uint_set set = CUTE_UINT_SET(-5, 0, 5);
 *
 *      cute_check_uint_set(0, in,     set);
 *      cute_check_uint_set(7, not_in, CUTE_UINT_SET(-10, -5, 0, 5, 10));
 * }
 * @endcode
 *
 * @see
 * - cute_check_uint_set
 * - cute_uint_set
 */
#define CUTE_UINT_SET(...) \
	__CUTE_SET(uint, "{" # __VA_ARGS__ "}", __VA_ARGS__)

struct cute_flt {
	const char * expr;
	long double  value;
};

/**
 * Floating point number range descriptor.
 *
 * Use cute_flt_range to define a range of floating point numbers in combination
 * with the #CUTE_FLT_RANGE macro.
 *
 * The range is defined by minimum and maximum floating point values
 * (inclusive). Suitable value types are one of :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or equivalent *typedef*'ed types.
 *
 * @note
 * Quad-precision floating point numbers (`__float128`) are not yet supported.
 *
 * @see
 * - #CUTE_FLT_RANGE
 * - cute_check_flt_range
*/
struct cute_flt_range {
	/** Range description string. */
	const char * expr;
	/** Range's minimum value. */
	long double  min;
	/** Range's maximum value. */
	long double  max;
};

/**
 * Define a floating point number range.
 *
 * @param[in] _min range's minimum value (inclusive)
 * @param[in] _max range's maximum value (inclusive)
 *
 * Initialize a cute_flt_range variable that defines a range of floating point
 * numbers.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_range range = CUTE_FLT_RANGE(-5.5f, 20.5f);
 *
 *      cute_check_flt_range(0.5f, in,     range);
 *      cute_check_flt_range(10,   not_in, CUTE_FLT_RANGE(-5.5, 5.5));
 * }
 * @endcode
 *
 * @see
 * - cute_check_flt_range
 * - cute_flt_range
 */
#define CUTE_FLT_RANGE(_min, _max) \
	__CUTE_RANGE(flt, "{" # _min " ... " # _max "}", _min, _max)

/**
 * Floating point number set descriptor.
 *
 * Use cute_flt_set to define a set of floating point numbers in combination
 * with the #CUTE_FLT_SET macro.
 *
 * The set is defined by an unordered list of floating point values. Suitable
 * value types are one of :
 * - `float`,
 * - `double`,
 * - `long double`,
 * - or equivalent *typedef*'ed types.
 *
 * @note
 * Quad-precision floating point numbers (`__float128`) are not yet supported.
 *
 * @see
 * - #CUTE_FLT_SET
 * - cute_check_flt_set
 */
struct cute_flt_set {
	/** Set description string. */
	const char *        expr;
	/** Count of set values stored into cute_flt_set::items */
	unsigned int        count;
	/** Array of set values */
	const long double * items;
};

/**
 * Define a floating point number set.
 *
 * @param[in] ... list of set values
 *
 * Initialize a cute_flt_set variable that defines a set of floating point
 * numbers.
 *
 * The `...` variable argument list is an unordered list of floating point
 * values separated by commas that composes the set.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_flt_set set = CUTE_FLT_SET(-5.5f, 0.5f, 5.5f);
 *
 *      cute_check_flt_set(0.5f, in,     set);
 *      cute_check_flt_set(7,    not_in, CUTE_FLT_SET(-10.5, 0.5, 10.0));
 * }
 * @endcode
 *
 * @see
 * - cute_check_flt_set
 * - cute_flt_set
 */
#define CUTE_FLT_SET(...) \
	__CUTE_SET(flt, "{" # __VA_ARGS__ "}", __VA_ARGS__)

struct cute_str {
	const char * expr;
	const char * value;
};

/**
 * String content set descriptor.
 *
 * Use cute_str_set to define a set of strings in combination
 * with the #CUTE_STR_SET macro.
 *
 * The set is defined by an unordered list of `NULL` terminated C strings.
 *
 * @see
 * - #CUTE_STR_SET
 * - cute_check_str_set
 */
struct cute_str_set {
	/** Set description string. */
	const char *  expr;
	/** Count of set values stored into cute_str_set::items */
	unsigned int  count;
	/** Array of set values */
	const char ** items;
};

/**
 * Define set of strings.
 *
 * @param[in] ... list of set strings
 *
 * Initialize a cute_str_set variable that defines a set of strings.
 *
 * The `...` variable argument list is an unordered list of `NULL` terminated C
 * strings separated by commas that composes the set.
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(mytest)
 * {
 *      const struct cute_str_set set = CUTE_STR_SET("Lorem", "ipsum", "dolor");
 *
 *      cute_check_str_set("ipsum", in,     set);
 *      cute_check_str_set("Lorem", not_in, CUTE_FLT_SET("amet, ",
 *                                                       "consectetur");
 * }
 * @endcode
 *
 * @see
 * - cute_check_str_set
 * - cute_str_set
 */
#define CUTE_STR_SET(...) \
	__CUTE_SET(str, "{" # __VA_ARGS__ "}", __VA_ARGS__)

struct cute_ptr {
	const char * expr;
	const void * value;
};

struct cute_ptr_range {
	/** Range description string. */
	const char * expr;
	/** Range's minimum value. */
	const void * min;
	/** Range's maximum value. */
	const void * max;
};

#define CUTE_PTR_RANGE(_min, _max) \
	__CUTE_RANGE(ptr, "{" # _min " ... " # _max "}", _min, _max)

struct cute_ptr_set {
	/** Set description string. */
	const char *  expr;
	/** Count of set values stored into cute_ptr_set::items */
	unsigned int  count;
	/** Array of set values */
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
