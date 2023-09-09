/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(check_bool_is_pass_test)
{
	bool chk = true;
	bool ref = false;

	cute_check_bool(false, is, false);
	cute_check_bool(true, is, true);
	cute_check_bool(chk, is, true);
	cute_check_bool(false, is, ref);
	chk = false;
	cute_check_bool(chk, is, ref);
}

CUTE_TEST(check_bool_literal_is_fail_test)
{
	cute_check_bool(false, is, true);
}

CUTE_TEST(check_bool_var_is_fail_test)
{
	bool chk = true;

	cute_check_bool(chk, is, false);
}

CUTE_TEST(check_bool_is_ref_fail_test)
{
	bool ref = true;

	cute_check_bool(false, is, ref);
}

CUTE_TEST(check_bool_var_is_ref_fail_test)
{
	bool chk = true;
	bool ref = false;

	cute_check_bool(chk, is, ref);
}

CUTE_TEST(check_bool_is_not_pass_test)
{
	bool chk = true;
	bool ref = false;

	cute_check_bool(false, is_not, true);
	cute_check_bool(true, is_not, false);
	cute_check_bool(chk, is_not, false);
	cute_check_bool(true, is_not, ref);
	cute_check_bool(chk, is_not, ref);
}

CUTE_TEST(check_bool_literal_is_not_fail_test)
{
	cute_check_bool(true, is_not, true);
}

CUTE_TEST(check_bool_var_is_not_fail_test)
{
	bool chk = false;

	cute_check_bool(chk, is_not, false);
}

CUTE_TEST(check_bool_is_not_ref_fail_test)
{
	bool ref = true;

	cute_check_bool(true, is_not, ref);
}

CUTE_TEST(check_bool_var_is_not_ref_fail_test)
{
	bool chk = false;
	bool ref = false;

	cute_check_bool(chk, is_not, ref);
}

CUTE_GROUP(check_bool_tests) = {
	CUTE_REF(check_bool_is_pass_test),
	CUTE_REF(check_bool_literal_is_fail_test),
	CUTE_REF(check_bool_var_is_fail_test),
	CUTE_REF(check_bool_is_ref_fail_test),
	CUTE_REF(check_bool_var_is_ref_fail_test),

	CUTE_REF(check_bool_is_not_pass_test),
	CUTE_REF(check_bool_literal_is_not_fail_test),
	CUTE_REF(check_bool_var_is_not_fail_test),
	CUTE_REF(check_bool_is_not_ref_fail_test),
	CUTE_REF(check_bool_var_is_not_ref_fail_test),
};

CUTE_SUITE(check_bool_suite, check_bool_tests);

CUTE_MAIN(check_bool_suite, "CUTe", NULL)
