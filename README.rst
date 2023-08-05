.. SPDX-License-Identifier: GFDL-1.3-only

   This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. include:: sphinx/_cdefs.rst

About
#####

|CUTe| is a lightweight `unit testing <utest_>`_ and `mocking <mock_>`_
framework for C.

It is meant to ease the process of writing `unit tests <utest_>`_ and make them
suitable for test automation and `regression testing <regtest_>`_ activities.

.. rubric:: Features

* flexible and modular test suite hierarchy,
* `test fixtures <fixture_>`_,
* strict function `mocking <mock_>`_,
* extensive reporting,
* easy debugging of test failures,
* `JUnit <junit_>`_ XML reporting,
* `Test Anything Protocol <tap_>`_ reporting,
* C library dependency only.

.. rubric:: Limitations

* GNU / Linux / `Glibc <glibc_>`_ userspace C `unit testing <utest_>`_ only
* |longjmp(3)| based test failure recovery only
* no |fork(2)| based test isolation
* no parameterizable test support
* custom check assertion not yet supported
  
.. rubric:: Licensing

|CUTe| is distributed under the :ref:`GNU Lesser General Public License
<https://www.gnu.org/licenses/lgpl-3.0.html>`.

Getting Help
############

:doc:`Integration guide <sphinx/install.rst>` shows how to **build and deploy**
|CUTe|.

:doc:`User guide <sphinx/user.rst>` documents how to **write and run tests**.

In addition, once familiar with |CUTe|, you may refer to the
:doc:`API reference guide <sphinx/api.rst>` *while in the process of writing
tests*.
