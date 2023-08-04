/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cute/cute.h"

static void
simple_fail_setup()
{
	int * data = NULL;

	*data = 2;
}

CUTE_TEST_STATIC(simple_fail_setup_test,
                 simple_fail_setup,
                 CUTE_NULL_TEARDOWN,
                 CUTE_DFLT_TMOUT)
{
}

CUTE_GROUP(simple_fail_setup_tests) = {
	CUTE_REF(simple_fail_setup_test),
};

CUTE_SUITE(simple_fail_setup_suite, simple_fail_setup_tests);

CUTE_MAIN(simple_fail_setup_suite, "CUTe", NULL)
