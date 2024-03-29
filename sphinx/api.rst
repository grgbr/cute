.. SPDX-License-Identifier: GPL-3.0-only
   
   This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

.. |timeout|           replace:: :ref:`timeout <sect-user-writing_tests-test_timeout>`
.. |user guide|        replace:: :doc:`User guide <user>`

.. |skipped|           replace:: :ref:`skipped <sect-user-result>`
.. |expectation|       replace:: :ref:`expectation <sect-user-mocking>`

.. _sect-api-overview:

Overview
========

Here is the CUTe_'s programmatic API reference manual. If you need an in-depth
description of CUTe_ usage, you should refer to the |user guide| instead.

.. index:: build configuration, configuration macros

Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final CUTe_ build. From client code, you may
eventually refer to the corresponding C macros listed below:

* :c:macro:`CONFIG_CUTE_INTERN_ASSERT`
* :c:macro:`CONFIG_CUTE_UTEST`

Functional domains
==================

.. index:: test;API, API;test

Test definition
---------------

Various C preprocessor macros are exposed to define test cases. These are:

* :c:macro:`CUTE_TEST`
* :c:macro:`CUTE_TEST_DEFN`
* :c:macro:`CUTE_TEST_DECL`
* :c:macro:`CUTE_TEST_STATIC`
* :c:macro:`CUTE_TEST_EXTERN`

See |test case| definition section of |user guide| for more informations.

.. index:: fixture;API, API;fixture

Fixture operations
------------------

Various C preprocessor macros are exposed to attach test fixture functions to
tests or suites at definition time. These are:

* :c:macro:`CUTE_NULL_SETUP`
* :c:macro:`CUTE_NULL_TEARDOWN`
* :c:macro:`CUTE_INHR_SETUP`
* :c:macro:`CUTE_INHR_TEARDOWN`

See |fixture| operations section of |user guide| for more informations.

.. index:: timeout;API, API;timeout, timer;API, API;timer

Timeouts
--------

Various C preprocessor macros are exposed to assign timeouts to tests or suites
at definition time. These are:

* :c:macro:`CUTE_DFLT_TMOUT`
* :c:macro:`CUTE_INHR_TMOUT`
* :c:macro:`CUTE_NONE_TMOUT`

See |timer| section of |user guide| for more informations.

.. index:: group;API, API;group

Group definition
----------------

Various C preprocessor macros are exposed to define test groups. These are:

* :c:macro:`CUTE_REF`
* :c:macro:`CUTE_GROUP`
* :c:macro:`CUTE_GROUP_DEFN`
* :c:macro:`CUTE_GROUP_DECL`
* :c:macro:`CUTE_GROUP_STATIC`
* :c:macro:`CUTE_GROUP_EXTERN`

See |group| definition section of |user guide| for more informations.

.. index:: suite;API, API;suite

Suite definition
----------------

Various C preprocessor macros are exposed to define test suites. These are:

* :c:macro:`CUTE_SUITE`
* :c:macro:`CUTE_SUITE_DEFN`
* :c:macro:`CUTE_SUITE_DECL`
* :c:macro:`CUTE_SUITE_STATIC`
* :c:macro:`CUTE_SUITE_EXTERN`

See |suite| definition section of |user guide| for more informations.

.. index:: assertion;API, API;assertion

Test assertion
--------------

Multiple test assertions are exposed to implement testing logic. These are
described in the following sections.

You may also refer to :ref:`sect-user-assertion` section of |user guide| for
more informations.

.. _sect-api-basic_assert:

Basic assertion
***************

:c:macro:`cute_check_assert` implements a basic versatile test assertion 
exposed as a C preprocessor macros.

.. index:: boolean assertion;API, bool;API,
           API;boolean assertion, API;bool assertion,

.. _sect-api-bool_assert:

Booleans
********

Boolean values may be verified using the :c:macro:`cute_check_bool` macro.

.. index:: signed integer assertion;API, sint;API,
           API;signed integer assertion, API;sint assertion,
           integer;signed assertion

.. _sect-api-sint_assert:

Signed integers
***************

Signed integer values may be verified using so-called `sint` definitions :

.. hlist::

   * **Value assertion** :

      * :c:macro:`cute_check_sint`

   * **Range assertion** :

     * :c:struct:`cute_sint_range` structure
     * :c:macro:`CUTE_SINT_RANGE` initializer
     * :c:macro:`cute_check_sint_range` function-like macro

   * **Set assertion** :

     * :c:struct:`cute_sint_set` structure
     * :c:macro:`CUTE_SINT_SET` initializer
     * :c:macro:`cute_check_sint_set` function-like macro

.. index:: unsigned integer assertion;API, uint assertion;API,
           API;unsigned integer assertion, API;uint assertion,
           hexadecimal assertion;API, hex assertion;API,
           API;hexadecimal assertion, API;hex assertion,
           integer;unsigned assertion

.. _sect-api-uint_assert:

Unsigned integers
*****************

Unsigned integer values may be verified using so-called `uint` and / or `hex`
definitions :

.. hlist::

   * **Value assertion** :

      * :c:macro:`cute_check_uint`
      * :c:macro:`cute_check_hex`

   * **Range assertion** :

     * :c:struct:`cute_uint_range` structure
     * :c:macro:`CUTE_UINT_RANGE` initializer
     * :c:macro:`cute_check_uint_range` function-like macro
     * :c:macro:`cute_check_hex_range` function-like macro

   * **Set assertion** :

     * :c:struct:`cute_uint_set` structure
     * :c:macro:`CUTE_UINT_SET` initializer
     * :c:macro:`cute_check_uint_set` function-like macro
     * :c:macro:`cute_check_hex_set` function-like macro

.. index:: floating point assertion;API, flt assertion;API,
           API;floating point assertion, API;flt assertion
.. _sect-api-flt_assert:

Floating point numbers
**********************

Floating point values may be verified using so-called `flt` definitions :

.. hlist::

   * **Value assertion** :

      * :c:macro:`cute_check_flt`

   * **Range assertion** :

     * :c:struct:`cute_flt_range` structure
     * :c:macro:`CUTE_FLT_RANGE` initializer
     * :c:macro:`cute_check_flt_range` function-like macro

   * **Set assertion** :

     * :c:struct:`cute_flt_set` structure
     * :c:macro:`CUTE_FLT_SET` initializer
     * :c:macro:`cute_check_flt_set` function-like macro

.. index:: string assertion;API, str assertion;API,
           API;string assertion, API;str assertion
.. _sect-api-str_assert:

Strings
*******

`NULL` terminated C string contents may be verified using so-called `str`
definitions :

.. hlist::

   * **Value assertion** :

      * :c:macro:`cute_check_str`

   * **Set assertion** :

     * :c:struct:`cute_str_set` structure
     * :c:macro:`CUTE_STR_SET` initializer
     * :c:macro:`cute_check_str_set` function-like macro

.. index:: pointer assertion;API, ptr assertion;API,
           API;pointer assertion, API;ptr assertion
.. _sect-api-ptr_assert:

Pointers
********

Pointer values may be verified using so-called `ptr` definitions :

.. hlist::

   * **Value assertion** :

      * :c:macro:`cute_check_ptr`

   * **Range assertion** :

     * :c:struct:`cute_ptr_range` structure
     * :c:macro:`CUTE_PTR_RANGE` initializer
     * :c:macro:`cute_check_ptr_range` function-like macro

   * **Set assertion** :

     * :c:struct:`cute_ptr_set` structure
     * :c:macro:`CUTE_PTR_SET` initializer
     * :c:macro:`cute_check_ptr_set` function-like macro

.. index:: memory area assertion;API, mem assertion;API,
           API;memory area assertion, API;mem assertion
.. _sect-api-mem_assert:

Memory areas
************

Use :c:macro:`cute_check_mem` to verify content of memory areas.

.. index:: mocking;API, API;mocking, expectation;API, API;expectation

Mocking
-------

Multiple types of mocking expectations are supported to implement testing logic.
These are described in the following sections.

You may also refer to :ref:`sect-user-mocking` section of |user guide| for
more informations.

.. index:: assertion expectation;API, API;assertion expectation

.. _sect-api-assert_xpct:

Assertion expectation
*********************

Combine :c:macro:`cute_expect_assertion()` and :c:func:`cute_mock_assert()`
to ensure that an assertion failure has been thrown.

.. index:: function call expectation;API, call expectation;API,
           API;function call expectation, API;call expectation

.. _sect-api-call_xpct:

Call expectation
****************

Combine :c:macro:`cute_expect_call()` and :c:func:`cute_mock_call()`
to ensure that a function is called according to the expected schedule order.

.. index:: function parameter expectation;API, parameter expectation;API,
           API;function parameter expectation, API;parameter expectation

.. _sect-api-parm_xpct:

Parameter expectation
*********************

Multiple types of function parameter expectations are supported to implement
testing logic. These are described in the following sections.

.. index:: boolean expectation;API, bool expectation;API,
           API;boolean expectation, API;bool expectation

Booleans
^^^^^^^^

Boolean function parameter expectations may be verified using so-called
`bool` definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_bool_parm`

   * **Parameter check**:

      * :c:macro:`cute_mock_bool_parm()` function-like macro

.. index:: signed integer expectation;API, sint expectation;API,
           API;signed integer expectation, API;sint expectation,
           integer;signed expectation

Signed integers
^^^^^^^^^^^^^^^

Signed integer function parameter expectations may be verified using so-called
`sint` definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_sint_parm`

   * **Parameter range expectation** :

     * :c:struct:`cute_sint_range` structure
     * :c:macro:`CUTE_SINT_RANGE` initializer
     * :c:macro:`cute_expect_sint_range` function-like macro

   * **Parameter set expectation** :

     * :c:struct:`cute_sint_set` structure
     * :c:macro:`CUTE_SINT_SET` initializer
     * :c:macro:`cute_expect_sint_set` function-like macro

   * **Parameter check**:

      * :c:macro:`cute_mock_sint_parm()` function-like macro

.. index:: unsigned integer expectation;API, uint expectation;API,
           API;unsigned integer expectation, API;uint expectation,
           integer;unsigned expectation

Unsigned integers
^^^^^^^^^^^^^^^^^

Unsigned integer function parameter expectations may be verified using so-called
`uint` definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_uint_parm`
      * :c:macro:`cute_expect_hex_parm`

   * **Parameter range expectation** :

     * :c:struct:`cute_uint_range` structure
     * :c:macro:`CUTE_UINT_RANGE` initializer
     * :c:macro:`cute_expect_uint_range` function-like macro
     * :c:macro:`cute_expect_hex_range` function-like macro

   * **Parameter set expectation** :

     * :c:struct:`cute_uint_set` structure
     * :c:macro:`CUTE_UINT_SET` initializer
     * :c:macro:`cute_expect_uint_set` function-like macro
     * :c:macro:`cute_expect_hex_set` function-like macro

   * **Parameter check**:

      * :c:macro:`cute_mock_uint_parm()` function-like macro
      * :c:macro:`cute_mock_hex_parm()` function-like macro

.. index:: floating point expectation;API, flt expectation;API,
           API;floating point expectation, API;flt expectation,
           integer;floating point

Floating point numbers
^^^^^^^^^^^^^^^^^^^^^^

Floating point number function parameter expectations may be verified using
so-called `flt` definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_flt_parm`

   * **Parameter range expectation** :

     * :c:struct:`cute_flt_range` structure
     * :c:macro:`CUTE_FLT_RANGE` initializer
     * :c:macro:`cute_expect_flt_range` function-like macro

   * **Parameter set expectation** :

     * :c:struct:`cute_flt_set` structure
     * :c:macro:`CUTE_FLT_SET` initializer
     * :c:macro:`cute_expect_flt_set` function-like macro

   * **Parameter check**:

      * :c:macro:`cute_mock_flt_parm()` function-like macro

.. index:: string expectation;API, str expectation;API,
           API;string expectation, API;str expectation,
           integer;string

Strings
^^^^^^^

String function parameter expectations may be verified using
so-called `str` definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_str_parm`

   * **Parameter set expectation** :

     * :c:struct:`cute_str_set` structure
     * :c:macro:`CUTE_STR_SET` initializer
     * :c:macro:`cute_expect_str_set` function-like macro

   * **Parameter check**:

      * :c:macro:`cute_mock_str_parm()` function-like macro

.. index:: pointer expectation;API, ptr expectation;API,
           API;pointer expectation, API;ptr expectation,
           integer;pointer

Pointers
^^^^^^^^

Pointer function parameter expectations may be verified using so-called `ptr`
definitions :

.. hlist::

   * **Parameter value expectation** :

      * :c:macro:`cute_expect_ptr_parm`

   * **Parameter range expectation** :

     * :c:struct:`cute_ptr_range` structure
     * :c:macro:`CUTE_PTR_RANGE` initializer
     * :c:macro:`cute_expect_ptr_range` function-like macro

   * **Parameter set expectation** :

     * :c:struct:`cute_ptr_set` structure
     * :c:macro:`CUTE_PTR_SET` initializer
     * :c:macro:`cute_expect_ptr_set` function-like macro

   * **Parameter check**:

      * :c:macro:`cute_mock_ptr_parm()` function-like macro

.. index:: memory area expectation;API, mem expectation;API,
           API;memory area expectation, API;mem expectation

Memory areas
^^^^^^^^^^^^

Combine :c:macro:`cute_expect_mem_parm()` and :c:func:`cute_mock_mem_parm()`
to verify memory areas content.

.. index:: function return value expectation;API, return value expectation;API,
           API;return value expectation, API;return value expectation

.. _sect-api-retval_xpct:

Return value expectation
************************

Multiple types of function return value expectations are supported to implement
testing logic :

.. hlist::

   * **Booleans** :

     * :c:macro:`cute_expect_bool_retval` function-like macro
     * :c:macro:`cute_mock_bool_retval` function-like macro

   * **Signed integers** :

     * :c:macro:`cute_expect_sint_retval` function-like macro
     * :c:macro:`cute_mock_sint_retval` function-like macro

   * **Unsigned integers** :

     * :c:macro:`cute_expect_uint_retval` function-like macro
     * :c:macro:`cute_mock_uint_retval` function-like macro

   * **Floating point numbers** :

     * :c:macro:`cute_expect_flt_retval` function-like macro
     * :c:macro:`cute_mock_flt_retval` function-like macro

   * **Strings** :

     * :c:macro:`cute_expect_str_retval` function-like macro
     * :c:macro:`cute_mock_str_retval` function-like macro

   * **Pointers** :

     * :c:macro:`cute_expect_ptr_retval` function-like macro
     * :c:macro:`cute_mock_ptr_retval` function-like macro

.. index:: main;API, API;main

Running tests
-------------

Use :c:macro:`CUTE_MAIN` as replacement for ``main()`` entry point of a test
runner executable.
This is the most straightforward way to run a test |hierarchy|. Head to section
:ref:`main entry point <sect-user-main>` of |user guide| for more informations.

The following low-level |API| is exposed for additional flexibility :

.. hlist::

   * **Run setup** :

     * :c:struct:`cute_config` structure
     * :c:func:`cute_init`
     * :c:func:`cute_fini`

   * **Operations** :

     * :c:func:`cute_run_suite`
     * :c:func:`cute_show_suite`
     * :c:func:`cute_suite_info`

See section :ref:`Low level runner <sect-user-low_level_runner>` of |user guide|
for more informations.

.. index:: API;reference, reference;API

Reference
=========

Configuration macros
--------------------

CONFIG_CUTE_INTERN_ASSERT
*************************

.. doxygendefine:: CONFIG_CUTE_INTERN_ASSERT

CONFIG_CUTE_UTEST
*****************

.. doxygendefine:: CONFIG_CUTE_UTEST

Macros
------

CUTE_CONFIG_INIT
****************

.. doxygendefine:: CUTE_CONFIG_INIT

CUTE_DFLT_TMOUT
***************

.. doxygendefine:: CUTE_DFLT_TMOUT

CUTE_FLT_RANGE
***************

.. doxygendefine:: CUTE_FLT_RANGE

CUTE_FLT_SET
*************

.. doxygendefine:: CUTE_FLT_SET

CUTE_GROUP
**********

.. doxygendefine:: CUTE_GROUP

CUTE_GROUP_DEFN
***************

.. doxygendefine:: CUTE_GROUP_DEFN

CUTE_GROUP_DECL
***************

.. doxygendefine:: CUTE_GROUP_DECL

CUTE_GROUP_STATIC
*****************

.. doxygendefine:: CUTE_GROUP_STATIC

CUTE_GROUP_EXTERN
*****************

.. doxygendefine:: CUTE_GROUP_EXTERN

CUTE_INHR_SETUP
***************

.. doxygendefine:: CUTE_INHR_SETUP

CUTE_INHR_TEARDOWN
******************

.. doxygendefine:: CUTE_INHR_TEARDOWN

CUTE_INHR_TMOUT
***************

.. doxygendefine:: CUTE_INHR_TMOUT

CUTE_MAIN
*********

.. doxygendefine:: CUTE_MAIN

CUTE_NONE_TMOUT
***************

.. doxygendefine:: CUTE_NONE_TMOUT

CUTE_NULL_SETUP
***************

.. doxygendefine:: CUTE_NULL_SETUP

CUTE_NULL_TEARDOWN
******************

.. doxygendefine:: CUTE_NULL_TEARDOWN

CUTE_PTR_RANGE
**************

.. doxygendefine:: CUTE_PTR_RANGE

CUTE_PTR_SET
************

.. doxygendefine:: CUTE_PTR_SET

CUTE_REF
********

.. doxygendefine:: CUTE_REF

CUTE_SINT_RANGE
***************

.. doxygendefine:: CUTE_SINT_RANGE

CUTE_SINT_SET
*************

.. doxygendefine:: CUTE_SINT_SET

CUTE_STR_SET
*************

.. doxygendefine:: CUTE_STR_SET

CUTE_SUITE
**********

.. doxygendefine:: CUTE_SUITE

CUTE_SUITE_DEFN
***************

.. doxygendefine:: CUTE_SUITE_DEFN

CUTE_SUITE_DECL
***************

.. doxygendefine:: CUTE_SUITE_DECL

CUTE_SUITE_STATIC
*****************

.. doxygendefine:: CUTE_SUITE_STATIC

CUTE_SUITE_EXTERN
*****************

.. doxygendefine:: CUTE_SUITE_EXTERN

CUTE_TEST
*********

.. doxygendefine:: CUTE_TEST

CUTE_TEST_DECL
**************

.. doxygendefine:: CUTE_TEST_DECL

CUTE_TEST_DEFN
**************

.. doxygendefine:: CUTE_TEST_DEFN

CUTE_TEST_EXTERN
****************

.. doxygendefine:: CUTE_TEST_EXTERN

CUTE_TEST_STATIC
****************

.. doxygendefine:: CUTE_TEST_STATIC

CUTE_UINT_RANGE
***************

.. doxygendefine:: CUTE_UINT_RANGE

CUTE_UINT_SET
*************

.. doxygendefine:: CUTE_UINT_SET

cute_check_assert
*****************

.. doxygendefine:: cute_check_assert

cute_check_bool
***************

.. doxygendefine:: cute_check_bool

cute_check_flt
**************

.. doxygendefine:: cute_check_flt

cute_check_flt_range
********************

.. doxygendefine:: cute_check_flt_range

cute_check_flt_set
******************

.. doxygendefine:: cute_check_flt_set

cute_check_hex
**************

.. doxygendefine:: cute_check_hex

cute_check_hex_range
********************

.. doxygendefine:: cute_check_hex_range

cute_check_hex_set
******************

.. doxygendefine:: cute_check_hex_set

cute_check_mem
**************

.. doxygendefine:: cute_check_mem

cute_check_ptr
**************

.. doxygendefine:: cute_check_ptr

cute_check_ptr_range
********************

.. doxygendefine:: cute_check_ptr_range

cute_check_ptr_set
******************

.. doxygendefine:: cute_check_ptr_set

cute_check_sint
***************

.. doxygendefine:: cute_check_sint

cute_check_sint_range
*********************

.. doxygendefine:: cute_check_sint_range

cute_check_sint_set
*******************

.. doxygendefine:: cute_check_sint_set

cute_check_str
**************

.. doxygendefine:: cute_check_str

cute_check_str_set
******************

.. doxygendefine:: cute_check_str_set

cute_check_uint
***************

.. doxygendefine:: cute_check_uint

cute_check_uint_range
*********************

.. doxygendefine:: cute_check_uint_range

cute_check_uint_set
*******************

.. doxygendefine:: cute_check_uint_set

cute_expect_assertion
*********************

.. doxygendefine:: cute_expect_assertion

cute_expect_bool_parm
*********************

.. doxygendefine:: cute_expect_bool_parm

cute_expect_bool_retval
***********************

.. doxygendefine:: cute_expect_bool_retval

cute_expect_call
****************

.. doxygendefine:: cute_expect_call

cute_expect_flt_parm
********************

.. doxygendefine:: cute_expect_flt_parm

cute_expect_flt_range
*********************

.. doxygendefine:: cute_expect_flt_range

cute_expect_flt_retval
**********************

.. doxygendefine:: cute_expect_flt_retval

cute_expect_flt_set
*******************

.. doxygendefine:: cute_expect_flt_set

cute_expect_hex_parm
********************

.. doxygendefine:: cute_expect_hex_parm

cute_expect_hex_range
*********************

.. doxygendefine:: cute_expect_hex_range

cute_expect_hex_set
*******************

.. doxygendefine:: cute_expect_hex_set

cute_expect_mem_parm
********************

.. doxygendefine:: cute_expect_mem_parm

cute_expect_ptr_parm
********************

.. doxygendefine:: cute_expect_ptr_parm

cute_expect_ptr_range
*********************

.. doxygendefine:: cute_expect_ptr_range

cute_expect_ptr_retval
**********************

.. doxygendefine:: cute_expect_ptr_retval

cute_expect_ptr_set
*******************

.. doxygendefine:: cute_expect_ptr_set

cute_expect_sint_parm
*********************

.. doxygendefine:: cute_expect_sint_parm

cute_expect_sint_range
**********************

.. doxygendefine:: cute_expect_sint_range

cute_expect_sint_retval
***********************

.. doxygendefine:: cute_expect_sint_retval

cute_expect_sint_set
********************

.. doxygendefine:: cute_expect_sint_set

cute_expect_str_parm
********************

.. doxygendefine:: cute_expect_str_parm

cute_expect_str_retval
**********************

.. doxygendefine:: cute_expect_str_retval

cute_expect_str_set
*******************

.. doxygendefine:: cute_expect_str_set

cute_expect_uint_parm
*********************

.. doxygendefine:: cute_expect_uint_parm

cute_expect_uint_range
**********************

.. doxygendefine:: cute_expect_uint_range

cute_expect_uint_retval
***********************

.. doxygendefine:: cute_expect_uint_retval

cute_expect_uint_set
********************

.. doxygendefine:: cute_expect_uint_set

cute_fail
*********

.. doxygendefine:: cute_fail

cute_mock_bool_parm
*******************

.. doxygendefine:: cute_mock_bool_parm

cute_mock_bool_retval
*********************

.. doxygendefine:: cute_mock_bool_retval

cute_mock_call
**************

.. doxygendefine:: cute_mock_call

cute_mock_flt_parm
******************

.. doxygendefine:: cute_mock_flt_parm

cute_mock_flt_retval
********************

.. doxygendefine:: cute_mock_flt_retval

cute_mock_hex_parm
******************

.. doxygendefine:: cute_mock_hex_parm

cute_mock_mem_parm
******************

.. doxygendefine:: cute_mock_mem_parm

cute_mock_ptr_parm
******************

.. doxygendefine:: cute_mock_ptr_parm

cute_mock_ptr_retval
********************

.. doxygendefine:: cute_mock_ptr_retval

cute_mock_sint_parm
*******************

.. doxygendefine:: cute_mock_sint_parm

cute_mock_sint_retval
*********************

.. doxygendefine:: cute_mock_sint_retval

cute_mock_str_parm
******************

.. doxygendefine:: cute_mock_str_parm

cute_mock_str_retval
********************

.. doxygendefine:: cute_mock_str_retval

cute_mock_uint_parm
*******************

.. doxygendefine:: cute_mock_uint_parm

cute_mock_uint_retval
*********************

.. doxygendefine:: cute_mock_uint_retval

cute_skip
*********

.. doxygendefine:: cute_skip

Enums
-----

cute_config_report
******************

.. doxygenenum:: cute_config_report

cute_config_tty
***************

.. doxygenenum:: cute_config_tty

Structs
-------

cute_config
***********

.. doxygenstruct:: cute_config
   :members:

cute_flt_range
**************

.. doxygenstruct:: cute_flt_range
   :members:

cute_flt_set
************

.. doxygenstruct:: cute_flt_set
   :members:

cute_ptr_range
**************

.. doxygenstruct:: cute_ptr_range
   :members:

cute_ptr_set
************

.. doxygenstruct:: cute_ptr_set
   :members:

cute_sint_range
***************

.. doxygenstruct:: cute_sint_range
   :members:

cute_sint_set
*************

.. doxygenstruct:: cute_sint_set
   :members:

cute_str_set
************

.. doxygenstruct:: cute_str_set
   :members:

cute_uint_range
***************

.. doxygenstruct:: cute_uint_range
   :members:

cute_uint_set
*************

.. doxygenstruct:: cute_uint_set
   :members:

Functions
---------

cute_init
*********

.. doxygenfunction:: cute_init

cute_fini
*********

.. doxygenfunction:: cute_fini

cute_mock_assert
****************

.. doxygenfunction:: cute_mock_assert

cute_run_suite
**************

.. doxygenfunction:: cute_run_suite

cute_show_suite
***************

.. doxygenfunction:: cute_show_suite

cute_suite_info
***************

.. doxygenfunction:: cute_suite_info
