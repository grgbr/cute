.. SPDX-License-Identifier: GPL-3.0-only
   
   This file is part of CUTe.
   Copyright (C) 2024 Grégor Boirie <gregor.boirie@free.fr>

.. include:: /_cdefs.rst

.. _xml schema: https://en.wikipedia.org/wiki/XML_schema
.. _jenkins:    https://www.jenkins.io
.. _short name: `test naming`_
.. _full name:  `test naming`_

.. program:: cute-report

**************
cute-report(1)
**************

Name
====

cute-report - CUTe test results management and reporting tool with JUnit support

.. _synopsis:

Synopsis
========

.. parsed-literal::

   :program:`cute-report` verify_ [help_option] <:option:`PATH`>
   :program:`cute-report` sumup_ [show_options] [fields_option] <:option:`PATH`> [<:option:`SELECTOR`>]
   :program:`cute-report` result_ [show_options] <:option:`PATH`> [<:option:`NAME`>]
   :program:`cute-report` info_ [show_options] <:option:`PATH`> [<:option:`NAME`>]

   :program:`cute-report` union_ [update_options] <:option:`DBPATH`> <:option:`PATH`> <:option:`NAME`>
   :program:`cute-report` join_ [update_options] <:option:`DBPATH`> <:option:`PATH`> <:option:`NAME`>
   :program:`cute-report` del_ <:option:`DBPATH`> <:option:`FULLNAME`>

   :program:`cute-report` list_ [show_options]
   :program:`cute-report` [help_option]

   *show_options*   := [help_option] [color_option]

   *color_option*   := { :option:`-c` | :option:`--color` } <*color*>
   *color*          := ``on`` | ``off`` | ``auto``

   *fields_option*  := { :option:`-f` | :option:`--fields` } <field_list>
   *field_list*     := [[field_list],]<:option:`FIELD`>

   *update_options* := [help_option]
                     [ { :option:`-a` | :option:`--package` } <:option:`PACKAGE`> ]
                     [ { :option:`-r` | :option:`--revision` } <:option:`REVISION`> ]
                     [ { :option:`-p` | :option:`--parent` } <:option:`FULLNAME`> ] 

   *help_option*    := :option:`-h` | :option:`--help`

Description
===========

:program:`cute-report` is a command line tool that allows to manipulate and
report results of testsuites that are stored into JUnit_ XML files. It provides
multiple subcommands :

* verify_, sumup_, result_ and info_ report results ;
* union_, join_ and del_ modify results |hierarchy| ;
* list_ displays supported result information fields.

.. _verify:

When verify_ is given as first argument, :program:`cute-report` displays no
output and terminates with a ``0`` (success) exit code when all tests found into
the :option:`PATH` file have passed. It terminates with a ``1`` (failure) exit
code otherwise.

.. _sumup:

When sumup_ is given as first argument, :program:`cute-report` shows a tabular
view of test results found into the :option:`PATH` file.  The optional
:option:`SELECTOR` argument may be given to select a subset of results to show.

.. _result:

When result_ is given as first argument, :program:`cute-report` shows result
of a single testcase or testsuite run as found into the :option:`PATH` file.
Testcase or testsuite is selected by name according to the optional third
argument :option:`NAME`. When unspecified, the overall result of all test runs
is shown instead.

.. _info:

When info_ is given as first argument, :program:`cute-report` shows test
|hierarchy| related informations of a single testcase or testsuite as found into
the :option:`PATH` file. Testcase or testsuite is selected by name according to
the optional third argument :option:`NAME`. When unspecified, informations are
shown for the entire test results store instead.

.. _union:

When union_ is given as first argument, :program:`cute-report` performs a
union of :option:`PATH` file content into :option:`DBPATH` file. Operation is
implemented as following:

* search for the top-level node of |hierarchy| found into the :option:`PATH`
  file ; let's call this node the *node to attach* ;
* in addition, let's call the *node to attach* and all of its descendants the
  *subtree to attach* ;
* into :option:`DBPATH` file, search for the node which
  :option:`FULLNAME` is passed to :option:`-p` or :option:`--parent` option ; if
  unspecified, use :option:`DBPATH`'s top-level node instead ; let's call this
  node the *parent node* ;
* if *parent node* has a child which `short name`_ matches the *node to
  attach*'s `short name`_, delete it ;
* register the *subtree to attach* as child of the *parent node* ;
* save result into :option:`DBPATH` file.

For example, running the command
``cute-report union testdb.xml conf0.xml conf0_suite`` would lead to the
following situation::

   «conf0.xml»           «testdb.xml»
    root_suite            root_suite
        |                     |
     suite_0            -- suite_1 --
        |               |           |
     test_00         test_10     test_11

                \ /
                 |  [join]
                 v

            «testdb.xml»
         --- root_suite ---
         |                |
    conf0_suite     -- suite_1 --
         |          |           |
      test_00    test_10     test_11

.. _join:

When join_ is given as first argument, :program:`cute-report` performs a
*partial* union of :option:`PATH` file content into :option:`DBPATH` file.
Operation is implemented as following:

* search for the top-level node of |hierarchy| found into the :option:`PATH`
  file ; let's call this node the *node to attach* ;
* into :option:`DBPATH` file, search for the node which
  :option:`FULLNAME` is passed to :option:`-p` or :option:`--parent` option ; if
  unspecified, use :option:`DBPATH`'s top-level node instead ; let's call this
  node the *parent node* ;
* if *parent node* has a child which `short name`_ matches the *node to
  attach*'s `short name`_, delete it ;
* register the *node to attach* as child of the *parent node* excluding all of
  its descendants ;
* save result into :option:`DBPATH` file.

For example, running the command
``cute-report join testdb.xml conf0.xml conf0_suite`` would lead to the
following situation::

   «conf0.xml»           «testdb.xml»
    root_suite            root_suite
        |                     |
     suite_0            -- suite_1 --
        |               |           |
     test_00         test_10     test_11

                \ /
                 |  [join]
                 v

            «testdb.xml»
         --- root_suite ---
         |                |
    conf0_suite     -- suite_1 --
                    |           |
                 test_10     test_11

.. _del:

When del_ is given as first argument, :program:`cute-report` deletes the
subtree which root node is specified by :option:`FULLNAME` from the
:option:`DBPATH` file.
For example, running the command
``cute-report del testdb.xml root_suite::suite_0`` would lead to the following
situation::

         «testdb.xml»
      --- root_suite ---
      |                |
   suite_0       -- suite_1 --
      |          |           |
   test_00    test_10     test_11

              |
              |  [del]
              v

         «testdb.xml»
          root_suite
              |
        -- suite_1 --
        |           |
     test_10     test_11

.. _list:

The list_ subcommands shows a list of supported result information fields that
may be used as :option:`FIELD` identifiers given to :option:`-f` or
:option:`--fields` options. In addition, these identifiers may also be used
within :option:`SELECTOR` Python_ expression in combination with the sumup_
subcommand.

Field informations are presented according to a tabular view composed of 3
columns :

* the first column, named ``IDENTIFIER``, shows field identifiers ;
* the second column, named ``LABEL``, shows column titles used by the sumup_
  subcommand as well as field labels used by the result_ and info_ subcommands ;
* the third and last column shows a short field description.

Arguments
=========

.. option:: DBPATH

   Pathname to JUnit_ XML formatted file where to store the result of union_,
   join_ or del_ operations. See section Files_ for more informations.

.. option:: FIELD

   A field identifier as shown by the list_ subcommand that may given to
   :option:`-f` or :option:`--fields` options. It is used to select which kind
   of information to display when using the sumup_ reporting subcommand.

.. option:: FULLNAME

   `Full name`_ identifying a single testcase or testsuite for use with union_,
   join_ and del_ subcommands.
   See section `Test naming`_ for more informations.

.. option:: NAME

   `Short name`_ or `full name`_ identifying a single testcase or testsuite for
   use with result_, info_, union_ and join_ subcommands.
   See section `Test naming`_ for more informations.

.. option:: PACKAGE

   When performing a union_ or join_ operation, name of package to assign to the
   top-level node of |hierarchy| found into the :option:`PATH` file that is
   attached / included into :option:`DBPATH` store.

.. option:: PATH

   A pathname to a file where JUnit_ XML content to process is to stored.
   See section Files_ for more informations.

.. option:: REVISION

   When performing a union_ or join_ operation, package revision string to
   assign to the top-level node of |hierarchy| found into the :option:`PATH` file
   and that is attached / included into :option:`DBPATH` store.

.. option:: SELECTOR

   A Python_ expression to select a subset of results to show when used in
   combination with the sumup_ subcommand. See section `Selecting test`_ for
   more informations.

Options
=======

.. option:: -c <color>, --color <color>

   Setup colorized output according to *color* argument which may take one of
   the following value :

   * ``on`` to enforce output colorization ;
   * ``off`` to disable output colorization ;
   * ``auto`` to enable colorization when output terminal supports it only.

.. option:: -f <field_list>, --fields <field_list>

   Setup result information fields to display when running the sumup_ reporting
   subcommand. *field_list* must be given as a comma separated list of
   :option:`FIELD` identifiers as shown by the list_ subcommand.

.. option:: -h, --help

   Output a help message.

.. option:: -a <PACKAGE>, --package <PACKAGE>

   While performing a union_ or join_ operation, setup :option:`PACKAGE` name to
   assign to the top-level node of |hierarchy| found into the :option:`PATH` file
   and that is attached / included into :option:`DBPATH` store.

.. option:: -p <FULLNAME>, --parent <FULLNAME>

   Specify the :option:`DBPATH` file's node under which to perform a union_ or
   join_ operation. Node is identified thanks to the mandatory
   :option:`FULLNAME` argument as described into Arguments_ section.

.. option:: -r <REVISION>, --revision <REVISION>

   While performing a union_ or join_ operation, setup package
   :option:`REVISION` string to assign to the top-level node of |hierarchy|
   found into the :option:`PATH` file and that is attached / included into
   :option:`DBPATH` store.

.. _sect-cute-report-test_naming:

Test naming
===========

:option:`PATH` and :option:`DBPATH` arguments must point to a file containing
a JUnit_ XML test |hierarchy| such as::

      --- root_suite ---
      |                |
   suite_0       -- suite_1 --
      |          |           |
   test_00    test_10     test_11

When given to :ref:`subcommands <synopsis>`, :option:`NAME` and
:option:`FULLNAME` allows to select a particular node within hierarchies
according to 2 different naming schemes: *short name* and *full name*.

As opposed to short names, a *full name* can uniquely identify a test
|hierarchy| node (either testcase or testsuite). This node is selected by
following a path along a hierarchy composed of a double colon (``::``) separated
sequence of *short names* for each of its ancestors.

As an example, the following table shows short and full names that would be
assigned to some of the above sample test |hierarchy| nodes:

.. table::
   :align: left

   ==============  ============================
   **Short name**  **Full name**
   ==============  ============================
   root_suite      root_suite
   suite_0         root_suite::suite_0
   test_10         root_suite::suite_1::test_10
   ==============  ============================

Selecting test
==============

Basically, the sumup_ subcommand reports informations for each existing
|hierarchy| node that the :option:`SELECTOR` expression matches.
To select a particular node. :option:`SELECTOR` expression must return ``True``.
It should return ``False`` otherwise. The following node fields are exposed for
selection purposes :

.. table::
   :align: left

   ==============  ==================  ====================================================================================
   **Field**       **Type**            **Description**
   ==============  ==================  ====================================================================================
   *build_id*      str                 node build identifier
   *children*      Iterator[CuteBase]  Iterator over node's children
   *depth*         int                 node depth starting from ``0`` within the |hierarchy|
   *excp_count*    int                 absolute number of crashed test cases for a test suite
   *excp_ratio*    int                 ratio of crashed test cases for a test suite
   *exec_count*    int                 absolute number of executed test cases for a test suite
   *exec_ratio*    int                 ratio of executed test cases for a test suite
   *fail_count*    int                 absolute number of failed test cases for a test suite
   *fail_ratio*    int                 ratio of failed test cases for a test suite
   *full_name*     str                 node `full name`_
   *has_build_id*  bool                ``True`` if node is assigned a build identifier, ``False`` otherwise
   *has_children*  bool                ``True`` if node has child nodes, ``False`` otherwise
   *has_host*      bool                ``True`` if node is assigned a host, ``False`` otherwise
   *has_line*      bool                ``True`` if node is assigned a source file line number, ``False`` otherwise
   *has_origin*    bool                ``True`` if node is assigned a result source file pathname, ``False`` otherwise
   *has_parent*    bool                ``True`` if node has a parent node, ``False`` otherwise
   *has_path*      bool                ``True`` if node is assigned a source file pathname, ``False`` otherwise
   *has_pkg*       bool                ``True`` if node is assigned a package name, ``False`` otherwise
   *has_start*     bool                ``True`` if node is assigned a start date, ``False`` otherwise
   *has_time*      bool                ``True`` if node is assigned an elapsed running time, ``False`` otherwise
   *has_vers*      bool                ``True`` if node is assigned a package version, ``False`` otherwise
   *host*          str                 name of host that ran node
   *line*          int                 line number within source file where test node is defined
   *name*          str                 node `short name`_
   *off_count*     int                 absolute number of disabled test cases for a test suite
   *off_ratio*     int                 ratio of disabled test cases for a test suite
   *origin*        str                 pathname to file where test suite result is initially stored
   *parent*        CuteSuite           node's parent object
   *pass_count*    int                 absolute number of passed test cases for a test suite
   *pass_ratio*    int                 ratio of passed test cases for a test suite
   *path*          str                 pathname to source file of node
   *pkg*           str                 package name of test case / suite
   *skip_count*    int                 absolute number of skipped test cases for a test suite
   *skip_ratio*    int                 ratio of skipped test cases for a test suite
   *start*         str                 start date expressed according to local time settings
   *stat*          str                 node result status, i.e., either ``pass``, ``skip``, ``fail``, ``excp`` or ``off``
   *time*          str                 elapsed running time expressed as <*hours*>:<*minutes*>:<*seconds*>.<*microseconds*>
   *total*         int                 absolute number of test cases
   *type*          str                 either ``store``, ``suite`` or ``case``
   *vers*          str                 node package version
   ==============  ==================  ====================================================================================

As an example, the following command would show a result summary for |hierarchy|
test suites which `short name`_ starts with ``myproject_``:

.. code-block:: console

   cute-report sumup myproject.xml "name.startswith('myproject_') and type == 'suite'"

Exit status
===========

When given the verify_ subcommand, :program:`cute-report` terminates with a
``0`` (success) exit code when all tests found into the :option:`PATH` file have
passed. It terminates with a ``1`` (failure) exit code otherwise.

For all other subcommands :program:`cute-report` terminates with a non zero
failure exit code when and input files parsing or output files saving operation
fails.

Files
=====

<:option:`DBPATH`> and <:option:`PATH`> given as argument to
:program:`cute-report` should hold valid JUnit_ XML content as specified by
the :file:`cute-junit.xsd` `XML schema`_ shipped under CUTe_'s |DATADIR|/cute
directory.

:file:`cute-junit.xsd` `XML schema`_ is deeply inspired from 
Jenkins_ JUnit_ schema augmented with a few optional extensions. Refer to
section :ref:`JUnit XML <sect-user-junit>` of the CUTe_'s
:doc:`user manual <../user>` for more informations.

See also
========

.. only:: not man

   :doc:`cute-run(1) <cute-run>`

.. only:: man

   **cute-run(1)**

.. include:: /man/see_also.rst
