/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(simple_exec_pass_test)
{
	cute_check_assert(0 == 0);
}

CUTE_GROUP(simple_exec_pass_tests) = {
	CUTE_REF(simple_exec_pass_test),
};

CUTE_SUITE(simple_exec_pass_suite, simple_exec_pass_tests);

CUTE_MAIN(simple_exec_pass_suite, "CUTe", NULL)
