.. SPDX-License-Identifier: GPL-3.0-only
   
   This file is part of CUTe.
   Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>

.. include:: /_cdefs.rst

.. |user guide| replace:: CUTe_\’s :doc:`User guide </user>`

.. program:: cute-run

***********
cute-run(1)
***********

Name
====

cute-run - CUTe test runner executable command line

Synopsis
========

.. parsed-literal::

   :program:`cute-run` [info_options] info_ [<:option:`FULLNAME`>]
   :program:`cute-run` [show_options] show_ [<:option:`PATTERN`>]
   :program:`cute-run` [run_options] run_ [<:option:`PATTERN`>]
   :program:`cute-run` [help_option] help_

   *show_options*    := [icase_option] [info_options]
   *info_options*    := <terse_console> | <verbose_console>
   *run_options*     := [debug_option] [icase_option]
                      [<silent_console> | <terse_console> | <verbose_console> | <xml_console> | <tap_console>]
                      [<tap_store> | <xml_store>]
   *help_option*     := :option:`-h` | :option:`--help`


   *debug_option*    := :option:`-d` | :option:`--debug`
   *icase_option*    := :option:`-i` | :option:`--icase`

   *silent_console*  := :option:`-s` | :option:`--silent`
   *terse_console*   := :option:`-t`\[<:option:`COLOR`>] | :option:`--terse`\[=<:option:`COLOR`>]
   *verbose_console* := :option:`-v`\[<:option:`COLOR`>] | :option:`--verbose`\[=<:option:`COLOR`>]
   *COLOR*           := ``on`` | ``off`` | ``auto``

   *xml_console*     := :option:`-x` | :option:`--xml`
   *tap_console*     := :option:`-a` | :option:`--tap`

   *tap_store*       := :option:`-a`\<:option:`PATH`> | :option:`--tap`\=<:option:`PATH`>
   *xml_store*       := :option:`-x`\<:option:`PATH`> | :option:`--xml`\=<:option:`PATH`>

Description
===========

.. _info:

When the ``info`` argument is given, a description is shown for the suite or
test specified by :option:`FULLNAME` as stated into the section
|Test description| of |user guide|.

.. _show:

When the ``show`` argument is given, test |hierarchy| is listed according to
the optional :option:`PATTERN` argument.
It is also usefull to display tests that are candidates for running (without
actually running them at all) as the :option:`PATTERN` argument is handled the
same way than for the run_ subcommand.

.. _run:

When the ``run`` argument is given, test hierarchy is executed according
to the optional :option:`PATTERN` argument. Testing progress and result is
reported according to *run_options* given on the command line and defined into
section Synopsis_. Refer to section :ref:`Test reports <sect-user-test_reports>`
of |user guide| for more informations about reporting configuration.
Process terminates with a ``0`` (success) exit code when all tests have passed
(or have been skipped). It terminates with a ``1`` (failure) exit code
otherwise.

.. _help:

Finally, the :option:`-h` and :option:`--help` options as well as the
``help`` argument displays a help message.

Arguments
=========

.. option:: COLOR

   Controls the colorization of console output. Possible values are:

   * ``on``, to enforce colorization,
   * ``off``, to disable colorization,
   * ``auto``, the default, automatically enables colorization when current
     terminal supports it.

.. only:: not man

   .. option:: FULLNAME

      Specify a test |hierarchy| node's full name to select a single testcase or
      testsuite. When missing, the top-level suite is considered. See section
      :ref:`Test naming <sect-cute-report-test_naming>` of
      :doc:`cute-report(1) <cute-report>` man page for more informations.

.. only:: man

   .. option:: FULLNAME

      Specify a test |hierarchy| node's full name to select a single testcase or
      testsuite. When missing, the top-level suite is considered. See section
      :ref:`Test naming <sect-cute-report-test_naming>` of
      **cute-report(1)** man page for more informations.

.. option:: PATH

   A pathname to a file where to store generated output.
   When specified as ``-``, output is directed to standard output
   in which case this option is exclusive with :option:`-s`, :option:`--silent`,
   :option:`-t`, :option:`--terse`, :option:`-v` and :option:`--verbose`.

.. only:: not man

   .. option:: PATTERN

      When given, select a subset of the test |hierarchy| to run_ or show_. When
      missing, the whole |hierarchy| is considered.
      It should be specified as a POSIX extended regular expression as described
      in :manpage:`regex(7)`. Matching is performed against test full names,
      **looking for substrings** where :option:`PATTERN` produces a match. As a
      consequence, to search for an **exact** full name, enclose the regular
      expression with a leading caret and a trailing dollar (``^like_this$``) as
      if matching an entire line.
      See section :ref:`Test naming <sect-cute-report-test_naming>` of
      :doc:`cute-report(1) <cute-report>` man page for more informations about
      full names.

.. only:: man

   .. option:: PATTERN

      When given, select a subset of the test |hierarchy| to run_ or show_. When
      missing, the whole |hierarchy| is considered.
      It should be specified as a POSIX extended regular expression as described
      in :manpage:`regex(7)`. Matching is performed against test full names,
      **looking for substrings** where :option:`PATTERN` produces a match. As a
      consequence, to search for an **exact** full name, enclose the regular
      expression with a leading caret and a trailing dollar (``^like_this$``) as
      if matching an entire line.
      See section :ref:`Test naming <sect-cute-report-test_naming>` of
      **cute-report(1)** man page for more informations about full names.

Options
=======

.. option:: -d, --debug

   Run in debug mode without neither exception handling nor timeouts.
   This may be useful when running a test hierarchy under a debugger (see
   |test case| section of |user guide|).

.. option:: -h, --help

   Output a help message.

.. option:: -i, --icase

   Ignore case when matching against <:option:`PATTERN`>.

.. option:: -s, --silent

   Silence all suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -t, --terse, -t<COLOR>, --terse=<COLOR>

   Enable minimal suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -v, --verbose, -v<COLOR>, --verbose=<COLOR>

   Enable verbose suites and tests console output.

   :option:`-s`, :option:`--silent`, :option:`-t`, :option:`--terse`,
   :option:`-v`, and :option:`--verbose` options are exclusive.

.. option:: -x, --xml, -x<PATH>, --xml=<PATH>

   Generate output according to :ref:`JUnit XML format <sect-user-junit>` into
   <:option:`PATH`> file when specified. Output onto console otherwise.

.. option:: -a, --tap, -a<PATH>, --tap=<PATH>

   Generate output according to
   :ref:`Test Anything Protocol format <sect-user-tap>` into <:option:`PATH`>
   file when specified. Output onto console otherwise.


Exit status
===========

When given the ``run`` subcommand, :program:`cute-run` terminates with a ``0``
(success) exit code when all tests have passed (or have been skipped). It
terminates with a ``1`` (failure) exit code otherwise.

For all other subcommands :program:`cute-run` terminates with a non zero
failure exit code when :option:`FULLNAME` or :option:`PATTERN` arguments match
no existing test |hierarchy| node.

See also
========

.. only:: not man

   :doc:`cute-report(1) <cute-report>`

.. only:: man

   **cute-report(1)**

.. include:: /man/see_also.rst
