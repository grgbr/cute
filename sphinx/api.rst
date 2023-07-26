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

.. index:: test definition, test case definition

Test definition
===============

Various C preprocessor macros are exposed to define test cases. These are:

.. hlist::

   * Test definition:

     * :c:macro:`CUTE_TEST`
     * :c:macro:`CUTE_TEST_DEFN`

   * Test timeout:

     * :c:macro:`CUTE_DFLT_TMOUT`
     * :c:macro:`CUTE_INHR_TMOUT`
     * :c:macro:`CUTE_NONE_TMOUT`

.. index:: fixture, fixture operation, fixture function
   
Fixture operations
==================

Various C preprocessor macros are exposed to attach test fixture functions to
tests or suites. These are:

* :c:macro:`CUTE_NULL_SETUP`
* :c:macro:`CUTE_NULL_TEARDOWN`
* :c:macro:`CUTE_INHR_SETUP`
* :c:macro:`CUTE_INHR_TEARDOWN`

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

CUTE_TEST_DEFN
**************

.. doxygendefine:: CUTE_TEST_DEFN
