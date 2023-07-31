.. include:: _cdefs.rst

.. |timeout|    replace:: :ref:`timeout <sect-user-writing_tests-test_timeout>`
.. |user guide| replace:: :doc:`User guide <user>`

.. _sect-api-overview:

Overview
========

Here is the |CUTe|'s programmatic API reference manual. If you need an in-depth
description of |CUTe| usage, you should refer to the |user guide| instead.

.. index:: build configuration, configuration macros

Build configuration
===================

At :ref:`Build configuration time <workflow-configure-phase>`, multiple build
options are available to customize final |CUTe| build. From client code, you may
eventually refer to the corresponding C macros listed below:

* :c:macro:`CONFIG_CUTE_INTERN_ASSERT`
* :c:macro:`CONFIG_CUTE_UTEST`

.. index::
   single: test definition; API
   single: test case definition; API

Test definition
===============

Various C preprocessor macros are exposed to define test cases. These are:

* :c:macro:`CUTE_TEST`
* :c:macro:`CUTE_TEST_DEFN`
* :c:macro:`CUTE_TEST_DECL`
* :c:macro:`CUTE_TEST_STATIC`
* :c:macro:`CUTE_TEST_EXTERN`

See |test case| definition section of |user guide| for more informations.

.. index::
   single: fixture; API
   single: fixture operation; API
   single: fixture function; API

Fixture operations
==================

Various C preprocessor macros are exposed to attach test fixture functions to
tests or suites at definition time. These are:

* :c:macro:`CUTE_NULL_SETUP`
* :c:macro:`CUTE_NULL_TEARDOWN`
* :c:macro:`CUTE_INHR_SETUP`
* :c:macro:`CUTE_INHR_TEARDOWN`

See |fixture| operations section of :doc:`User guide <user>` for more
informations.

.. index::
   single: timeout; API
   single: test timeout; API
   single: timer; API
   single: test timer; API

Timeouts
========

Various C preprocessor macros are exposed to assign timeouts to tests or suites
at definition time. These are:

* :c:macro:`CUTE_DFLT_TMOUT`
* :c:macro:`CUTE_INHR_TMOUT`
* :c:macro:`CUTE_NONE_TMOUT`

See |timer| section of :doc:`User guide <user>` for more informations.

.. index::
   single: group definition; API
   single: test group definition; API

Group definition
================

Various C preprocessor macros are exposed to define test groups. These are:

* :c:macro:`CUTE_REF`
* :c:macro:`CUTE_GROUP`
* :c:macro:`CUTE_GROUP_DEFN`
* :c:macro:`CUTE_GROUP_DECL`
* :c:macro:`CUTE_GROUP_STATIC`
* :c:macro:`CUTE_GROUP_EXTERN`

See |group| definition section of :doc:`User guide <user>` for more
informations.

.. index::
   single: suite definition; API
   single: test suite definition; API

Suite definition
================

Various C preprocessor macros are exposed to define test suites. These are:

* :c:macro:`CUTE_SUITE`
* :c:macro:`CUTE_SUITE_DEFN`
* :c:macro:`CUTE_SUITE_DECL`
* :c:macro:`CUTE_SUITE_STATIC`
* :c:macro:`CUTE_SUITE_EXTERN`

See |suite| definition section of :doc:`User guide <user>` for more
informations.

Running
=======

As a shorthand, use :c:macro:`CUTE_MAIN` as replacement for ``main()`` entry
point of a test hierarchy runner executable.

.. index:: API reference, reference

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
