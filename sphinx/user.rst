.. SPDX-License-Identifier: GPL-3.0-only
   
   This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

.. _tdd:        https://en.wikipedia.org/wiki/Test-driven_development
.. _bdd:        https://en.wikipedia.org/wiki/Behavior-driven_development
.. _stdios:     https://www.gnu.org/software/libc/manual/html_node/Standard-Streams.html
.. _umlclass:   https://en.wikipedia.org/wiki/Class_diagram
.. _pkgconfig:  https://www.freedesktop.org/wiki/Software/pkg-config
.. _make:       https://www.gnu.org/software/make/
.. |pkg-config| replace:: `pkg-config <pkgconfig_>`_
.. |signal(7)|  replace:: :manpage:`signal(7)`
.. |test scope| replace:: :ref:`test global file scope <sect-user-writing_tests-test_file_scope>`
.. |build|      replace:: :ref:`build <sect-user-building_tests>`
.. |run|        replace:: :ref:`run <sect-user-running_tests>`
.. |regex(7)|   replace:: :manpage:`regex(7)`
.. |longjmp(3)| replace:: :manpage:`longjmp(3)`
.. |fork(2)|    replace:: :manpage:`fork(2)`

.. _passed:     sect-user-result_

.. _mock: https://en.wikipedia.org/wiki/Mock_object
.. _mocking: mock_

.. _regtest: https://en.wikipedia.org/wiki/Regression_testing
.. _regression testing: regtest_

.. _test anything protocol: tap_
.. _tap home page: tap_
.. _test fixtures: fixture_

.. _utest: https://en.wikipedia.org/wiki/Unit_testing
.. _unit tests: utest_
.. _unit testing: utest_
 
Overview
========

CUTe_ is a lightweight `unit testing`_ and mocking_ framework for C.

`Unit testing`_ is a software development practice where small units of
code are tested for proper operation. The technique is carried out as part of
software development process testing activities.
`Test-Driven Development (TDD) <tdd_>`_ and
`Behavior-Driven Development (BDD) <bdd_>`_ are well known examples of such
development processes making heavy usage of `unit testing`_.

CUTe_ is meant to ease the process of writing `unit tests`_ and make
them suitable for test automation and `regression testing`_ activities. It is
designed with the following goals in mind:

.. rubric:: Features

* test failure recovery,
* extensive reporting,
* flexible and modular test suite hierarchy,
* `test fixtures`_,
* strict function mocking_,
* JUnit_ XML reporting,
* `Test Anything Protocol`_ reporting,
* easy debugging of test failures,
* clear and expressive C API,
* C library dependency only.

.. rubric:: Limitations

* GNU / Linux / `Glibc`_ userspace C `unit testing`_ only
* |longjmp(3)| based test failure recovery only
* no |fork(2)| based test isolation
* no parameterizable test support
* custom check assertion not yet supported.
  
CUTe_ sources are distributed under the :ref:`GNU Lesser General Public License
<lgpl>` whereas documentation manuals are distributed under the `GNU General
Public License <gpl>`.

Basic concepts
==============

CUTe_ is a framework allowing to write, manage and run unit tests in C. It's
|API| provides a set of functions to :

* structure |test case| / |suite| |hierarchy| ;
* check strongly typed data against specified constraints using
  `test assertion`_\s;
* schecule and verify :ref:`mock expectations <sect-user-mocking>` against
  specified constraints ;
* |run| test cases and suites according to specified configurations ;
* |report| test results onto the console and / or,
* into files according to various specified formats.

CUTe_ is built as a static and / or shared library which must be linked with
the user's testing code.

Tests are organized in a hierarchical manner as shown in the example diagram
below :

.. graphviz::
   :align: center

   digraph {
        node [shape=box3d margin="0.15"]
        root [label=<root<BR/><FONT COLOR="gray40"><I>&laquo;suite&raquo;</I></FONT>>]
        s0   [label=<suite 0<BR/><FONT COLOR="gray40"><I>&laquo;suite&raquo;</I></FONT>>]
        s1   [label=<suite 1<BR/><FONT COLOR="gray40"><I>&laquo;suite&raquo;</I></FONT>>]

        node [shape=box margin="0.15"]
        t0   [label=<test 0<BR/><FONT COLOR="gray40"><I>&laquo;test&raquo;</I></FONT>>]
        t00  [label=<test 00<BR/><FONT COLOR="gray40"><I>&laquo;test&raquo;</I></FONT>>]
        t01  [label=<test 01<BR/><FONT COLOR="gray40"><I>&laquo;test&raquo;</I></FONT>>]
        t10  [label=<test 10<BR/><FONT COLOR="gray40"><I>&laquo;test&raquo;</I></FONT>>]

        root -> s0
        root -> t0
        root -> s1
        s0 -> t00
        s0 -> t01
        s1 -> t10
   }


A test (case) implements the logic validating a single unit of code to check for
proper operation.

A |suite| is a collection of suites and / or test cases. All suites may
arbitrarily be selected for a |run|.

Finally, a particular |run| may be setup to |report| test results according to a
specified configuration.

Typical Workflow
================

As a **developper**, implementing tests involves the following sequence of
steps:

.. rubric:: Preparatory phase:

#. write a dummy failing |test case| ;
#. setup a primary test |hierarchy| including a single root |suite| and the test
   just created ;
#. |build| and |run| to check for proper operation ;
#. modify the test written initially to implement the first real test case ;

.. rubric:: Iterative test implementation phases:

#. |build| and |run| ;
#. fix test failures ;
#. implement additional |test case| ;
#. when required, refine test |hierarchy| by defining test |suite|\s

As an **integrator**, you may be required to tweak test suites |run|
configuration for proper integration within test regression / automation
infrastructure.

Writing tests
=============

Test case
---------

.. index:: test;definition, definition;test
.. _sect-user-writing_tests-test_definition:

Test definition
***************

To **define a test**, one is expected to:

* give the test a name,
* provide a function / block of instructions that defines the testing logic,
* optionally specify test |fixture| function(s) and / or |timer|.

Most of the time, implementing the testing logic relies upon heavy usage of
`test assertion`_.

.. _user-signals:

At running time, the test function is executed with the following
**assumptions** :

* the ``setup()`` |fixture| function (if any) has completed ;
* a |timer| has been armed to protect against situations where testing
  logic hangs ;
* ``SIGFPE``, ``SIGILL``, ``SIGSEGV``, ``SIGBUS`` and ``SIGSYS`` |signal(7)|
  dispositions have been setup to catch potential testing logic exceptions /
  traps / crashes ;
* the ``teardown()`` |fixture| function (if any) is always executed after test
  function has completed ;
* the test function does not alter CUTe_'s internal state consistency.

As stated above, result is unpredictable when the test function alters CUTe_'s
internal state consistency. In particular, the **test function is not allowed
to** :

* modify test |hierarchy| ;
* |run| tests or |suite|\s ;
* alter |signal(7)| dispositions installed by CUTe_.

Using the :c:macro:`CUTE_TEST` macro is the most straightforward way to define
a test:

.. code-block:: c
   :linenos:

   CUTE_TEST(sample_test)
   {
        /* Implement testing logic here */
   }

The code sample above implements a test case named ``sample_test``. The call to
:c:macro:`CUTE_TEST` is immediately followed by a block of instructions that
defines the related testing logic.

As not specified, no particular |fixture| functions are attached to the
``sample_test`` test case, meaning that it will inherit them from its parent
suite once explicicly registered.

In addition, as no test |timer| is specified, it will also inherit from its
parent |suite| timeout settings at registering time.

Just in case you need to debug the testing logic, you should remind that
the block of instructions above is used to assign the ``sample_test`` test case
a test function defined as following :

.. code-block:: c

   static void sample_test__cute_exec(void);

For additional flexibility, CUTe_ also allows to :

* attach |fixture| functions to tests and / or suites ;
* assign a |timer| to tests and / or suites ;
* specify global |test scope| for tests and / or suites.

.. index:: fixture;test, test;fixture
.. _sect-user-writing_tests-fixture_operations:

Fixture operations
******************

A test case may be attached optional |fixture| operations to setup runtime
preconditions and / or postconditions of the unit of code under test.

Using |fixture| operations brings in multiple benefits :

* it enhances test repeatability since tests are executed with controlled and
  reproducible runtime preconditions ;
* it allows for better reusability since testing logic and testing context setup
  may be delegated to different functions.

2 types of |fixture| operation are distinguished. These are implemented using
``setup()`` and ``teardown()`` functions so that the whole test case sequence
is :

* execute the ``setup()`` function attached to test case ;
* *if* ``setup()`` function succeeded:

  * execute test case test function ;
  * execute the ``teardown()`` function attached to test case.

* *else* fail the test case.

The ``setup()`` function is meant to initialize the testing context to a
deterministic state whereas the ``teardown()`` function is used to restore
testing context that existed prior to ``setup()`` execution.

Use the :c:macro:`CUTE_TEST_DEFN` macro to define a fixture'd test case:

.. code-block:: c
   :linenos:
   :emphasize-lines: 23,24

   static void sample_setup(void)
   {
        /* Initialize test case runtime context. */
   }

   static void sample_teardown(void)
   {
        /* Finalize test case runtime context. */
   }

   /* `sample_fixtured_test' test case test function. */
   static void sample_fixtured_test_exec(void)
   {
        /* Implement testing logic here */
   }

   /*
    * Define `sample_fixtured_test' test case with attached setup() and
    * teardown() fixture operations as well as default test timeout.
    */
   static CUTE_TEST_DEFN(sample_fixtured_test,
                         sample_fixtured_test_exec,
                         sample_setup,
                         sample_teardown,
                         CUTE_DFLT_TMOUT);

Fixture functions may be specified at |test case| definition time using either
of the following alternatives :

* :c:macro:`CUTE_NULL_SETUP`, denoting an empty / NULL setup function ;
* :c:macro:`CUTE_INHR_SETUP`, requesting a setup fixture operation
  :ref:`inherited from the parent <sect-user-test_hierarchy-fixture_inheritance>`
  |suite| if existing and which falls back to :c:macro:`CUTE_NULL_SETUP`
  otherwise ;
* :c:macro:`CUTE_NULL_TEARDOWN`, denoting an empty / NULL teardown function ;
* :c:macro:`CUTE_INHR_TEARDOWN`, requesting a teardown fixture operation
  :ref:`inherited from the parent <sect-user-test_hierarchy-fixture_inheritance>`
  |suite| if existing and which falls back to :c:macro:`CUTE_NULL_TEARDOWN`
  otherwise ;
* the address of a custom function as mentionned above.

.. index:: timeout;test, timer;test, test;timeout, test;timer
.. _sect-user-writing_tests-test_timeout:

Test timeout
************

To protect against situations where a |test case| or |fixture| function hangs,
CUTe_ may arm a test timer that interrupts current test case execution upon
expiry.
The timeout may be specified at |test case| definition time using one of the
following C macros :

* :c:macro:`CUTE_TEST_DEFN`,
* :c:macro:`CUTE_TEST_STATIC`,
* :c:macro:`CUTE_TEST_EXTERN`.

All 3 macros take a timeout setting passed as ``_tmout`` argument and which may
be specified as one of :

* :c:macro:`CUTE_DFLT_TMOUT`, denoting a default timeout of 3 seconds ;
* :c:macro:`CUTE_INHR_TMOUT`, requesting a timeout
  :ref:`inherited from the parent <sect-user-test_hierarchy-timeout_inheritance>`
  |suite| if existing and which falls back to :c:macro:`CUTE_DFLT_TMOUT`
  otherwise ;
* :c:macro:`CUTE_NONE_TMOUT`, to disable the timeout mechanism ;
* or a *non-zero unsigned integer*, specifying a timeout value expressed as
  seconds.

When the timer expires, the current |test case| or |fixture| function execution
is *interrupted*, CUTe_ marks the test as *failing* then proceeds to the *next*
one in sequence.

Use the :c:macro:`CUTE_TEST_DEFN` macro to specify a timeout at definition
time :

.. code-block:: c
   :linenos:
   :emphasize-lines: 15

   /* `sample_timed_test' test case test function. */
   static void sample_timed_test_exec(void)
   {
        /* Implement testing logic here */
   }

   /*
    * Define `sample_timed_test' test case with an explicitly specified
    * timeout of 10 seconds.
    */
   static CUTE_TEST_DEFN(sample_timed_test,
                         sample_timed_test_exec,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         10U);

.. index:: file scope;test, test;file scope
.. _sect-user-writing_tests-test_file_scope:

Test file scope
***************

To enhance test case reusability, CUTe_ allows to specify global file scope
at test definition time. 2 shorthand macros allow to minimize test developper
workload compared to :c:macro:`CUTE_TEST_DEFN` usage. These are :

* :c:macro:`CUTE_TEST_STATIC`,
* :c:macro:`CUTE_TEST_DECL` and :c:macro:`CUTE_TEST_EXTERN`.

:c:macro:`CUTE_TEST_STATIC` allows to define a test with ``static`` global file
scope. :ref:`Registering <sect-user-test_hierarchy>` the created test to a
|suite| is then restricted to the source file where the |suite| is defined :

.. code-block:: c
   :linenos:
   :emphasize-lines: 5-8

   /*
    * Define `sample_static_test' test case with static global file
    * scope.
    */
   CUTE_TEST_STATIC(sample_static_test,
                    CUTE_INHR_SETUP,
                    CUTE_INHR_TEARDOWN,
                    CUTE_INHR_TMOUT)
   {
        /* Implement testing logic here */
   }

:c:macro:`CUTE_TEST_DECL` and :c:macro:`CUTE_TEST_EXTERN` macros are meant to
work with reusable definitions.
Use :c:macro:`CUTE_TEST_EXTERN` to define a test with default global file
scope, i.e. with external linkage :

.. code-block:: c
   :linenos:
   :emphasize-lines: 7-10

   #include "test.h"

   /*
    * Define `sample_extern_test' test case with default global file
    * scope.
    */
   CUTE_TEST_EXTERN(sample_extern_test,
                    CUTE_INHR_SETUP,
                    CUTE_INHR_TEARDOWN,
                    CUTE_INHR_TMOUT)
   {
        /* Implement testing logic here */
   }

And use :c:macro:`CUTE_TEST_DECL` to declare the test defined above in a header
so that it may be referenced from other compilation units :

.. code-block:: c
   :linenos:
   :emphasize-lines: 10

   #ifndef _TEST_H
   #define _TEST_H

   #include <cute/cute.h>

   /*
    * Declare `sample_extern_test' test case with default global file
    * scope.
    */
   extern CUTE_TEST_DECL(sample_extern_test);

   #endif /* _TEST_H */

As shown above, both :c:macro:`CUTE_TEST_STATIC` and :c:macro:`CUTE_TEST_EXTERN`
macros should be immediately followed by a block of instructions that defines
the related testing logic.
The block of instructions is used to define a test function which is assigned to
the created test case with the following signature scheme:

.. code-block:: c

   static void <test_case_name>__cute_exec(void);

Next section shows how to register test cases to suites and how to structure a
complete test |hierarchy|.

.. _sect-user-test_hierarchy:

Test hierarchy
--------------

Test hierarchy is structured around test suites. A |suite| is basically a
container for (sub-)tests and / or (sub-)suites.
Once a
:ref:`suite has been defined <sect-user-test_hierarchy-suite_definition>`,
(sub-)tests and / or (sub-)suites may be registered to it thanks to a |group|
definition.

A test hierarchy `UML class diagram <umlclass_>`_ would barely look like:

.. uml::
   :align: center

      @startuml

      skinparam monochrome true
      skinparam backgroundColor transparent
      skinparam shadowing false
      skinparam defaultFontSize 14
      skinparam linetype ortho
      skinparam circledCharacterFontSize 0
      skinparam circledCharacterRadius 0
      skinparam classAttributeIconSize 0
      hide empty members

      together {
          class cute_base
          class cute_suite
      }

      cute_base "*" --o "1" cute_suite
      Group . (cute_suite, cute_base)
      cute_base <|-- cute_suite
      cute_base <|-- cute_test

      @enduml

To create a test hierarchy, follow the typical workflow hereafter :

* define |test case|\s,
* define a |group| referencing tests defined above,
* :ref:`define a suite <sect-user-test_hierarchy-suite_definition>` based on the
  group defined above.

Let's see how to define a group...

.. index:: group;definition, definition;group
.. _sect-user-test_hierarchy-group_definition:

Group definition
****************

A test group is basically an ordered collection of test cases and / or
|suite|\s.

Using the :c:macro:`CUTE_GROUP` macro is the most straightforward way to define
a group:

.. code-block:: c
   :linenos:
   :emphasize-lines: 11-14

   CUTE_TEST(sample_test_0)
   {
        /* Implement testing logic here */
   }

   CUTE_TEST(sample_test_1)
   {
        /* Implement testing logic here */
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_test_0),
        CUTE_REF(sample_test_1),
   };

The code sample above defines 2 test cases respectively named ``sample_test_0``
and ``sample_test_1``. The call to :c:macro:`CUTE_GROUP` defines a test group
named ``sample_group`` that aggregates both test cases created above.
You may later :ref:`define suite(s) <sect-user-test_hierarchy-suite_definition>`
by using the variable named ``sample_group`` which itself refers to the created
group.

This test group variable is defined with *static global file scope*. However,
for additional flexibility and reusability, CUTe_ also provides the following
macros that allow to specify a desired global file scope :

* :c:macro:`CUTE_GROUP_STATIC`,
* :c:macro:`CUTE_GROUP_EXTERN`,
* :c:macro:`CUTE_GROUP_DEFN` and :c:macro:`CUTE_GROUP_DECL`.

Now that a group has been defined, let's see how to use it to define a suite...

.. index:: suite;definition, definition;suite
.. _sect-user-test_hierarchy-suite_definition:

Suite definition
****************

At running time, a suite iterates over tests and / or suites and run each one of
them sequencially according to the order they are registered to it.
Tests and / or suites are registered to a parent suite thanks to the |group|
given in argument at definition time.

To **define a suite**, one is expected to:

* define a |group| of tests and / or suites,
* define the suite using the |group| created above,
* optionally specify |fixture| function(s) and / or |timer|.

Using the :c:macro:`CUTE_SUITE` macro is the most straightforward way to define
a suite:

.. code-block:: c
   :linenos:
   :emphasize-lines: 16

   CUTE_TEST(sample_test_0)
   {
        /* Implement testing logic here */
   }

   CUTE_TEST(sample_test_1)
   {
        /* Implement testing logic here */
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_test_0),
        CUTE_REF(sample_test_1)
   };

   CUTE_SUITE(sample_suite, sample_group);

The code sample above implements a test group named ``sample_group`` which
aggregates 2 test cases named ``sample_test_0`` and ``sample_test_1``
respectively.
A test suite named ``sample_suite`` is defined using the :c:macro:`CUTE_SUITE`
which is passed the ``sample_group`` test group as argument.

As not specified, no particular |fixture| functions are attached to the
``sample_suite`` test suite, meaning that it will inherit them from an eventual
parent suite if explicitly registered.

In addition, as no test |timer| is specified, it will inherit its timeout
setting from an eventual parent suite if explicitly registered.

For additional flexibility, CUTe_ also allows to :

* attach :ref:`fixture <sect-user-test_hierarchy-fixture_inheritance>` functions
  to suites ;
* assign a :ref:`timeout setting <sect-user-test_hierarchy-timeout_inheritance>`
  to suites ;
* specify :ref:`global file scope <sect-user-test_hierarchy-suite_file_scope>`
  for suites.

.. index:: fixture;inheritance, fixture;suite, suite;fixture
.. _sect-user-test_hierarchy-fixture_inheritance:

Fixture inheritance
*******************

A test |suite| may be attached optional |fixture| operations to setup runtime
preconditions and / or postconditions of |test case|\s registered to it.

.. important::

   |fixture| operations registered to a |suite| **are not executed prior to /
   after running the suite** as it would for a |test case|.

   Suite |fixture| operations registering is just a mechanism allowing a |test
   case| to inherit ``setup()`` and ``teardown()`` operations from its nearest
   ancestor if it requests it at definition time.

Use the :c:macro:`CUTE_SUITE_DEFN` macro to define a fixture'd test suite:

.. code-block:: c
   :linenos:
   :emphasize-lines: 46,47

   static int value;

   static void sample_setup(void)
   {
        value = 1;
   }

   /*
    * Define `sample_test_0' test case with default inherited fixture operations
    * and timeout setting.
    */
   CUTE_TEST(sample_test_0)
   {
        cute_check_assert(value > 0);
   }

   static void sample_setup_1(void)
   {
        value = 5;
   }

   /*
    * Define `sample_test_1' test case with `sample_setup_1()` as setup()
    * fixture operation, inherited teardown() operation and timeout
    * setting.
    */
   CUTE_TEST_STATIC(sample_test_1,
                    sample_setup_1,
                    CUTE_INHR_TEARDOWN,
                    CUTE_INHR_TMOUT)
   {
        cute_check_assert(value == 5);
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_test_0),
        CUTE_REF(sample_test_1)
   };

   /*
    * Define `sample_fixtured_suite' test suite with attached setup() and
    * teardown() fixture operations as well as default test timeout.
    */
   static CUTE_SUITE_DEFN(sample_fixtured_suite,
                          sample_group,
                          sample_setup,
                          CUTE_NULL_TEARDOWN,
                          CUTE_DFLT_TMOUT);

In the example above, note how the ``sample_test_1`` |test case| is attached the
``sample_setup_1()`` function as ``setup()`` fixture operation. Conversely, the
``sample_test_0`` |test case| is defined with |fixture| operations inherited
from the ``sample_fixtured_suite`` test |suite|.
Thereby, running ``sample_fixtured_suite`` carries out the following sequence of
operations :

1. run ``sample_test_0`` :

   a. execute ``sample_setup()`` function inherited from the
      ``sample_fixtured_suite`` test |suite|,
   b. execute ``sample_setup_0`` test function,
   c. skip ``teardown()`` operation thanks to the :c:macro:`CUTE_NULL_TEARDOWN`
      setting inherited from the ``sample_fixtured_suite`` test |suite|,

2. run ``sample_test_1`` :

   a. execute ``sample_setup_1()``,
   b. execute ``sample_test_1`` test function,
   c. skip ``teardown()`` operation thanks to the :c:macro:`CUTE_NULL_TEARDOWN`
      setting inherited from ``sample_fixtured_suite`` test |suite|,

Note that you may also use the following alternate macros to register |fixture|
operations to |suite|\s :

* :c:macro:`CUTE_SUITE_STATIC`,
* :c:macro:`CUTE_SUITE_EXTERN`.

.. index:: timeout;inheritance, timeout;suite, suite;timeout,
           timer;inheritance, timer;suite, suite;timer
.. _sect-user-test_hierarchy-timeout_inheritance:

Timeout inheritance
*******************

To protect against situations where a |test case| or |fixture| function hangs,
CUTe_ may arm a test |timer| that interrupts current test case execution upon
expiry.

As for a |test case| |timer| setting, the timeout may be specified at |suite|
definition time using one of the following C macros :

* :c:macro:`CUTE_SUITE_DEFN`,
* :c:macro:`CUTE_SUITE_STATIC`,
* :c:macro:`CUTE_SUITE_EXTERN`.

All 3 macros take a timeout setting passed as ``_tmout`` argument and which may
be specified as one of :

* :c:macro:`CUTE_DFLT_TMOUT`, denoting a default timeout of 3 seconds ;
* :c:macro:`CUTE_INHR_TMOUT`, requesting a timeout inherited from the parent
  |suite| if existing and which falls back to :c:macro:`CUTE_DFLT_TMOUT`
  otherwise ;
* :c:macro:`CUTE_NONE_TMOUT`, to disable the timeout mechanism ;
* or a *non-zero unsigned integer*, specifying a timeout value expressed as
  seconds.

.. important::

   |timer| setting assigned to a |suite| **is not applied to the suite itself**
   as it would for a |test case|.

   Suite |timer| setting assignment is just a mechanism allowing a |test case|
   to inherit timeout setting from its nearest ancestor if it requests it at
   definition time.

When the timer expires, the current |test case| or |fixture| function execution
is *interrupted*, CUTe_ marks the test as *failing* then proceeds to the *next*
one in sequence within the current |suite|.

Use the :c:macro:`CUTE_SUITE_DEFN` macro to specify a timeout at definition time
:

.. code-block:: c
   :linenos:
   :emphasize-lines: 35

   /*
    * Define `sample_test_0' test case with default inherited fixture operations
    * and timeout setting.
    */
   CUTE_TEST(sample_test_0)
   {
        cute_check_assert(0 == 0);
   }

   /*
    * Define `sample_test_1' test case with default inherited fixture operations
    * and assign it a timeout of 1 second.
    */
   CUTE_TEST_STATIC(sample_test_1,
                    CUTE_INHR_SETUP,
                    CUTE_INHR_TEARDOWN,
                    1U)
   {
        cute_check_assert(1 == 1);
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_test_0),
        CUTE_REF(sample_test_1)
   };

   /*
    * Define `sample_timed_suite' test suite with default fixture operations
    * and a 10 seconds timeout.
    */
   static CUTE_SUITE_DEFN(sample_timed_suite,
                          sample_group,
                          CUTE_NULL_SETUP,
                          CUTE_NULL_TEARDOWN,
                          10U);

In the example above, note how the ``sample_test_1`` |test case| is assigned a
``1`` second timeout explicitly. Conversely, the ``sample_test_0`` |test case|
is defined with the timeout setting inherited from the ``sample_timed_suite``
test |suite| of ``10`` seconds.

.. index:: file scope;suite, suite;file scope
.. _sect-user-test_hierarchy-suite_file_scope:

Suite file scope
****************

To enhance |suite| reusability, CUTe_ allows to specify global
file scope at |suite| definition time. 2 shorthand macros allow to minimize test
developper workload compared to :c:macro:`CUTE_SUITE_DEFN` usage. These are :

* :c:macro:`CUTE_SUITE_STATIC`,
* :c:macro:`CUTE_SUITE_DECL` and :c:macro:`CUTE_SUITE_EXTERN`.

:c:macro:`CUTE_SUITE_STATIC` allows to define a |suite| with ``static`` global
file scope. :ref:`Registering <sect-user-test_hierarchy>` the created |suite| to
a parent one is then restricted to the source file where it is defined :

.. code-block:: c
   :linenos:
   :emphasize-lines: 9-13

   CUTE_GROUP(sample_group) = {
        /* Include a list of references to test cases or suites here. */
   };

   /*
    * Define `sample_static_suite' test suite with static global file
    * scope.
    */
   CUTE_SUITE_STATIC(sample_static_suite,
                     sample_group,
                     CUTE_INHR_SETUP,
                     CUTE_INHR_TEARDOWN,
                     CUTE_INHR_TMOUT);

:c:macro:`CUTE_SUITE_DECL` and :c:macro:`CUTE_SUITE_EXTERN` macros are meant to
work with reusable definitions.
Use :c:macro:`CUTE_SUITE_EXTERN` to define a |suite| with default global file
scope, i.e. with external linkage :

.. code-block:: c
   :linenos:
   :emphasize-lines: 11-15

   #include "test.h"

   CUTE_GROUP(sample_group) = {
        /* Include a list of references to test cases or suites here. */
   };

   /*
    * Define `sample_extern_suite' test suite with default global file
    * scope.
    */
   CUTE_SUITE_EXTERN(sample_extern_suite,
                     sample_group,
                     CUTE_INHR_SETUP,
                     CUTE_INHR_TEARDOWN,
                     CUTE_INHR_TMOUT);

And use :c:macro:`CUTE_SUITE_DECL` to declare the |suite| defined above in a
header so that it may be referenced from other compilation units :

.. code-block:: c
   :linenos:
   :emphasize-lines: 7

   #ifndef _TEST_H
   #define _TEST_H

   #include <cute/cute.h>

   /* Declare `sample_extern_suite' test suite with external linkage. */
   extern CUTE_SUITE_DECL(sample_extern_suite);

   #endif /* _TEST_H */

Now that we know how to instantiate a complete test |hierarchy|, let's see how
to implement test functions using CUTe_'s test assertions.

.. index:: assertion
.. _sect-user-assertion:

Test assertion
--------------

In addition to :c:func:`cute_skip` and :c:func:`cute_fail` functions, testing
logic of |fixture| and |test case| functions should rely upon *testing
assertions*.
These are an expressive way to verify behavior of the software under test. In
addition, while |report|\ing failures, they provide *descriptive messages* to
ease the process of debugging.

Basically, a failing testing assertion interrupts current test and marks it
as |failed|.
The simple sample code below shows how the ``sample_test`` |test case| calls
such testing assertions :

.. code-block:: c
   :linenos:

   #include <cute/cute.h>
   #include <cute/check.h>

   static const bool           bool_var = true;
   static const int            sint_var = -2;
   static const unsigned short ushrt_var = 5;
   static const void * const   ptr_var = (const void *)&sint_var;
   static const char * const   str_var = "a test string";

   CUTE_TEST(sample_test)
   {
        /* Assert that bool_var is true */
        cute_check_bool(bool_var, is, true);

        /* Assert that sint_var > -10 */
        cute_check_sint(sint_var, greater, -10);

        /* Assert that 0 <= ushrt_var <= 10 */
        cute_check_uint_range(ushrt_var, in, CUTE_SINT_RANGE(0U, 10U));

        /* Assert that ptr_var == &sint_var */
        cute_check_ptr(ptr_var, equal, (const void *)&sint_var);

        /* Assert that str_var string ends with "string" */
        cute_check_str(str_var, end, "string");
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_test),
   };

   CUTE_SUITE(sample_suite, sample_group);

Testing assertions are called using functions which name begins with
``cute_check_``. Note how all assertion function signatures shown above take 3
arguments where :

1. first argument must hold the variable to test,
2. second argument is a literal indicating which testing operation / comparison
   to perform,
3. third argument hold the reference variable used to compare the first argument
   with.

Refer to the |API| guide to find documentation about available types of testing
assertions :

* :ref:`sect-api-basic_assert`,
* :ref:`sect-api-bool_assert`,
* :ref:`sect-api-sint_assert`,
* :ref:`sect-api-uint_assert`,
* :ref:`sect-api-str_assert`,
* :ref:`sect-api-ptr_assert`,
* and :ref:`sect-api-mem_assert`.

Finally, the more versatile :c:func:`cute_check_assert` testing assertion is
also available at the expense of less descriptive failure messages :

.. code-block:: c
   :linenos:

   #include <cute/cute.h>
   #include <cute/check.h>

   static const int sint_var = -2;

   CUTE_TEST(sample_test)
   {
        /* Assert that sint_var > -10 */
        cute_check_assert(sint_var > -10);
   }

In addition to standard testing assertions, CUTe_ also provides mocking
expectations. These are described in the next section.

.. index:: mocking, expectation
.. _sect-user-mocking:

Mocking
-------

CUTe_ also provides the ability to test software that has external dependencies
(such as system or external library calls for example) according to a mocking_
strategy.
The purpose of mocking_ is to isolate the code being tested by emulating the
behavior and / or state of external dependencies. This allows to focus on
software under test and not on dependencies.

CUTe_ allows to replace dependencies by emulation code carefully crafted to
control inputs and outputs of software under test. Emulation is implemented
using so-called mock expectations.
An expectation is a 2 steps verification process. First step schedules a testing
assertion before executing a piece of software under test. Scheduled assertions
are then verified in a differed manner during software under test execution.

CUTe_ implements mock expectations according to a strict strategy. Expectations
are scheduled in a First-In First-Out manner. An expectation verification
failure interrupts the current |test case| and marks it as |failed|.
A |test case| based upon mock expectations is marked as passed_ only when all
of their respective verifications resolve to passed_.

To ensure proper function calls ordering, use :ref:`sect-api-call_xpct`\s as
shown below :

.. code-block:: c
   :linenos:

   #include <cute/cute.h>
   #include <cute/expect.h>

   static void
   external_callee_first(void)
   {
        cute_mock_call();
   }

   static void
   external_callee_second(void)
   {
        cute_mock_call();
   }

   static void
   code_to_test(void)
   {
        external_callee_first();
        external_callee_second();
   }

   CUTE_TEST(sample_call_order_test)
   {
        /*
         * Ensure that code_to_test() calls external_callee_first()
         * first ...
         */
        cute_expect_call(external_callee_first);
        /* ... then external_callee_second(). */
        cute_expect_call(external_callee_second);

        /* Run the software under test. */
        code_to_test();
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_call_order_test)
   };

   CUTE_SUITE(sample_suite, sample_group);

To check function parameter values, use :ref:`sect-api-parm_xpct`\s as shown
below :

.. code-block:: c
   :linenos:

   #include <cute/cute.h>
   #include <cute/expect.h>

   static void
   external_callee(int first, const char * second)
   {
        cute_mock_sint_parm(first);
        cute_mock_str_parm(second);
   }

   static void
   code_to_test(void)
   {
        external_callee(0, "zero");
   }

   CUTE_TEST(sample_parm_test)
   {
        /*
         * Ensure that code_to_test() calls external_callee() with 0 as
         * first argument ...
         */
        cute_expect_sint_parm(external_callee, first, equal, 0);
        /* and a string that begins with "ze" as second argument. */
        cute_expect_str_parm(external_callee, second, begin, "ze");

        /* Run the software under test. */
        code_to_test();
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_parm_test)
   };

   CUTE_SUITE(sample_suite, sample_group);

To program function return codes, use :ref:`sect-api-retval_xpct`\s as shown
below :

.. code-block:: c
   :linenos:

   #include <stdbool.h>
   #include <cute/cute.h>
   #include <cute/expect.h>

   static int
   external_callee(void)
   {
        static int sum = 0;

        sum += (int)cute_mock_sint_retval();

        return sum;
   }

   static int
   code_to_test(void)
   {
        int sum;

        sum = external_callee();
        sum += external_callee();

        return sum;
   }

   CUTE_TEST(sample_retval_test)
   {
        /*
         * Schedule 2 integer values to be returned by
         * external_callee().
         */
        cute_expect_sint_retval(external_callee, 1);
        cute_expect_sint_retval(external_callee, 2);

        /*
         * Run the software under test and ensure it returns the sum of
         * the 2 integer values above.
         */
        cute_check_sint(code_to_test(), equal, 3);
   }

   CUTE_GROUP(sample_group) = {
        CUTE_REF(sample_retval_test)
   };

   CUTE_SUITE(sample_suite, sample_group);

Finally, standard assertion failures may be verified using
:ref:`sect-api-assert_xpct`\s.

.. _sect-user-running_tests:

Running tests
-------------

Once a test |hierarchy| has been defined, some additional glue code is required
to instantiate and operate it.
To alleviate test developper workload, CUTe_ provides a single macro to use as
`main entry point`_ replacement that implements all the required glue code.

Finally, you should :ref:`compile and link <sect-user-building_tests>` the test
code to provide a standalone executable.

Where you need more flexibility, CUTe_'s |API| also exposes functions required
to instantiate and operate a test |hierarchy| step by step. See section
`Low-level runner`_ for more informations.

.. _sect-user-main:

Main entry point
****************

The :c:macro:`CUTE_MAIN` macro implements all the required steps to run a
test |hierarchy|.

It should be used as a replacement for a test |hierarchy| runner executable's
``main()`` entry point :

.. code-block:: c
   :linenos:
   :emphasize-lines: 14

   #include <cute/cute.h>

   CUTE_TEST(my_first_failing_test)
   {
        cute_fail(NULL);
   }

   CUTE_GROUP(my_first_group) = {
        CUTE_REF(my_first_failing_test)
   };

   CUTE_SUITE(my_first_suite, my_first_group);

   CUTE_MAIN(my_first_suite, "My first package", "0.1")

The code shown above implements a runnable test |hierarchy| composed of a single
root test |suite| ``my_first_suite`` aggregating a single |test case| named
``my_first_failing_test``.
See :c:macro:`CUTE_MAIN` description for details about its argument list.

:ref:`Building <sect-user-building_tests>` this test code should produce an
executable supporting the following features :

* :ref:`run <sect-user-run_suite>` selectable subsets of the test |hierarchy|,
* :ref:`show <sect-user-list_suite>` informations about selectable subsets of
  the test |hierarchy|,
* produce test |report|\s according to specified format(s),
* according to options given on the command line as described in section
  `Command line`_.

Basically, in addition to command line arguments parsing, :c:macro:`CUTE_MAIN`
makes use of functions publicly exposed by CUTe_ |API| to implement
its internal logic. These are described in the following sections...

.. _sect-user-low_level_runner:

Low-level runner
****************

Basically, to instantiate and operate a test |hierarchy| step by step :

* :ref:`initialize <sect-user-setup_run>` CUTe_,
* :ref:`run <sect-user-run_suite>` test |hierarchy|,
* or :ref:`show <sect-user-list_suite>` test |hierarchy|,
* then :ref:`release <sect-user-setup_run>` CUTe_ resources.

.. _sect-user-setup_run:

Setting up runner
^^^^^^^^^^^^^^^^^

Any usage of CUTe_'s library *must* :

* begin with a call to :c:func:`cute_init` to initialize |API| internals,
* finish with a call to :c:func:`cute_fini` to close |API| I/O streams and
  release allocated resources.

.. code-block:: c
   :linenos:
   :emphasize-lines: 9,14

   #include <cute/cute.h>
   #include <stdlib.h>

   int main(void)
   {
        int                err;
        struct cute_config config = CUTE_CONFIG_INIT;

        if (cute_init(&config, "My first package", "0.1"))
                return EXIT_FAILURE;

        /* Do something with CUTe... */

        cute_fini();

        return err ? EXIT_FAILURE : EXIT_SUCCESS;
   }

In the example show above, |API| is initialized thanks to a call to
:c:func:`cute_init` which is passed :

* as first argument, a default run configuration referenced by ``config`` ;
* as second argument, the name of the package for which testing code is
  written ;
* and the package version string as third argument.

The ``config`` variable is initialized using the :c:macro:`CUTE_CONFIG_INIT`
macro to configure the way the test |hierarchy| will run using default settings.
These settings are stored into a :c:struct:`cute_config` structure which
basically allows to :

* select subset(s) of a test |hierarchy| to run,
* setup :c:member:`debug mode <cute_config.debug>`,
* and test |report|\ing.

Finally, :c:func:`cute_fini` function is called to flush and close test
|hierarchy| reporting I/O streams and release |API| resources allocated
internally.

.. _sect-user-run_suite:

Running suites
^^^^^^^^^^^^^^

Once CUTe_'s |API| is :ref:`setup <sect-user-setup_run>` with a run
:c:struct:`configuration <cute_config>`, running a test |hierarchy| is
straightforward :

.. code-block:: c
   :linenos:
   :emphasize-lines: 14

   #include <cute/cute.h>
   #include <stdlib.h>

   extern CUTE_SUITE_DECL(my_first_suite);

   int main(void)
   {
        int                err;
        struct cute_config config = CUTE_CONFIG_INIT;

        if (cute_init(&config, "My first package", "0.1"))
                return EXIT_FAILURE;

        err = cute_run_suite(&my_first_suite);

        cute_fini();

        return err ? EXIT_FAILURE : EXIT_SUCCESS;
   }

The code above shows how to run the test |hierarchy| which top-level |suite| is
``my_first_suite`` using a call to :c:func:`cute_run_suite`.
While running, progress is shown according to :c:struct:`configuration
<cute_config>` given at :ref:`setup <sect-user-setup_run>` time.
See |report| section for explanations about displayed informations.

.. _sect-user-list_suite:

Listing suites
^^^^^^^^^^^^^^

Alternatively, you may display informations about the ``my_first_suite`` test
|hierarchy| as shown below :

.. code-block:: c
   :linenos:
   :emphasize-lines: 14

   #include <cute/cute.h>
   #include <stdlib.h>

   extern CUTE_SUITE_DECL(my_first_suite);

   int main(void)
   {
        int                err;
        struct cute_config config = CUTE_CONFIG_INIT;

        if (cute_init(&config, "My first package", "0.1"))
                return EXIT_FAILURE;

        cute_show_suite(&my_first_suite);

        cute_fini();

        return EXIT_SUCCESS;
   }

Calling the :c:func:`cute_show_suite` function above displays informations
according to the :c:struct:`configuration <cute_config>` given at
:ref:`setup <sect-user-setup_run>` time.
Refer to |report| section for explanations about displayed informations.

.. _sect-user-building_tests:

Building tests
**************

Basically, building a test |hierarchy| runner executable requires to compile and
link testing code compilation units with CUTe_'s library. You may choose to
link against either the *static* library or the *shared* one.

Here is basic `Makefile <make_>`_ examples for building a ``my_first_tests``
runner executable against *shared* and *static* libraries respectively.

.. code-block:: make
   :caption: Build against CUTe's shared library with handcoded build flags
   :linenos:

   CUTE_PREFIX     := /usr/local
   CUTE_INCLUDEDIR := ${CUTE_PREFIX}/include
   CUTE_LIBDIR     := ${CUTE_PREFIX}/lib

   CFLAGS          := -I${CUTE_INCLUDEDIR}
   LDFLAGS         := -L${CUTE_LIBDIR} -lcute

   my_first_tests: my_first_tests.c
   	${CC} -o $@ $< ${CFLAGS} ${LDFLAGS}

.. code-block:: make
   :caption: Build against CUTe's static library with handcoded build flags
   :linenos:

   CUTE_PREFIX     := /usr/local
   CUTE_INCLUDEDIR := ${CUTE_PREFIX}/include
   CUTE_LIBDIR     := ${CUTE_PREFIX}/lib

   CFLAGS          := -I${CUTE_INCLUDEDIR}
   LDFLAGS         := -L${CUTE_LIBDIR} \
                      -Wl,--push-state,-Bstatic -lcute -Wl,--pop-state

   my_first_tests: my_first_tests.c
   	${CC} -o $@ $< ${CFLAGS} ${LDFLAGS}

In addition, as CUTe_ is shipped with its own |pkg-config| metadata files, you
may either choose to make use of handcoded flags as shown above, or
to retrieve build flags through :program:`pkg-config` tool instead.

You will find below 2 `Makefile <make_>`_ examples for building the same runner
executable against *shared* and *static* libraries respectively using the
:program:`pkg-config` tool.

.. code-block:: make
   :caption: Build against CUTe's shared library with pkg-config tool
   :linenos:

   PKG_CONFIG := pkg-config
   CFLAGS     := ${shell ${PKG_CONFIG} --cflags libcute}
   LDFLAGS    := ${shell ${PKG_CONFIG} --libs libcute}

   my_first_tests: my_first_tests.c
   	${CC} -o $@ $< ${CFLAGS} ${LDFLAGS}

.. code-block:: make
   :caption: Build against CUTe's static library with pkg-config tool
   :linenos:

   PKG_CONFIG := pkg-config
   CFLAGS     := ${shell ${PKG_CONFIG} --static --cflags libcute}
   LDFLAGS    := ${shell ${PKG_CONFIG} --static \
                                       --libs-only-L \
                                       --libs-only-other libcute} \
                 -Wl,--push-state,-Bstatic \
                 ${shell ${PKG_CONFIG} --static --libs-only-l libcute} \
                 -Wl,--pop-state

   my_first_tests: my_first_tests.c
   	${CC} -o $@ $< ${CFLAGS} ${LDFLAGS}

Command line
************

|Build|\ing a test |hierarchy| runner using :c:macro:`CUTE_MAIN` as ``main()``
entry point replacement should produce an executable that allows to run tests
and / or suites according to arguments given on the command line.
Unless command line arguments parsing was modified by the test developper, the
produced executable should behave like what is shown for the fictional
:program:`sample-test` program hereafter.

.. program:: sample-test

.. rubric:: SYNOPSIS

| **sample-test** [<*OPTIONS*>] show [<*PATTERN*>]
| **sample-test** [<*OPTIONS*>] run [<*PATTERN*>]
| **sample-test** [-h|--help] [help]

.. rubric:: DESCRIPTION

When the ``show`` argument is given, test hierarchy is listed according to
the specified :option:`PATTERN`.

When the ``run`` argument is given, test hierarchy is executed according
to the specified :option:`PATTERN`. Process exits with an **error status** if
one of the tests failed or crashed.

Finally, the :option:`-h` and :option:`--help` options as well as the
``help`` argument displays a help message.

.. rubric:: OPTIONS

.. option:: -d, --debug

   Run in debug mode without neither exception handling nor timeouts.

   This may be useful when running a test hierarchy under a debugger (see
   |test case| section).

.. option:: -h, --help

   Output a help message.

.. option:: -i, --icase

   Ignore case when matching against <:option:`PATTERN`>.

.. option:: -s, --silent

   Silence all suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -t[<COLOR>], --terse[=<COLOR>]

   Enable minimal suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -v[<COLOR>], --verbose[=<COLOR>]

   Enable verbose suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -x[<PATH>], --xml[=<PATH>]

   Generate output to <:option:`PATH`> according to
   :ref:`JUnit XML format <sect-user-junit>`.

.. option:: -a[<PATH>], --tap[=<PATH>]

   Generate output to <:option:`PATH`> according to
   :ref:`Test Anything Protocol format <sect-user-tap>`.

.. option:: COLOR

   Controls the colorization of console output. When ``on``, it enforces
   colorization whereas it disables it when ``off``.

   By default, colorization is automatically enabled if current terminal supports
   it.

.. option:: PATH

   A pathname to a file where to store generated output.

   When unspecified or specified as ``-``, output is directed to standard output in
   which case this option is exclusive with :option:`-s`, :option:`--silent`,
   :option:`-t`, :option:`--terse`, :option:`-v` and :option:`--verbose`.

.. option:: PATTERN

   When given, selects a subset of the test hierarchy. When missing, the whole
   hierarchy is considered.

   It should be specified as a POSIX extended regular expression as described in
   |regex(7)|.

.. _sect-user-test_reports:

Test reports
============

Test results are output into reports where, for each |test case| or |suite|,
you may find usefull informations about test outcomes. A more in-depth
explanation about results may be found into section `test result`_.

Reporting may occur onto terminal and / or
may be stored into files. Multiple reporting formats are supported :

* `console`_ output formats allow progress monitoring ;
* :ref:`JUnit XML <sect-user-junit>` allows the recording of test results ;
* :ref:`Test Anything Protocol <sect-user-tap>` allows the streaming and
  recording of test results.

Reporting formats may be selected thanks to :

* a programmatic scheme allowing to :ref:`setup <sect-user-setup_run>` a test
  |hierarchy| runner using handcoded settings ;
* or through `command line`_ arguments when a runner implementation is based
  upon the :ref:`CUTE_MAIN <sect-user-main>` |API| macro.

.. note::

   Console output may be coupled with the ability to report results into files
   according to :ref:`JUnit XML <sect-user-junit>` and / or
   :ref:`TAP <sect-user-tap>` formats.
   See `Command line`_ section for more informations.

Console
-------

Console reports show general progress informations as well as details about
test failures onto `standard streams <stdios_>`_.
See section `statistics`_ for explainations about output details.

Console reporting may output progress informations in 3 different ways :
`terse`_, `verbose`_ or `silent`_.

Terse
*****

When unspecified, *Terse* is the default console output format. It shows :

* a progress bar (on ``stderr``),
* *failed* test details with fully qualified test names (on ``stdout``),
* and overall test result statistics_ at end of run (on ``stdout``).

The following command requests the :file:`cute-full-sample` executable shipped
with CUTe_ to produce **terse console output** in addition to generating a
:ref:`JUnit XML <sect-user-junit>` report into :file:`full_sample_junit.xml`
file :

.. code-block:: console

   $ cute-full-sample -t -xfull_sample_junit.xml run

This command should display something like :

.. literalinclude:: terse_report.txt
   :language: text

.. note::

   *Terse* console output may also be requested in a programmatic way by
   enabling :c:macro:`CUTE_CONFIG_TERSE_REPORT` into
   :c:member:`cute_config.reports`.

Verbose
*******

*Verbose* console outputs informations in a hierarchical manner and shows :

* *passed*, *skipped* and *disabled* test statuses, 
* *failed* and *crashed* test details,
* and overall test result statistics_ at end of run.

The following command requests the :file:`cute-full-sample` executable shipped
with CUTe_ to produce **verbose console output** in addition to generating a
:ref:`TAP <sect-user-tap>` report into :file:`full_sample.tap`
file :

.. code-block:: console

   $ cute-full-sample -v -afull_sample.tap run

This command should display something like :

.. literalinclude:: verbose_report.txt
   :language: text

.. note::

   *Verbose* console output may also be requested in a programmatic way by
   enabling :c:macro:`CUTE_CONFIG_VERB_REPORT` into
   :c:member:`cute_config.reports`.

Silent
******

As its name suggests, *silent* console produces no output (except for internal
CUTe_ errors).

The following command requests the :file:`cute-full-sample` executable shipped
with CUTe_ to produce **silent console output** in addition to generating a
:ref:`JUnit XML <sect-user-junit>` report into :file:`full_sample_junit.xml`
file :

.. code-block:: console

   $ cute-full-sample -s --xml=full_sample_junit.xml run

.. note::

   *Silent* console output may also be requested in a programmatic way by
   enabling :c:macro:`CUTE_CONFIG_SILENT_REPORT` into
   :c:member:`cute_config.reports`.

Statistics
**********

`Terse`_ and `verbose`_ console reports show an overall test results sumup at
end of run where test |hierarchy| :ref:`results <sect-user-result>` are
aggregated and presented in a hierarchical manner on a per |suite| basis. Here
is an excerpt of examples shown in previous sections :

.. literalinclude:: terse_report.txt
   :language: text
   :linenos:
   :lines: 28-42
   :emphasize-lines: 2-9

Lines 2 to 9 above show for each suite, 2 rows of informations according to a
tabular format which is depicted in the table below.

.. list-table::
   :header-rows: 1
   :stub-columns: 1

   * - Column name
     - Description
     - First row
     - Second row
   * - NAME
     - name of suite
     - short name
     - empty
   * - STAT
     - result status
     - status abbreviation
     - empty
   * - TIME
     - run time elapsed
     - :token:`seconds` . :token:`microseconds`
     - empty
   * - #PASS
     - number of direct descendants passed
     - absolute value
     - ratio computed as :math:`\frac{\#PASS\times100}{\#EXEC}`
   * - #SKIP
     - number of direct descendants skipped
     - absolute value
     - ratio computed as :math:`\frac{\#SKIP\times100}{\#EXEC}`
   * - #FAIL
     - number of direct descendant failures
     - absolute value
     - ratio computed as :math:`\frac{\#SKIP\times100}{\#EXEC}`
   * - #EXCP
     - number of direct descendant crashes (exceptions)
     - absolute value
     - ratio computed as :math:`\frac{\#EXCP\times100}{\#EXEC}`
   * - #EXEC
     - number of direct descendants executed, (not off)
     - absolute value
     - ratio computed as :math:`\frac{\#EXEC\times100}{\#TOTAL}`
   * - #TOTAL
     - total number of direct descendants registered.
     - absolute value
     - empty

Report is completed by lines 11 and 12 which present overall aggregated test
result statistics. Note that values displayed here account for **test case
related results ONLY**, i.e., suites are excluded from computation.

.. literalinclude:: terse_report.txt
   :language: text
   :linenos:
   :lines: 28-42
   :emphasize-lines: 11,12

This is the reason why, given a particular column, the sum of absolute values
found at lines 2 to 9 may differ from absolute value found at line 11.

.. index:: report;JUnit, report;XML
           JUnit;report, XML;report
.. _sect-user-junit:

JUnit XML
---------

Reports may be generated according to JUnit_ XML format. As stated
onto `Testmo's junitxml Github <https://github.com/testmoapp/junitxml>`_ :

   The JUnit XML file format has been made popular by the JUnit and has since
   become the de facto standard format to exchange test results between tools.
   JUnit-style XML files are generated by countless testing frameworks and are
   processed by CI tools, build servers and test management software to read and
   display test results.

   There is no official specification for the JUnit XML file format and various
   tools generate and support different flavors of this format. The goal of this
   project here is to document a common set of elements, attributes and
   conventions supported by many tools2. If you are building software that
   produces or consumes JUnit-style XML files, this project might be a good
   starting point to learn about the format.

JUnit_ XML format is quite descriptive and allows the recording of test results
for long-term statistical analysis. In addition, it allows for merging test
results from multiple files across multiple test
:ref:`hierarchies <sect-user-test_hierarchy>`.

The following command requests the :file:`cute-full-sample` executable shipped
with CUTe_ to produce **JUnit XML output onto the console** :

.. code-block:: console

   $ cute-full-sample -x run

The above command should display something like what is shown below. For more
informations about the format itself and how to understand it, see
`Testmo's junitxml README <https://github.com/testmoapp/junitxml/blob/main/README.md>`_.

.. literalinclude:: junit_report.xml
   :language: xml

.. note::

   *JUnit XML* console output may also be requested in a programmatic way by
   enabling :c:macro:`CUTE_CONFIG_XML_REPORT` into
   :c:member:`cute_config.reports`.

.. index:: report;TAP, report;Test Anything Protocol,
           TAP;report, Test Anything Protocol;report
.. _sect-user-tap:

TAP
---

Reports may also be generated according to *Test Anything Protocol* format
*version 14*. As stated onto `TAP home page`_ :

   TAP, the Test Anything Protocol, is a simple text-based interface between
   testing modules in a test harness. It decouples the reporting of errors from
   the presentation of the reports.

   One of its major uses is for noise reduction; when you have a suite of many
   tests, making them TAP producers and using a TAP consumer to view them helps
   ensures that you will see everything you need to notice and diagnose breakage
   without being distracted by a flood of irrelevant success messages. It can
   assist other forms of analysis and statistics-gathering as well.

   TAP started life as part of the test harness for Perl but now has
   implementations in C, C++, Python, PHP, Perl, Java, JavaScript, Go, Rust, and
   others. Consumers and producers do not have to be written in the same
   language to interoperate.

TAP format is suitable for both test result streaming and storage. Streaming
may be usefull when spawing tests remotely and one wants to monitor test suites
progress. On the other hand, the results storage ability is required for
long-term statistical analysis.

The following command requests the :file:`cute-full-sample` executable shipped
with CUTe_ to produce **TAP output onto the console** :

.. code-block:: console

   $ cute-full-sample -a run

The above command should display something like what is shown below. For more
informations about the format itself and how to understand it, see
`TAP specification v14 <https://testanything.org/tap-version-14-specification.html>`_.

.. literalinclude:: tap_report.tap
   :language: tap

.. note::

   *TAP* console output may also be requested in a programmatic way by
   enabling :c:macro:`CUTE_CONFIG_TAP_REPORT` into
   :c:member:`cute_config.reports`.

.. index:: result, test;result, suite;result, assertion;result
.. _sect-user-result:

Test result
-----------

For each |test case| or |suite|, result is reported using multiple kinds of
informations. These are described below.

.. index:: status, result;status, test;status, suite;status

status
  A synthetic *state* depicting the outcome of a test or suite ; possible
  statuses are :

  * *pass* : a test case or suite passed entirely ;
  * *skip* : a test case or suite was skipped programmatically, i.e thanks to a
    call to :c:func:`cute_skip` ;
  * *fail* : a test case failed or a suite has at least one failed or crashed
    sub-test / sub-suite ; may also happen when :c:func:`cute_fail` is called
    explicitly ;
  * *excp* : a test case crashed or all sub-tests / sub-suites of a suite
    crashed ;
  * *off* : a test case has been deselected or all sub-tests / sub-suites of a
    suite have been deselected because of an unsuccessful regular expression
    matching (see :option:`command line test selection <PATTERN>` and
    :c:member:`programmatic test selection <cute_config.match>`).

.. index:: issue, result;issue, test;issue, suite;issue

issue
  The nature of a particular test event, i.e. *what* happened.

.. index:: reason, result;reason, test;reason, suite;reason

reason
  Basically explain *why* a particular test issue happened.

.. index:: source, result;source, test;source, suite;source, assertion;source

source
  Source code location where a particular `test assertion` is called or a |test
  case| or |suite| is defined; expressed as :token:`pathname`::token:`line`.

.. index:: caller, result;caller, assertion;caller, expectation;caller

caller
  Name of the function that called a particular `test assertion`_.

There are additional types of reported informations that are specific to the
failing `test assertion`_ that procuded them.
