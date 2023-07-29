.. include:: _cdefs.rst

.. _tdd:            https://en.wikipedia.org/wiki/Test-driven_development
.. _bdd:            https://en.wikipedia.org/wiki/Behavior-driven_development
.. _regtest:        https://en.wikipedia.org/wiki/Regression_testing
.. _fixture:        https://en.wikipedia.org/wiki/Test_fixture
.. _mock:           https://en.wikipedia.org/wiki/Mock_object
.. _tap:            https://testanything.org/
.. _junit:          https://en.wikipedia.org/wiki/JUnit
.. _glibc:          https://www.gnu.org/software/libc/
.. _umlclass:       https://en.wikipedia.org/wiki/Class_diagram
.. |longjmp(3)|     replace:: :manpage:`longjmp(3)`
.. |fork(2)|        replace:: :manpage:`fork(2)`
.. |signal(7)|      replace:: :manpage:`signal(7)`
.. |fixture|        replace:: :ref:`fixture <sect-user-writing_tests-fixture_operations>`
.. |timer|          replace:: :ref:`timer <sect-user-writing_tests-test_timeout>`
.. |test case|      replace:: :ref:`test case <sect-user-writing_tests-test_definition>`
.. |file scope|     replace:: :ref:`file scope <sect-user-writing_tests-file_scope>`
.. |hierarchy|      replace:: :ref:`hierarchy <sect-user-test_hierarchy>`
.. |build|          replace:: :ref:`build <sect-user-building_tests>`
.. |run|            replace:: :ref:`run <sect-user-running_tests>`
.. |regex(7)|       replace:: :manpage:`regex(7)`

Overview
========

|CUTe| is a lightweight unit testing and mocking framework for C.

Unit testing is a software development practice where small units of code are
tested for proper operation. The technique is carried out as part of software
development process testing activities.
`Test-Driven Development (TDD) <tdd_>`_ and
`Behavior-Driven Development (BDD) <bdd_>`_ are well known examples of such
development processes making heavy usage of unit testing.

|CUTe| is meant to ease the process of writing unit tests and make them suitable
for test automation and `regression testing <regtest_>`_ activities. It is
designed with the following goals in mind:

.. rubric:: Features

* test failure recovery
* extensive reporting
* flexible and modular test suite hierarchies
* `test fixtures <fixture_>`_
* strict function `mocking <mock_>`_
* `JUnit <junit_>`_ XML reporting
* `Test Anything Protocol <tap_>`_ reporting
* ease debugging of test failures
* clear and expressive C API
* C library dependency only

.. rubric:: Limitations

* GNU / Linux / `Glibc <glibc_>`_ userspace C unit testing only
* |longjmp(3)| based test failure recovery only
* no |fork(2)| based test isolation
* no parameterizable test support
* custom check assertion not supported

Basic concepts
==============

|CUTe| is a framework allowing to write, manage and run unit tests in C. It's
:ref:`API <sect-api-overview>` provides a set of functions to :

* structure test case / suite |hierarchy| ;
* check strongly typed data against specified constraints ;
* schecule and verify mock expectations against specified constraints ;
* |run| test cases and suites according to specified configurations ;
* report test results onto the console and / or,
* into files according to various specified formats.

|CUTe| is built as a static and / or shared library which must be linked with
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

A suite is a collection of suites and / or test cases. All suites may
arbitrarily be selected for a |run|.

Finally, a particular |run| may be setup to :ref:`report
<sect-user-test_reports>` test results according to a specified configuration.

Typical Workflow
================

As a **developper**, implementing tests involves the following sequence of
steps:

.. rubric:: Preparatory phase:

#. write a dummy failing |test case| ;
#. setup a primary test |hierarchy| including a single root suite and the test
   just created ;
#. |build| and |run| to check for proper operation ;
#. modify the test written initially to implement the first real test case ;

.. rubric:: Iterative test implementation phases:

#. |build| and |run| ;
#. fix test failures ;
#. implement additional |test case| ;
#. when required, refine test |hierarchy| by defining test suites.

As an **integrator**, you may be required to tweak test suites |run|
configuration for proper integration within test regression / automation
infrastructure.

Writing tests
=============

.. index:: test definition, test case definition
.. _sect-user-writing_tests-test_definition:

Test definition
---------------

To **define a test**, one is expected to:

* give the test a name,
* provide a function / block of instructions that defines the testing logic,
* optionally specify test |fixture| function(s) and / or |timer|.

At running time, the test function is executed with the following
**assumptions** :

* the ``setup()`` |fixture| function (if any) has completed,
* a |timer| has been armed to protect against situations where testing
  logic hangs,
* ``SIGFPE``, ``SIGILL``, ``SIGSEGV``, ``SIGBUS`` and ``SIGSYS`` |signal(7)|
  dispositions have been setup to catch potential testing logic exceptions /
  traps / crashes,
* the ``teardown()`` |fixture| function (if any) is always executed after test
  function has completed.
* the test function does not alter |CUTe|'s internal state consistency.

As state above, result is unpredictable when the test function alters |CUTe|'s
internal state consistency. In particular, the **test function is not allowed
to** :

* modify test |hierarchy| ;
* |run| tests or suites ;
* alter |signal(7)| dispositions installed by |CUTe|.

Using the :c:macro:`CUTE_TEST` macro is the most straightforward way to define
a test:

.. code-block:: c

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
parent suite timeout settings at registering time.

Just in case you need to debug the testing logic, you should remind that
the block of instructions above is used to define a test function assigned to
the ``sample_test`` test case as following :

.. code-block:: c

   static void sample_test__cute_exec(void);

For additional flexibility, |CUTe| also allows to :

* attach |fixture| functions to tests and / or suites ;
* assign a |timer| to tests and / or suites ;
* specify global |file scope| for tests and / or suites.

.. index:: fixture, fixture operation, fixture function
.. _sect-user-writing_tests-fixture_operations:

Fixture operations
------------------

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

.. index:: timeout, test timeout, timer, test timer
.. _sect-user-writing_tests-test_timeout:

Test timeout
------------

To protect against situations where a |test case| or |fixture| function hangs,
|CUTe| may arm a test timer that interrupts current test case execution upon
expiry.

The timeout may be specified at |test case| definition time using one of the
following C macros :

* :c:macro:`CUTE_TEST_DEFN`,
* :c:macro:`CUTE_TEST_STATIC`,
* :c:macro:`CUTE_TEST_EXTERN`.

All 3 macros take a timeout setting passed as ``_tmout`` argument and which may
be specified as one of :

* :c:macro:`CUTE_DFLT_TMOUT`, denoting a default timeout of 3 seconds ;
* :c:macro:`CUTE_INHR_TMOUT`, requesting a timeout inherited from the parent
  suite if existing and which falls back to :c:macro:`CUTE_DFLT_TMOUT`
  otherwise ;
* :c:macro:`CUTE_NONE_TMOUT`, to disable the timeout mechanism ;
* or an *unsigned integer*, specifying a timeout value expressed as seconds.

When the timer expires, the current |test case| or |fixture| function execution
is *interrupted*, |CUTe| marks the test as *failing* then proceeds to the *next*
one in sequence.

Use the :c:macro:`CUTE_TEST_DEFN` macro to specify a timeout at definition
time :

.. code-block:: c

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

.. index:: file scope, test scope
.. _sect-user-writing_tests-file_scope:

File scope
----------

To enhance test case reusability, |CUTe| allows to specify global file scope
at test definition time. 2 shorthand macros allow to minimize test developper
workload compared to :c:macro:`CUTE_TEST_DEFN` usage. These are :

* :c:macro:`CUTE_TEST_STATIC`,
* :c:macro:`CUTE_TEST_DECL` and :c:macro:`CUTE_TEST_EXTERN`.

:c:macro:`CUTE_TEST_STATIC` allows to define a test with ``static`` global file
scope. :ref:`Registering <sect-user-test_hierarchy>` the created test to a suite
is then restricted to the source file where the suite is defined :

.. code-block:: c

   /*
    * Define `sample_static_test' test case with static global file
    * scope.
    */
   CUTE_TEST_STATIC(sample_static_test,
                    CUTE_INHR_SETUP,
                    CUTE_INHR_TEARDOWN,
                    CUTE_INHR_TMOUT);
   {
        /* Implement testing logic here */
   }

:c:macro:`CUTE_TEST_DECL` and :c:macro:`CUTE_TEST_EXTERN` macros are meant to
work with reusable definitions.
Use :c:macro:`CUTE_TEST_EXTERN` to define a test with default global file
scope, i.e. with external linkage :

.. code-block:: c

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
==============

Test hierarchy is structured around test suites. A suite is basically a
container for (sub-)tests and / or (sub-)suites.
Once a
:ref:`suite has been defined <sect-user-test_hierarchy-suite_definition>`,
(sub-)tests and / or (sub-)suites may be registered to it thanks to a `group
definition`_.

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
          class Base
          class Suite
      }

      Base "*" --o "1" Suite
      Group . (Suite, Base)
      Base <|-- Suite
      Base <|-- Test

      @enduml

To create a test hierarchy, follow the typical workflow hereafter :

* define |test case|\s,
* :ref:`define a group <sect-user-test_hierarchy-group_definition>` referencing
  tests defined above,
* :ref:`define a suite <sect-user-test_hierarchy-suite_definition>` based on the
  group defined above.

Let's see how to define a group...

.. index:: group definition, test group definition
.. _sect-user-test_hierarchy-group_definition:

Group definition
----------------

A test group is basically an ordered collection of test cases and / or suites.

Using the :c:macro:`CUTE_GROUP` macro is the most straightforward way to define
a group:

.. code-block:: c

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
name ``sample_group`` that aggregates both test cases created above.
You may later :ref:`define suite(s) <sect-user-test_hierarchy-suite_definition>`
by using the variable named ``sample_group`` which itself refers to the created
group.

This test group variable is defined with *static global file scope*. However,
for additional flexibility and reusability, |CUTe| also provides the following
macros that allow to specify a desired global file scope :

* :c:macro:`CUTE_GROUP_STATIC`,
* :c:macro:`CUTE_GROUP_EXTERN`,
* :c:macro:`CUTE_GROUP_DEFN` and :c:macro:`CUTE_GROUP_DECL`.

Now that a group has been defined, let's see how to use it to define a suite...

.. _sect-user-test_hierarchy-suite_definition:

Suite definition
----------------

Fixture inheritance
-------------------

Timeout inheritance
-------------------

Instantiation
-------------

Test assertions
===============

.. _sect-user-building_tests:

Building tests
==============

.. _sect-user-running_tests:

Running tests
=============

|Build|'ing a test hierarchy which instantiation_ is based upon
:c:func:`cute_main` and / or :c:macro:`CUTE_MAIN`, should produce an executable
that allows to run tests and / or suites according to arguments given on the
command line.

Unless command line arguments parsing was modified by the test developper, the
produced executable should behave like what is shown for the fictional
:program:`sample-test` program hereafter.

.. rubric:: SYNOPSIS

| **sample-test** [<*OPTIONS*>] show [<*PATTERN*>]
| **sample-test** [<*OPTIONS*>] run [<*PATTERN*>]
| **sample-test** [-h|--help] [help]

.. rubric:: DESCRIPTION

When the :option:`show` argument is given, test hierarchy is listed according to
the specified :option:`PATTERN`.

When the :option:`run` argument is given, test hierarchy is executed according
to the specified :option:`PATTERN`.

Finally, the :option:`-h` and :option:`--help` options as well as the
:option:`help` argument displays a help message.

.. rubric:: PATTERN

When given, :option:`PATTERN` selects a subset of the test hierarchy. When
missing, the whole hierarchy is considered.

:option:`PATTERN` should be specified as a POSIX extended regular expression as
described in |regex(7)|.

.. rubric:: OPTIONS

.. program:: sample-test

.. option:: -d|--debug

   Run in debug mode without neither exception handling nor timeouts.

   This may be useful when running a test hierarchy under a debugger (see
   |test case| section).

.. option:: -i|--icase

   Ignore case when matching against <*PATTERN*>.

.. option:: -s|--silent

   Silence all suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -t[<COLOR>]|--terse[=<COLOR>]

   Enable minimal suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -v[<COLOR>]|--verbose[=<COLOR>]

   Enable verbose suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -x[<PATH>]|--xml[=<PATH>]

   Generate output to <*PATH*> according to JUnit XML format.

.. option:: -a[<PATH>]|--tap[=<PATH>]

   Generate output to <*PATH*> according to Test Anything Protocol format.

.. rubric:: PATH

:option:`PATH` is a pathname to a file where to store generated output.

When unspecified or specified as ``-``, output is directed to standard output in
which case this option is exclusive with :option:`-s`, :option:`--silent`,
:option:`-t`, :option:`--terse`, :option:`-v` and :option:`--verbose`.

.. rubric:: COLOR

:option:`COLOR` controls the colorization of console output. When ``on``,
it enforces colorization whereas it disables it when ``off``.

By default, colorization is automatically enabled if current terminal supports
it.

.. _sect-user-test_reports:

Test reports
============

Console
-------

JUnit XML
---------

TAP
---
