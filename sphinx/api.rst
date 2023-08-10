.. SPDX-License-Identifier: GFDL-1.3-only

   This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

.. |timeout|           replace:: :ref:`timeout <sect-user-writing_tests-test_timeout>`
.. |user guide|        replace:: :doc:`User guide <user>`

.. |failed|            replace:: :ref:`failed <sect-user-result>`
.. |fixture functions| replace:: :ref:`fixture functions <sect-user-writing_tests-fixture_operations>`
.. |test functions|    replace:: :ref:`test functions <sect-user-writing_tests-test_definition>`

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

.. index:: test;API, API;test

Test definition
===============

Various C preprocessor macros are exposed to define test cases. These are:

* :c:macro:`CUTE_TEST`
* :c:macro:`CUTE_TEST_DEFN`
* :c:macro:`CUTE_TEST_DECL`
* :c:macro:`CUTE_TEST_STATIC`
* :c:macro:`CUTE_TEST_EXTERN`

See |test case| definition section of |user guide| for more informations.

.. index:: fixture;API, API;fixture

Fixture operations
==================

Various C preprocessor macros are exposed to attach test fixture functions to
tests or suites at definition time. These are:

* :c:macro:`CUTE_NULL_SETUP`
* :c:macro:`CUTE_NULL_TEARDOWN`
* :c:macro:`CUTE_INHR_SETUP`
* :c:macro:`CUTE_INHR_TEARDOWN`

See |fixture| operations section of |user guide| for more informations.

.. index:: timeout;API, API;timeout, timer;API, API;timer

Timeouts
========

Various C preprocessor macros are exposed to assign timeouts to tests or suites
at definition time. These are:

* :c:macro:`CUTE_DFLT_TMOUT`
* :c:macro:`CUTE_INHR_TMOUT`
* :c:macro:`CUTE_NONE_TMOUT`

See |timer| section of |user guide| for more informations.

.. index:: group;API, API;group

Group definition
================

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
================

Various C preprocessor macros are exposed to define test suites. These are:

* :c:macro:`CUTE_SUITE`
* :c:macro:`CUTE_SUITE_DEFN`
* :c:macro:`CUTE_SUITE_DECL`
* :c:macro:`CUTE_SUITE_STATIC`
* :c:macro:`CUTE_SUITE_EXTERN`

See |suite| definition section of |user guide| for more informations.

Test assertion
==============

Multiple test assertions are exposed as C preprocessor macros to implement
testing logic. These are:
  
.. hlist::

   * **Basic assertion** :

      * :c:macro:`cute_check_assert`

   * **Value assertion** :

      * :c:macro:`cute_check_sint`

   * **Range assertion** :

     * :c:struct:`cute_sint_range` structure
     * :c:macro:`CUTE_SINT_RANGE`
     * :c:macro:`cute_check_sint_range`

See :ref:`sect-user-assertion` section of |user guide| for more informations.

Running tests
=============

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

CUTE_REF
********

.. doxygendefine:: CUTE_REF

CUTE_SINT_RANGE
***************

.. doxygendefine:: CUTE_SINT_RANGE

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

cute_check_assert
*****************

.. doxygendefine:: cute_check_assert

cute_check_sint
***************

.. doxygendefine:: cute_check_sint

cute_check_sint_range
*********************

.. doxygendefine:: cute_check_sint_range

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
   
cute_sint_range
***************

.. doxygenstruct:: cute_sint_range
   :members:

Functions
---------

cute_init
*********

.. doxygenfunction:: cute_init

cute_fini
*********

.. doxygenfunction:: cute_fini

cute_run_suite
**************

.. doxygenfunction:: cute_run_suite

cute_show_suite
***************

.. doxygenfunction:: cute_show_suite
