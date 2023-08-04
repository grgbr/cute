/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cute/cute.h"

static void
simple_setup_fail()
{
	cute_fail("explicit setup failure");
}

CUTE_TEST_STATIC(simple_setup_fail_test,
                 simple_setup_fail,
                 CUTE_NULL_TEARDOWN,
                 CUTE_DFLT_TMOUT)
{
}

CUTE_GROUP(simple_setup_fail_tests) = {
	CUTE_REF(simple_setup_fail_test),
};

CUTE_SUITE(simple_setup_fail_suite, simple_setup_fail_tests);

CUTE_MAIN(simple_setup_fail_suite, "CUTe", NULL)
