.. SPDX-License-Identifier: GPL-3.0-only This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: _cdefs.rst

.. _elfutils:               https://sourceware.org/elfutils
.. _breathe:                https://github.com/breathe-doc/breathe
.. _graphviz:               https://www.graphviz.org/
.. _plantuml:               https://plantuml.com/
.. _sphinxcontrib-plantuml: https://pypi.org/project/sphinxcontrib-plantuml/
.. _gnu_make:               https://www.gnu.org/software/make/
.. |eBuild|                 replace:: `eBuild <ebuild_>`_
.. |eBuild User Guide|      replace:: :external+ebuild:doc:`eBuild User Guide <user>`
.. |eBuild Prerequisites|   replace:: :external+ebuild:ref:`eBuild Prerequisites <sect-user-prerequisites>`
.. |Configure|              replace:: :external+ebuild:ref:`sect-user-configure`
.. |Build|                  replace:: :external+ebuild:ref:`sect-user-build`
.. |Install|                replace:: :external+ebuild:ref:`sect-user-install`
.. |Staged install|         replace:: :external+ebuild:ref:`sect-user-staged-install`
.. |BUILDDIR|               replace:: :external+ebuild:ref:`var-builddir`
.. |PREFIX|                 replace:: :external+ebuild:ref:`var-prefix`
.. |BINDIR|                 replace:: :external+ebuild:ref:`var-bindir`
.. |CROSS_COMPILE|          replace:: :external+ebuild:ref:`var-cross_compile`
.. |DESTDIR|                replace:: :external+ebuild:ref:`var-destdir`
.. |GNU Make|               replace:: `GNU Make <gnu_make_>`_

Overview
========

This guide mainly focuses upon the construction process required to install
CUTe_.

CUTe_'s build logic is based upon |eBuild|. In addition to the build process
description explained below, you may refer to the |eBuild User Guide|
for further detailed informations.

Prerequisites
=============

In addition to the standard |eBuild Prerequisites|, elfutils_ is required to
to build CUTe_.

Optionally, you will need :command:`awk`, :command:`cmp` and a :command:`sh`
Bourne shell at runtime when unit testsuite_ is enabled (see
:c:macro:`CONFIG_CUTE_UTEST`).

Optionally, you will need multiple packages installed to build the
documentation_. In addition to packages listed into |eBuild Prerequisites|,
CUTe_'s documentation_ generation process requires :

* breathe_,
* graphviz_,
* plantuml_,
* and sphinxcontrib-plantuml_.

Getting help
============

From CUTe_ source tree root, enter:

.. code-block:: console

   $ make help

Also note that a more detailed help message is available:

.. code-block:: console

   $ make help-full

Refer to :external+ebuild:ref:`eBuild help target <target-help>` and
:external+ebuild:ref:`eBuild help-full target <target-help-full>` for further
informations.

The :external+ebuild:ref:`eBuild Troubleshooting <sect-user-troubleshooting>`
section also contains valuable informations.

Build Workflow
==============

As mentioned earlier, CUTe_'s build logic is based on |eBuild|, a |GNU make|
based build system. To build and install CUTe_, the typical workflow is:

#. Prepare and collect workflow requirements,
#. |Configure| the construction logic,
#. |Build| programs, libraries, etc.,
#. |Install| components, copying files previously built to
   system-wide directories

Alternatively, you may replace the last step mentioned above with a |Staged
Install|. You will find below a **quick starting guide** showing how to build
CUTe_.

You are also provided with the ability to :

* generate documentation_,
* build, install, and / or run CUTe_'s testsuite_.

Preparation phase
-----------------

The overall :external+ebuild:ref:`eBuild Workflow <sect-user-workflow>` is
customizable thanks to multiple :command:`make` variable settings. You should
adjust values according to your specific needs.

Most of the time, setting |BUILDDIR|, |PREFIX|, |CROSS_COMPILE| is enough.
You should also set the :envvar:`PATH` environment variable according to the
set of tools required by the build process.

Optionally, you may set ``EXTRA_CFLAGS`` and ``EXTRA_LDFLAGS`` variables to
give the compiler and linker additional flags respectively.

Finally, you may set the ``PYTHON_SHEBAND`` variable so that installed
tool(s) use the specified Python_ binary (defaults to ``/usr/bin/env python3``).

Refer to :external+ebuild:ref:`eBuild Tools <sect-user-tools>` and
:external+ebuild:ref:`eBuild Variables <sect-user-variables>` for further
informations.

.. _workflow-configure-phase:

Configure phase
---------------

To begin with, |Configure| the build process interactively by running the
:external+ebuild:ref:`eBuild menuconfig target <target-menuconfig>`:

.. code-block:: console

   $ make menuconfig BUILDDIR=$HOME/build/cute

Build phase
-----------

Now, proceed to the |Build| phase and compile / link programs, libraries, etc.
by running the :external+ebuild:ref:`eBuild build target <target-build>`:

.. code-block:: console

   $ make build BUILDDIR=$HOME/build/cute PREFIX=/usr

Install phase
-------------

Finally, |Install| programs, libraries, etc.: by running the
:external+ebuild:ref:`eBuild install target <target-install>`:

.. code-block:: console

   $ make install BUILDDIR=$HOME/build/cute PREFIX=/usr

Alternative staged install phase
--------------------------------

Alternatively, perform a |Staged install| by specifying the |DESTDIR| variable
instead:

.. code-block:: console

   $ make install BUILDDIR=$HOME/build/cute PREFIX=/usr DESTDIR=$HOME/staging

Documentation
-------------

You may generate CUTe_ documentation by running the
:external+ebuild:ref:`eBuild doc target <target-doc>` like so:

.. code-block:: console

   $ make doc BUILDDIR=$HOME/build/cute PREFIX=/usr

You may further install generated documentation by running the
:external+ebuild:ref:`eBuild install-doc target <target-install-doc>` target:

.. code-block:: console

   $ make install-doc BUILDDIR=$HOME/build/cute PREFIX=/usr DESTDIR=$HOME/staging

.. _testsuite:

Testing
-------

When the :c:macro:`CONFIG_CUTE_UTEST` build configuration setting is enabled,
you may build CUTe_ testsuite by running the
:external+ebuild:ref:`eBuild build-check target <target-build-check>` target
like so:

.. code-block:: console

   $ make build-check BUILDDIR=$HOME/build/cute PREFIX=/usr

You may further install generated testsuite by running the
:external+ebuild:ref:`eBuild install-check target <target-install-check>`
target:

.. code-block:: console

   $ make install-check BUILDDIR=$HOME/build/cute PREFIX=/usr DESTDIR=$HOME/staging

Use the :command:`cute-utest.sh` program installed under the |BINDIR| directory
to run the testsuite.

When *not cross-compiling*, running the ``check`` target builds and runs
the testsuite all at once with no required installation:

.. code-block:: console

   $ make check BUILDDIR=$HOME/build/cute CHECK_VERBOSE=y

The ``CHECK_VERBOSE`` :command:`make` variable given above may be specified as
one of ``y`` or ``1`` values to enable testsuite verbose output.

Further informations
--------------------

Finally, you may find lots of usefull informations into the
:external+ebuild:ref:`Reference <sect-user-reference>` section of the |eBuild
User Guide|.
