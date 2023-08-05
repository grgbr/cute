.. SPDX-License-Identifier: GFDL-1.3-only

   This file is part of CUTe.
   Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>

.. _cute:                   https://github.com/grgbr/cute

.. _fixture:                https://en.wikipedia.org/wiki/Test_fixture
.. _test fixtures:          fixture_

.. _fork(2):                https://man7.org/linux/man-pages/man2/fork.2.html

.. _glibc:                  https://www.gnu.org/software/libc

.. _junit:                  https://en.wikipedia.org/wiki/JUnit

.. _longjmp(3p):            https://man7.org/linux/man-pages/man3/longjmp.3p.html

.. _mock:                   https://en.wikipedia.org/wiki/Mock_object
.. _mocking:                mock_

.. _regtest:                https://en.wikipedia.org/wiki/Regression_testing
.. _regression testing:     regtest_

.. _tap:                    https://testanything.org/
.. _test anything protocol: tap_

.. _utest:                  https://en.wikipedia.org/wiki/Unit_testing
.. _unit tests:             utest_
.. _unit testing:           utest_

About
#####

CUTe_ is a lightweight `unit testing`_ and mocking_
framework for C.

It is meant to ease the process of writing `unit tests`_ and make them
suitable for test automation and `regression testing`_ activities.

.. rubric:: Features

* flexible and modular test suite hierarchy,
* `test fixtures`_,
* strict function mocking_,
* extensive reporting,
* easy debugging of test failures,
* JUnit_ XML reporting,
* `Test Anything Protocol`_ reporting,
* C library dependency only.

.. rubric:: Limitations

* GNU / Linux / Glibc_ userspace C `unit testing`_ only
* `longjmp(3p)`_ based test failure recovery only
* no `fork(2)`_ based test isolation
* no parameterizable test support
* custom check assertion not yet supported

.. rubric:: Licensing

CUTe_ is distributed under the `GNU Lesser General Public License
<https://www.gnu.org/licenses/lgpl-3.0.html>`_.

Getting Help
############

`Integration guide <sphinx/install.rst>`_ shows how to **build and deploy**
CUTe_.

`User guide <sphinx/user.rst>`_ documents how to **write and run tests**.

In addition, once familiar with CUTe_, you may refer to the
`API reference guide <sphinx/api.rst>`_ *while in the process of writing
tests*.
