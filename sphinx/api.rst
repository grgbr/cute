.. include:: _cdefs.rst
   
.. _sect-api-overview:

Overview
========

This document is a reference manual of |CUTe|'s programmatic API.

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
  
.. index::
   single: timeout; API
   single: test timeout; API
   single: timer; API
   single: test timer; API

Test Timeouts
=============

Various C preprocessor macros are exposed to assign timeouts to tests or suites
at definition time. These are:

* :c:macro:`CUTE_DFLT_TMOUT`
* :c:macro:`CUTE_INHR_TMOUT`
* :c:macro:`CUTE_NONE_TMOUT`

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

CUTE_INHR_SETUP
***************

.. doxygendefine:: CUTE_INHR_SETUP

CUTE_INHR_TEARDOWN
******************

.. doxygendefine:: CUTE_INHR_TEARDOWN

CUTE_INHR_TMOUT
***************

.. doxygendefine:: CUTE_INHR_TMOUT

CUTE_NONE_TMOUT
***************

.. doxygendefine:: CUTE_NONE_TMOUT

CUTE_NULL_SETUP
***************

.. doxygendefine:: CUTE_NULL_SETUP

CUTE_NULL_TEARDOWN
******************

.. doxygendefine:: CUTE_NULL_TEARDOWN

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
