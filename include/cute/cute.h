/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

/**
 * @file
 * Core public interface
 *
 * @author    Grégor Boirie <gregor.boirie@free.fr>
 * @date      26 Jul 2023
 * @copyright Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 * @license   [GNU Lesser General Public License (LGPL) v3]
 *            (https://www.gnu.org/licenses/lgpl+gpl-3.0.txt)
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

/**
 * Skip current test.
 *
 * @param[in] _reason reason description string
 *
 * Stop running current @rstsubst{test case} or @rstsubst{suite} and mark it as
 * @rstsubst{skipped}.
 *
 * When non `NULL`, the @p _reason argument should be a string describing the
 * reason why the test has been skipped.
 *
 * See @rstsubst{report} section for more informations about possible test
 * @rstsubst{hierarchy} outcomes.
 *
 * @see cute_fail()
 *
 * **Example**
 * @code{.c}
 * #if defined(CONFIG_FEATURE_ENABLED)
 * CUTE_TEST(feature_test)
 * {
 *      ...
 * }
 * #else
 * CUTE_TEST(feature_test)
 * {
 *      cute_skip("feature disabled");
 * }
 * #endif
 * @endcode
 */
#define cute_skip(_reason) \
	_cute_skip(_reason, __FILE__, __LINE__, __func__)

/**
 * Mark current test as failed.
 *
 * @param[in] _reason reason description string
 *
 * Stop running current @rstsubst{test case} or @rstsubst{suite} and mark it as
 * @rstsubst{failed}.
 *
 * When non `NULL`, the @p _reason argument should be a string describing the
 * reason why the test has failed.
 *
 * See @rstsubst{report} section for more informations about possible test
 * @rstsubst{hierarchy} outcomes.
 *
 * @see cute_skip()
 *
 * **Example**
 * @code{.c}
 * CUTE_TEST(sample_test)
 * {
 *      cute_fail(NULL);
 * }
 * @endcode
 */
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
 * @code{.h}
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
 * @code{.c}
 * void test_exec(void)
 * @endcode
 *
 * @p _setup is a setup() @rstsubst{fixture} function that should initialize the
 * testing context. It is run before @p _exec function and should be defined
 * according to the following signature:
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.h}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.h}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.h}
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
 * @code{.c}
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
 * @code{.h}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.c}
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
 * @code{.h}
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
 * @code{.c}
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

/**
 * Display informations of a test suite.
 *
 * @param[in] suite suite to run
 *
 * Show informations related to the test @rstsubst{hierarchy} which top-level
 * @rstsubst{suite} is given in argument.
 * Informations are displayed according to cute_config::reports setting passed
 * as argument to cute_init().
 *
 * @see
 * - cute_run_suite()
 * - cute_init()
 */
extern void
cute_show_suite(const struct cute_suite *  suite) __cute_export;

/**
 * Run a test suite.
 *
 * @param[in] suite suite to run
 *
 * @return 0 if successful, an errno-like error code otherwise.
 *
 * Run a test @rstsubst{hierarchy} which top-level @rstsubst{suite} is given in
 * argument. Run is carried out according to cute_config configuration passed as
 * argument to cute_init().
 *
 * cute_run_suite() returns an error when at least one child
 * @rstsubst{test case} or @rstsubst{suite} has failed or crashed.
 * See @rstsubst{report} section for more informations about possible test
 * @rstsubst{hierarchy} outcomes.
 *
 * @see
 * - cute_show_suite()
 * - cute_init()
 */
extern int
cute_run_suite(const struct cute_suite * suite) __cute_export;

/******************************************************************************
 * Top-level definitions
 ******************************************************************************/

/**
 * Report type selector.
 *
 * Select a particular test @rstsubst{report} format.
 *
 * @see cute_config::reports
 */
enum cute_config_report {
	/** Completly mute console report. */
	CUTE_CONFIG_SILENT_REPORT = (1U << 0),
	/** Enable minimalist console report. */
	CUTE_CONFIG_TERSE_REPORT  = (1U << 1),
	/** Enable verbose console report. */
	CUTE_CONFIG_VERB_REPORT   = (1U << 2),
	/** Enable Test Anything Protocol report. */
	CUTE_CONFIG_TAP_REPORT    = (1U << 3),
	/** Enable JUnit XML report. */
	CUTE_CONFIG_XML_REPORT    = (1U << 4)
};

/**
 * Console terminal probing strategy selector.
 *
 * Select the strategy used to probe the underlying console terminal. Probing
 * is needed to setup console output colorization.
 *
 * @see cute_config::tty
 */
enum cute_config_tty {
	/** Disable console terminal probing and turn colorization off. */
	CUTE_CONFIG_FALSE_TTY  = 0,
	/** Disable console terminal probing and turn colorization on. */
	CUTE_CONFIG_TRUE_TTY   = 1,
	/** Enable console terminal probing to autodetect colorization support. */
	CUTE_CONFIG_PROBE_TTY,
	/** @internal */
	CUTE_CONFIG_TTY_NR
};

/**
 * Test run configuration.
 *
 * cute_config is a structure that allows to configure the way a test
 * @rstsubst{hierarchy} is run.
 *
 * @see
 * - #CUTE_CONFIG_INIT
 * - cute_init()
 */
struct cute_config {
	/**
	 * Test hierarchy matching regular expression.
	 *
	 * Allows to select a subset of the test @rstsubst{hierarchy} to run.
	 *
	 * At run initialization time, each node of the test
	 * @rstsubst{hierarchy} is compared against the cute_config::match
	 * regular expression and selected for running if matched. Nodes for
	 * which the match failed are disabled.
	 *
	 * cute_config::match must be specified as a POSIX extended regular
	 * expression as described in @man{regex(7)}. cute_config::icase
	 * alters the matching process.
	 *
	 * cute_config::match may also be specified as ``NULL`` in which case,
	 * no matching is performed, meaning that the complete test
	 * @rstsubst{hierarchy} is selected for running.
	 *
	 * @see cute_config::icase
	 */
	const char * match;
	/**
	 * Ignore case when matching test hierarchy nodes.
	 *
	 * When matching a test @rstsubst{hierarchy} node thanks to
	 * cute_config::match, ignore case distinctions.
	 *
	 * cute_config::icase is ignore when cute_config::match is ``NULL``.
	 *
	 * @see cute_config::match
	 */
	bool icase;
	/**
	 * Enable test hierarchy running debug mode.
	 *
	 * Enable cute_config::debug to ease the debugging of a test
	 * |hierarchy|.
	 *
	 * When enabled, exception handling and timeouts are disabled so that a
	 * debugging session is not disturbed by an unexpected
	 * @rstref{signal <user-signals>} and / or @rstsubst{timer} expiry.
	 */
	bool debug;
	/**
	 * Test report selector mask.
	 *
	 * Request that test results be output according to formats specified by
	 * @rstsubst{report}.
	 *
	 * cute_config::reports must be specified as a mask of
	 * cute_config_report values according to the **following
	 * restrictions** :
	 *
	 * - #CUTE_CONFIG_SILENT_REPORT, #CUTE_CONFIG_TERSE_REPORT, and
	 *   #CUTE_CONFIG_VERB_REPORT are exclusive and cannot be simultaneously
	 *   set within the mask ;
	 * - when cute_config::tap_path is ``NULL``, #CUTE_CONFIG_TAP_REPORT
	 *   cannot be simultaneously set with #CUTE_CONFIG_SILENT_REPORT,
	 *   #CUTE_CONFIG_TERSE_REPORT or #CUTE_CONFIG_VERB_REPORT ;
	 * - when cute_config::xml_path is ``NULL``, #CUTE_CONFIG_XML_REPORT
	 *   cannot be simultaneously set with #CUTE_CONFIG_SILENT_REPORT,
	 *   #CUTE_CONFIG_TERSE_REPORT or #CUTE_CONFIG_VERB_REPORT.
	 *
	 * @see
	 * - cute_config_report
	 * - cute_config::tap_path
	 * - cute_config::xml_path
	 */
	unsigned int reports;
	/**
	* Console terminal probing strategy selector.
	 *
	 * Select the strategy used to probe the underlying console terminal for
	 * output colorization support.
	 *
	 * @see cute_config_tty
	 */
	enum cute_config_tty tty;
	/**
	 * TAP report pathname.
	 *
	 * Pathname to the file into which
	 * @rstref{Test Anything Protocol report <sect-user-tap>} output is
	 * stored.
	 *
	 * cute_config::tap_path is ignored if #CUTE_CONFIG_TAP_REPORT is
	 * cleared within cute_config::reports mask.
	 *
	 * Setting cute_config::tap_path to ``NULL`` requests that @rstref{TAP
	 * report <sect-user-junit>} output be redirected to standard output
	 * (but see cute_config::reports for restrictions).
	 *
	 * @see cute_config::reports
	 */
	const char * tap_path;
	/**
	 * JUnit XML report pathname.
	 *
	 * Pathname to the file into which
	 * @rstref{JUnit XML report <sect-user-junit>} output is stored.
	 *
	 * cute_config::xml_path is ignored if #CUTE_CONFIG_XML_REPORT is
	 * cleared within cute_config::reports mask.
	 *
	 * Setting cute_config::xml_path to ``NULL`` requests that
	 * @rstref{JUnit XML report <sect-user-junit>} output be redirected to
	 * standard output (but see cute_config::reports for restrictions).
	 *
	 * @see cute_config::reports
	 */
	const char * xml_path;
};

/**
 * Test run configuration default initializer.
 *
 * Initialize a cute_config with default values such that a test
 * @rstsubst{hierarchy} runner behaves as following :
 *
 * - @rstsubst{test case} and @rstsubst{suite} nodes are all enabled,
 * - debug mode is off,
 * - #CUTE_CONFIG_TERSE_REPORT is enabled,
 * - #CUTE_CONFIG_PROBE_TTY is enabled.
 */
#define CUTE_CONFIG_INIT \
	{ \
		.match    = NULL, \
		.icase    = false, \
		.debug    = false, \
		.reports  = 0, \
		.tty      = CUTE_CONFIG_PROBE_TTY, \
		.tap_path = NULL, \
		.xml_path = NULL \
	}

/**
 * Initialize CUTe API.
 *
 * @param[inout] config  test hierarchy run configuration
 * @param[in]    package package name string
 * @param[in]    version package version string
 *
 * @return 0 if successful, an errno-like error code otherwise.
 *
 * cute_init() initializes @rstsubst{API} internals and configure the way a
 * test @rstsubst{hierarchy} runs and outputs @rstsubst{report}.
 *
 * @p config argument configures the internal test @rstsubst{hierarchy} runner.
 * @p package should be given as a string containing the name of the package
 * for which testing code is written.
 * @p version should hold the version string identifying the @p package
 * package revision.
 * Both @p package and @p version are optional and may be specified as
 * ``NULL``.
 *
 * After a successful call to cute_init() and once no more @rstsubst{API} call
 * is required, cute_fini() **must** be called to release resources allocated
 * internally.
 *
 * @see
 * - cute_config
 * - cute_fini()
 * - #CUTE_MAIN
 */
extern int
cute_init(struct cute_config * config,
          const char *         package,
          const char *         version) __cute_export;

/**
 * Close CUTe API.
 *
 * Flush / close test @rstsubst{hierarchy} reporting I/O streams and release
 * @rstsubst{API} resources allocated internally.
 *
 * Any @rstsubst{API} usage made after a call to cute_fini() will lead to
 * unexpected results.
 *
 * @see
 * - cute_init()
 * - #CUTE_MAIN
 */
extern void
cute_fini(void) __cute_export;

/**
 * Test hierarchy runner main entry point.
 *
 * @param[in] _root    top-level test hierarchy suite
 * @param[in] _package package name string
 * @param[in] _version package version string
 *
 * Use #CUTE_MAIN as the ``main()`` entry point replacement for a test hierarchy
 * runner executable.
 *
 * @p _package should be given as a string containing the name of the package
 * for which testing code is written.
 * @p _version should hold the version string identifying the @p _package
 * package revision.
 * Both @p _package and @p _version are optional and may be specified as
 * ``NULL``.
 *
 * @see #CUTE_SUITE
 *
 * **Example**
 * @code{.c}
 * #include <cute/cute.h>
 *
 * CUTE_TEST(my_first_failing_test)
 * {
 *      cute_fail(NULL);
 * }
 *
 * CUTE_GROUP(my_first_group) = {
 *      CUTE_REF(my_first_failing_test)
 * };
 *
 * CUTE_SUITE(my_first_suite, my_first_group);
 *
 * CUTE_MAIN(my_first_suite, "My first package", "0.1")
 * @endcode
 */
#define CUTE_MAIN(_root, _package, _version) \
	int main(int argc, char * const argv[]) \
	{ \
		return cute_main(argc, argv, &(_root), _package, _version); \
	}

#endif /* _CUTE_H */
