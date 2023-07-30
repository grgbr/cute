/**
 * @file
 * Core public interface
 *
 * @author       Grégor Boirie <gregor.boirie@free.fr>
 * @date         26 Jul 2023
 * @copyright    Copyright (C) 2023 Grégor Boirie.
 * @licensestart GNU Lesser General Public License (LGPL) v3
 *
 * This file is part of cute
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, If not, see <http://www.gnu.org/licenses/>.
 * @licenseend
 */
#ifndef _CUTE_H
#define _CUTE_H

#include <cute/priv/core.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

/******************************************************************************
 * Test case implementation primitives
 ******************************************************************************/

#define cute_skip(_reason) \
	_cute_skip(_reason, __FILE__, __LINE__, __func__)

#define cute_fail(_reason) \
	_cute_fail(_reason, __FILE__, __LINE__, __func__)

/******************************************************************************
 * Common definitions
 ******************************************************************************/

/**
 * Dummy setup fixture.
 *
 * When defining a test or suite, use this to attach an empty / NULL setup
 * @rstsubst{fixture} function.
 *
 * @see #CUTE_INHR_SETUP
 */
#define CUTE_NULL_SETUP CUTE_NULL_FIXTURE

/**
 * Inherited setup fixture.
 *
 * When defining a test or suite, use this to attach a setup @rstsubst{fixture}
 * function inherited from its parent suite.
 *
 * @see #CUTE_NULL_SETUP
 */
#define CUTE_INHR_SETUP CUTE_INHR_FIXTURE

/**
 * Dummy teardown fixture.
 *
 * When defining a test or suite, use this to attach an empty / NULL teardown
 * @rstsubst{fixture} function.
 *
 * @see #CUTE_INHR_TEARDOWN
 */
#define CUTE_NULL_TEARDOWN CUTE_NULL_FIXTURE

/**
 * Inherited teardown fixture.
 *
 * When defining a test or suite, use this to attach a teardown
 * @rstsubst{fixture} function inherited from its parent suite.
 *
 * @see #CUTE_NULL_TEARDOWN
 */
#define CUTE_INHR_TEARDOWN CUTE_INHR_FIXTURE

/**
 * Default test timeout.
 *
 * @rstsubst{Timeout} in seconds assigned to test when not explicitly specified.
 * It is 3 seconds long.
 *
 * @see
 * - #CUTE_INHR_TMOUT
 * - #CUTE_NONE_TMOUT
 */
#define CUTE_DFLT_TMOUT (3U)

/**
 * Inherit timeout from parent suite.
 *
 * When defining a test case, use this to instruct CUTe to assign a
 * @rstsubst{timeout} inherited from a parent test suite.
 *
 * When the test case has no parent suite at running time, #CUTE_DFLT_TMOUT is
 * applied instead.
 *
 * @see
 * - #CUTE_DFLT_TMOUT
 * - #CUTE_NONE_TMOUT
 */
#define CUTE_INHR_TMOUT (UINT_MAX)

/**
 * Everlasting test timeout.
 *
 * When defining a test case, instruct CUTe to disable the test
 * @rstsubst{timeout} mechanism.
 *
 * @see
 * - #CUTE_DFLT_TMOUT
 * - #CUTE_INHR_TMOUT
 */
#define CUTE_NONE_TMOUT (0U)

/******************************************************************************
 * Test case definitions
 ******************************************************************************/

/**
 * Declare a test case
 *
 * @param[in] _name test case name
 *
 * Declare a test case designated by the @p _name variable.
 *
 * Use this in combination with #CUTE_TEST_EXTERN or #CUTE_TEST_DEFN to allow
 * referencing defined test cases from multiple compilation units.
 *
 * **Example**
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_TEST_DECL(sample_test);
 *
 * #endif
 * @endcode
 *
 * @see
 * - #CUTE_TEST_DEFN
 * - #CUTE_TEST_EXTERN
 */
#define CUTE_TEST_DECL(_name) \
	const struct cute_test _name

/**
 * Define a test case with attributes
 *
 * @param[in] _name     test case name
 * @param[in] _exec     test function
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test case designated by the @p _name variable with the specified
 * attributes.
 *
 * @p _exec is a test function that should implement the testing logic. It
 * should be defined according to the following signature:
 * @code
 * void test_exec(void)
 * @endcode
 *
 * @p _setup is a setup() @rstsubst{fixture} function that should initialize the
 * testing context. It is run before @p _exec function and should be defined
 * according to the following signature:
 * @code
 * void test_setup(void)
 * @endcode
 *
 * Values allowed for @p _setup are :
 * - #CUTE_NULL_SETUP,
 * - #CUTE_INHR_SETUP,
 * - or an arbitrary function,
 *
 * @p _teardown is a teardown() @rstsubst{fixture} function that should release
 * the testing context established by @p _setup. It is run if @p _setup has
 * completed and after @p _exec function. It should be defined according to the
 * following signature:
 * @code
 * void test_teardown(void)
 * @endcode
 *
 * Values allowed for @p _teardown are :
 * - #CUTE_NULL_TEARDOWN,
 * - #CUTE_INHR_TEARDOWN,
 * - or an arbitrary function,
 *
 * @p _tmout is a @rstsubst{timeout} expressed in seconds that protects the test
 * run against situations where @p _setup, @p _teardown or @p _exec functions
 * stall.
 * Values allowed for @p _tmout are :
 * - #CUTE_DFLT_TMOUT,
 * - #CUTE_INHR_TMOUT,
 * - #CUTE_NONE_TMOUT,
 * - or a non-zero unsigned integer.
 *
 * @see
 * - #CUTE_TEST
 * - #CUTE_TEST_STATIC
 * - #CUTE_TEST_EXTERN
 * - #CUTE_TEST_DECL
 *
 * **Example**
 * @code
 * static int value;
 *
 * static void sample_setup(void)
 * {
 *      value = 5;
 * }
 *
 * static void sample_test_exec(void)
 * {
 *      cute_check_assert(value == 5);
 * }
 *
 * static CUTE_TEST_DEFN(sample_test,
 *                       sample_test_exec,
 *                       sample_setup,
 *                       CUTE_NULL_TEARDOWN,
 *                       CUTE_DFLT_TMOUT);
 * @endcode
 */
#define CUTE_TEST_DEFN(_name, _exec, _setup, _teardown, _tmout) \
	const struct cute_test _name = CUTE_TEST_INIT(# _name, \
		                                      _exec, \
		                                      _setup, \
		                                      _teardown, \
		                                      _tmout)

/**
 * Define a static file scoped test case with attributes
 *
 * @param[in] _name     test case name
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test case designated by the @p _name variable with static global
 * file scope. The test case is created with attributes given in argument as
 * explained below.
 *
 * In addition, #CUTE_TEST_STATIC must be immediately followed by a block of
 * instructions defining the related testing logic as stated in #CUTE_TEST_DEFN.
 *
 * @p _setup is a setup() @rstsubst{fixture} function as described in
 * #CUTE_TEST_DEFN.  @p _teardown is a teardown() @rstsubst{fixture} function as
 * described in #CUTE_TEST_DEFN.
 * @p _tmout is a @rstsubst{timeout} expressed in seconds as described in
 * #CUTE_TEST_DEFN.
 *
 * @see
 * - #CUTE_TEST
 * - #CUTE_TEST_DEFN
 * - #CUTE_TEST_EXTERN
 *
 * **Example**
 * @code
 * CUTE_TEST_STATIC(sample_test,
 *                  CUTE_NULL_SETUP,
 *                  CUTE_NULL_TEARDOWN,
 *                  CUTE_DFLT_TMOUT)
 * {
 *      cute_check_assert(0 == 0);
 * }
 * @endcode
 */
#define CUTE_TEST_STATIC(_name, _setup, _teardown, _tmout) \
	static void _name ## __cute_exec(void); \
	static CUTE_TEST_DEFN(_name, \
	                      _name ## __cute_exec, \
	                      _setup, \
	                      _teardown, \
	                      _tmout); \
	static void _name ## __cute_exec(void)

/**
 * Define a test case with attributes and external linkage
 *
 * @param[in] _name     test case name
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test case designated by the @p _name variable with default global
 * file scope, i.e. with external linkage. The test case is created with
 * attributes given in argument as explained below.
 *
 * In addition, #CUTE_TEST_EXTERN must be immediately followed by a block of
 * instructions defining the related testing logic as stated in #CUTE_TEST_DEFN.
 *
 * @p _setup is a setup() @rstsubst{fixture} function as described in
 * #CUTE_TEST_DEFN.  @p _teardown is a teardown() @rstsubst{fixture} function as
 * described in #CUTE_TEST_DEFN.
 * @p _tmout is a @rstsubst{timeout} expressed in seconds as described in
 * #CUTE_TEST_DEFN.
 *
 * Use #CUTE_TEST_DECL to produce a declaration in a header file so that the
 * created test case may be referenced from other compilation units.
 *
 * @see
 * - #CUTE_TEST
 * - #CUTE_TEST_DEFN
 * - #CUTE_TEST_DECL
 * - #CUTE_TEST_STATIC
 *
 * **Example definition**
 * @code
 * #include "sample_test.h"
 *
 * CUTE_TEST_EXTERN(sample_test,
 *                  CUTE_NULL_SETUP,
 *                  CUTE_NULL_TEARDOWN,
 *                  CUTE_DFLT_TMOUT)
 * {
 *      cute_check_assert(0 == 0);
 * }
 * @endcode
 *
 * **Example declaration** in a sample_test.h header file
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_TEST_DECL(sample_test);
 *
 * #endif
 * @endcode
 */
#define CUTE_TEST_EXTERN(_name, _setup, _teardown, _tmout) \
	static void _name ## __cute_exec(void); \
	CUTE_TEST_DEFN(_name, \
	               _name ## __cute_exec, \
	               _setup, \
	               _teardown, \
	               _tmout); \
	static void _name ## __cute_exec(void)

/**
 * Define a test case with default attributes and class storage
 *
 * @param[in] _name test case name
 *
 * Define a test case designated by the @p _name variable. #CUTE_TEST must be
 * immediately followed by a block of instructions defining the related testing
 * logic as stated in #CUTE_TEST_DEFN.
 *
 * The @p _name test case variable is defined with default class storage
 * specifier, i.e. static global file scope.
 *
 * The test case is created so that it inherits setup() and teardown()
 * @rstsubst{fixture} functions from its parent suite when it exists. It is
 * attached dummy / null @rstsubst{fixture} functions otherwise.
 *
 * Similarly, it inherits @rstsubst{timeout} settings from its parent suite when
 * present.  #CUTE_DFLT_TMOUT default @rstsubst{timeout} is assigned to it
 * otherwise.
 *
 * @see
 * - #CUTE_TEST_STATIC
 * - #CUTE_TEST_EXTERN
 * - #CUTE_TEST_DEFN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test)
 * {
 *      cute_check_assert(0 == 0);
 * }
 * @endcode
 */
#define CUTE_TEST(_name) \
	CUTE_TEST_STATIC(_name, \
	                 CUTE_INHR_SETUP, \
	                 CUTE_INHR_TEARDOWN, \
	                 CUTE_INHR_TMOUT)

/******************************************************************************
 * Test group definitions
 ******************************************************************************/

/**
 * Return a pointer to a test case or suite.
 *
 * @param[in] _name test or suite name
 *
 * Use #CUTE_REF to define collections of test cases or suites when initializing
 * test groups.
 *
 * @see
 * - #CUTE_GROUP
 * - #CUTE_GROUP_STATIC
 * - #CUTE_GROUP_EXTERN
 * - #CUTE_GROUP_DEFN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_subtest)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_GROUP(sample_subgroup) = {
 *      CUTE_REF(sample_subtest)
 * };
 *
 * CUTE_SUITE(sample_subsuite, sample_subgroup);
 *
 * CUTE_TEST(sample_test)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_subsuite),
 *      CUTE_REF(sample_test)
 * };
 *
 * CUTE_SUITE(sample_suite, sample_group);
 * @endcode
 */
#define CUTE_REF(_name) \
	(&(_name).super)

/**
 * Declare a test group
 *
 * @param[in] _name test group name
 *
 * Declare a test group designated by the @p _name variable that may later be
 * used to define suites.
 *
 * Use this in combination with #CUTE_GROUP_EXTERN or #CUTE_GROUP_DEFN to allow
 * referencing defined test group from multiple compilation units.
 *
 * **Example**
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_GROUP_DECL(sample_group);
 *
 * #endif
 * @endcode
 *
 * @see
 * - #CUTE_GROUP_DEFN
 * - #CUTE_GROUP_EXTERN
 */
#define CUTE_GROUP_DECL(_name) \
	const struct cute_base * const * const _name

/**
 * Define a test group
 *
 * @param[in] _name test group name
 *
 * Define a test group designated by the @p _name variable that may later be
 * used to define suites.
 *
 * #CUTE_GROUP_DEFN must be immediately followed by an initializer. It should
 * define the collection of tests (and / or suites) that composes the @p _name
 * group thanks to the #CUTE_REF macro as shown in the example below.
 *
 * @see
 * - #CUTE_REF
 * - #CUTE_GROUP
 * - #CUTE_GROUP_STATIC
 * - #CUTE_GROUP_EXTERN
 * - #CUTE_GROUP_DECL
 * - #CUTE_SUITE
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * static CUTE_GROUP_DEFN(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE(sample_suite, sample_group);
 * @endcode
 */
#define CUTE_GROUP_DEFN(_name) \
	const struct cute_base * const _name[]

/**
 * Define a static file scoped test group
 *
 * @param[in] _name test group name
 *
 * Define a test group designated by the @p _name variable that may later be
 * used to define suites.
 * The @p _name test group variable is defined with static global file scope.
 *
 * #CUTE_GROUP_STATIC must be immediately followed by an initializer as stated
 * in #CUTE_GROUP_DEFN.
 *
 * @see
 * - #CUTE_REF
 * - #CUTE_GROUP
 * - #CUTE_GROUP_EXTERN
 * - #CUTE_GROUP_DEFN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * CUTE_GROUP_STATIC(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE(sample_suite, sample_group);
 * @endcode
 */
#define CUTE_GROUP_STATIC(_name) \
	static CUTE_GROUP_DEFN(_name)

/**
 * Define a test group with external linkage
 *
 * @param[in] _name test group name
 *
 * Define a test group designated by the @p _name variable that may later be
 * used to define suites.
 * The @p _name test group variable is defined with default global file scope,
 * i.e., with external linkage.
 *
 * #CUTE_GROUP_EXTERN must be immediately followed by an initializer as stated
 * in #CUTE_GROUP_DEFN.
 *
 * Use #CUTE_GROUP_DECL to produce a declaration in a header file so that the
 * created test group may be referenced from other compilation units.
 *
 * @see
 * - #CUTE_REF
 * - #CUTE_GROUP
 * - #CUTE_GROUP_STATIC
 * - #CUTE_GROUP_DEFN
 * - #CUTE_GROUP_DECL
 *
 * **Example definition**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * CUTE_GROUP_EXTERN(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 * @endcode
 *
 * **Example declaration** in a sample_test.h header file
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_GROUP_DECL(sample_group);
 *
 * #endif
 * @endcode
 */
#define CUTE_GROUP_EXTERN(_name) \
	CUTE_GROUP_DEFN(_name)

/**
 * Define a test group with default class storage
 *
 * @param[in] _name test group name
 *
 * Define a test group designated by the @p _name variable that may later be
 * used to define suites.
 * The @p _name test group variable is defined with default class storage
 * specifier, i.e., static global file scope.
 *
 * #CUTE_GROUP must be immediately followed by an initializer as stated in
 * #CUTE_GROUP_DEFN.
 *
 * @see
 * - #CUTE_REF
 * - #CUTE_GROUP_STATIC
 * - #CUTE_GROUP_EXTERN
 * - #CUTE_GROUP_DEFN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE(sample_suite, sample_group);
 * @endcode
 */
#define CUTE_GROUP(_name) \
	CUTE_GROUP_STATIC(_name)

/******************************************************************************
 * Test suite definitions
 ******************************************************************************/

/**
 * Declare a test suite
 *
 * @param[in] _name test suite name
 *
 * Declare a test suite designated by the @p _name variable.
 *
 * Use this in combination with #CUTE_SUITE_EXTERN or #CUTE_SUITE_DEFN to allow
 * referencing defined test suite from multiple compilation units.
 *
 * **Example**
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_SUITE_DECL(sample_suite);
 *
 * #endif
 * @endcode
 *
 * @see
 * - #CUTE_SUITE_DEFN
 * - #CUTE_SUITE_EXTERN
 */
#define CUTE_SUITE_DECL(_name) \
	const struct cute_suite _name

/**
 * Define a test suite with attributes
 *
 * @param[in] _name     test suite name
 * @param[in] _group    test group
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test suite designated by the @p _name variable with the specified
 * attributes.
 *
 * @p _group is an ordered collection of test cases and / or suites defined
 * using one of :
 * - #CUTE_GROUP,
 * - #CUTE_GROUP_STATIC,
 * - #CUTE_GROUP_EXTERN,
 * - or #CUTE_GROUP_DEFN.
 *
 * @p _setup is a setup() @rstsubst{fixture} function that should initialize the
 * runtime context of aggregated sub-tests.
 *
 * @p _teardown is a teardown() @rstsubst{fixture} function that should release
 * the runtime context established by @p _setup.
 *
 * @p _tmout is a @rstsubst{timeout} expressed in seconds that protects against
 * situations where aggregated sub-tests runs hang.
 *
 * See #CUTE_TEST_DEFN for more informations about @rstsubst{fixture} functions
 * and @rstsubst{timeout}.
 *
 * @see
 * - #CUTE_SUITE
 * - #CUTE_SUITE_STATIC
 * - #CUTE_SUITE_EXTERN
 * - #CUTE_SUITE_DECL
 * - #CUTE_TEST_DEFN
 *
 * **Example**
 * @code
 * static int value;
 *
 * static void sample_setup(void)
 * {
 *      value = 5;
 * }
 *
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(value > 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(value == 5);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * static CUTE_SUITE_DEFN(sample_suite,
 *                        sample_group,
 *                        sample_setup,
 *                        CUTE_NULL_TEARDOWN,
 *                        CUTE_DFLT_TMOUT);
 * @endcode
 */
#define CUTE_SUITE_DEFN(_name, _group, _setup, _teardown, _tmout) \
	const struct cute_suite _name = CUTE_SUITE_INIT(# _name, \
	                                                _group, \
	                                                _setup, \
	                                                _teardown, \
	                                                _tmout)

/**
 * Define a static file scoped test suite with attributes
 *
 * @param[in] _name     test suite name
 * @param[in] _group    test group
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test suite designated by the @p _name variable with static global
 * file scope. The test suite is created with attributes given in argument as
 * explained in #CUTE_SUITE_DEFN.
 *
 * @see
 * - #CUTE_SUITE
 * - #CUTE_SUITE_DEFN
 * - #CUTE_SUITE_EXTERN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(value > 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(value == 5);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE_STATIC(sample_suite,
 *                   sample_group,
 *                   CUTE_NULL_SETUP,
 *                   CUTE_NULL_TEARDOWN,
 *                   CUTE_DFLT_TMOUT);
 * @endcode
 */
#define CUTE_SUITE_STATIC(_name, _group, _setup, _teardown, _tmout) \
	static CUTE_SUITE_DEFN(_name, \
	                       _group, \
	                       _setup, \
	                       _teardown, \
	                       _tmout)

/**
 * Define a test suite with attributes and external linkage.
 *
 * @param[in] _name     test suite name
 * @param[in] _group    test group
 * @param[in] _setup    setup fixture function
 * @param[in] _teardown teardown fixture function
 * @param[in] _tmout    test timeout
 *
 * Define a test suite designated by the @p _name variable with default global
 * file scope, i.e. with external linkage.
 *
 * The test suite is created with attributes given in argument as explained in
 * #CUTE_SUITE_DEFN.
 *
 * Use #CUTE_SUITE_DECL to produce a declaration in a header file so that the
 * created test suite may be referenced from other compilation units.
 *
 * @see
 * - #CUTE_SUITE_DEFN
 * - #CUTE_SUITE_DECL
 * - #CUTE_SUITE
 * - #CUTE_SUITE_STATIC
 *
 * **Example definition**
 * @code
 * #include "sample_test.h"
 *
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(value > 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(value == 5);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE_EXTERN(sample_suite,
 *                   sample_group,
 *                   CUTE_INHR_SETUP,
 *                   CUTE_INHR_TEARDOWN,
 *                   CUTE_INHR_TMOUT);
 * @endcode
 *
 * **Example declaration** in a sample_test.h header file
 * @code
 * #ifndef _SAMPLE_TEST_H
 * #define _SAMPLE_TEST_H
 *
 * #include <cute/cute.h>
 *
 * extern CUTE_SUITE_DECL(sample_suite);
 *
 * #endif
 * @endcode

 */
#define CUTE_SUITE_EXTERN(_name, _group, _setup, _teardown, _tmout) \
	CUTE_SUITE_DEFN(_name, \
	                _group, \
	                _setup, \
	                _teardown, \
	                _tmout)

/**
 * Define a test suite with default class storage.
 *
 * @param[in] _name  test suite name
 * @param[in] _group test group
 *
 * Define a test suite designated by the @p _name variable with default class
 * storage specifier, i.e. static global file scope.
 *
 * @p _group is an ordered collection of test cases and / or suites as stated in
 * #CUTE_SUITE_DEFN.
 *
 * The test suite is created so that it inherits setup() and teardown()
 * @rstsubst{fixture} functions from its parent suite when it exists. It is
 * attached dummy / null @rstsubst{fixture} functions otherwise.
 *
 * Similarly, it inherits @rstsubst{timeout} settings from its parent suite when
 * present.  #CUTE_DFLT_TMOUT default @rstsubst{timeout} is assigned to it
 * otherwise.
 *
 * @see
 * - #CUTE_SUITE_STATIC
 * - #CUTE_SUITE_EXTERN
 * - #CUTE_SUITE_DEFN
 *
 * **Example**
 * @code
 * CUTE_TEST(sample_test_0)
 * {
 *      cute_check_assert(0 == 0);
 * }
 *
 * CUTE_TEST(sample_test_1)
 * {
 *      cute_check_assert(1 == 1);
 * }
 *
 * CUTE_GROUP(sample_group) = {
 *      CUTE_REF(sample_test_0),
 *      CUTE_REF(sample_test_1)
 * };
 *
 * CUTE_SUITE(sample_suite, sample_group);
 * @endcode
 */
#define CUTE_SUITE(_name, _group) \
	CUTE_SUITE_STATIC(_name, \
	                  _group, \
	                  CUTE_INHR_SETUP, \
	                  CUTE_INHR_TEARDOWN, \
	                  CUTE_INHR_TMOUT)

extern void
cute_show_suite(const struct cute_suite *  suite) __cute_export;

extern int
cute_run_suite(const struct cute_suite * suite) __cute_export;

/******************************************************************************
 * Top-level definitions
 ******************************************************************************/

enum cute_config_report {
	CUTE_CONFIG_SILENT_REPORT = (1U << 0),
	CUTE_CONFIG_TERSE_REPORT  = (1U << 1),
	CUTE_CONFIG_VERB_REPORT   = (1U << 2),
	CUTE_CONFIG_TAP_REPORT    = (1U << 3),
	CUTE_CONFIG_XML_REPORT    = (1U << 4)
};

enum cute_config_tty {
	CUTE_CONFIG_FALSE_TTY  = 0,
	CUTE_CONFIG_TRUE_TTY   = 1,
	CUTE_CONFIG_PROBE_TTY,
	CUTE_CONFIG_TTY_NR
};

struct cute_config {
	const char *         match;
	bool                 debug;
	bool                 icase;
	unsigned int         reports;
	enum cute_config_tty tty;
	const char *         tap_path;
	const char *         xml_path;
};

#define CUTE_CONFIG_INIT \
	{ \
		.match    = NULL, \
		.debug    = false, \
		.icase    = false, \
		.reports  = 0, \
		.tty      = CUTE_CONFIG_PROBE_TTY, \
		.tap_path = NULL, \
		.xml_path = NULL \
	}

extern int
cute_init(struct cute_config * config,
          const char *         package,
          const char *         version) __cute_export;

extern void
cute_fini(void) __cute_export;

extern int
cute_main(int                       argc,
          char * const              argv[],
          const struct cute_suite * suite,
          const char *              package,
          const char *              version) __cute_export;

#define CUTE_MAIN(_root, _package, _version) \
	int main(int argc, char * const argv[]) \
	{ \
		return cute_main(argc, argv, &(_root), _package, _version); \
	}

#endif /* _CUTE_H */
