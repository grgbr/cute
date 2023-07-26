.. include:: _cdefs.rst

.. _tdd:          https://en.wikipedia.org/wiki/Test-driven_development
.. _bdd:          https://en.wikipedia.org/wiki/Behavior-driven_development
.. _regtest:      https://en.wikipedia.org/wiki/Regression_testing
.. _fixture:      https://en.wikipedia.org/wiki/Test_fixture
.. _mock:         https://en.wikipedia.org/wiki/Mock_object
.. _tap:          https://testanything.org/
.. _junit:        https://en.wikipedia.org/wiki/JUnit
.. _glibc:        https://www.gnu.org/software/libc/
.. |longjmp(3)|   replace:: :manpage:`longjmp(3)`
.. |fork(2)|      replace:: :manpage:`fork(2)`
.. |signal(7)|    replace:: :manpage:`signal(7)`
.. |fixture|      replace:: :ref:`fixture <sect-user-writing_tests-fixture_operations>`
.. |timer|        replace:: :ref:`timer <sect-user-writing_tests-test_timeout>`

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
:ref:`API <sect-api-overview>` provides a set functions to :

* build test case / suite hierarchies ;
* check strongly typed data against specified constraints ;
* schecule and verify mock expectations against specified constraints ;
* run test cases and suites according to specified configurations ;
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
arbitrarily be selected for :ref:`running <sect-user-running_tests>`.

Finally, a particular run may be setup to :ref:`report <sect-user-test_reports>`
test results according to a specified configuration.

Typical Workflow
================

As a **developper**, implementing tests involves the following sequence of
steps:

* preparatory phase:

  #. :ref:`write <sect-user-writing_tests-test_definition>` a dummy failing
     test,
  #. setup a primary test hierarchy including a single root suite and the test
     just created
  #. build and run to check for proper operation
  #. modify the test written initially to implement the first real test case

* iterative test implementation:

  #. build and run
  #. fix test failures
  #. implement additional test cases
  #. when required, refine test hierarchy by defining test suites

As an **integrator**, you may be required to tweak test suites :ref:`run
<sect-user-running_tests>` configuration for proper integration within test
regression / automation infrastructure.

Writing tests
=============

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

* modify test hierarchy ;
* run tests or suites ;
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

For additional flexibility, |CUTe| also allows to attach |fixture| functions to
test cases.

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

The ``setup()`` function initializes the testing context to a deterministic
state whereas the ``teardown()`` function is used to restore testing context
that existed prior to ``setup()`` execution.

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

.. _sect-user-writing_tests-test_timeout:

Test timeout
------------

Test scope
----------

Test hierarchy
==============

Suite definition
----------------

Fixture inheritance
-------------------

Timeout inheritance
-------------------

.. _sect-user-running_tests:

Running tests
=============

Test selection
--------------

Output settings
---------------

Debug mode
----------

.. _sect-user-test_reports:

Test reports
============

Console settings
----------------

JUnit XML
---------

TAP
---
