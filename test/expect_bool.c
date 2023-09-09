/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "cute/cute.h"
#include "cute/check.h"
#include "cute/expect.h"

static void
expect_bool_value_callee(bool value)
{
	cute_mock_bool_parm(value);
}

static void
expect_bool_value_caller(bool value)
{
	expect_bool_value_callee(value);
}

CUTE_TEST(expect_bool_caller_fail_test)
{
	cute_expect_bool_parm(nonexistent_callee, value, is, true);
	expect_bool_value_caller(true);
}

CUTE_TEST(expect_bool_parm_fail_test)
{
	cute_expect_bool_parm(expect_bool_value_callee,
	                      nonexistent_parm,
	                      is,
	                      true);
	expect_bool_value_caller(true);
}

CUTE_TEST(expect_bool_is_pass_test)
{
	bool ref = false;

	cute_expect_bool_parm(expect_bool_value_callee, value, is, true);
	cute_expect_bool_parm(expect_bool_value_callee, value, is_not, ref);

	expect_bool_value_caller(true);
	expect_bool_value_caller(true);
}

CUTE_TEST(expect_bool_literal_is_fail_test)
{
	cute_expect_bool_parm(expect_bool_value_callee, value, is, true);
	expect_bool_value_caller(false);
}

CUTE_TEST(expect_bool_var_is_fail_test)
{
	bool ref = false;

	cute_expect_bool_parm(expect_bool_value_callee, value, is, ref);
	expect_bool_value_caller(true);
}

CUTE_TEST(expect_bool_is_not_pass_test)
{
	bool ref = true;

	cute_expect_bool_parm(expect_bool_value_callee, value, is_not, false);
	cute_expect_bool_parm(expect_bool_value_callee, value, is_not, ref);

	expect_bool_value_caller(true);
	expect_bool_value_caller(false);
}

CUTE_TEST(expect_bool_literal_is_not_fail_test)
{
	cute_expect_bool_parm(expect_bool_value_callee, value, is_not, false);
	expect_bool_value_caller(false);
}

CUTE_TEST(expect_bool_var_is_not_fail_test)
{
	bool ref = true;

	cute_expect_bool_parm(expect_bool_value_callee, value, is_not, ref);
	expect_bool_value_caller(true);
}

static bool
expect_bool_retval_callee(void)
{
	return cute_mock_bool_retval();
}

static void
expect_bool_retval_caller(bool * result, size_t size)
{
	unsigned int c;

	for (c = 0; c < (size); c++)
		result[c] = expect_bool_retval_callee();
}

CUTE_TEST(expect_bool_retval_pass_test)
{
	bool res[3] = { 0, };

	cute_expect_bool_retval(expect_bool_retval_callee, true);
	cute_expect_bool_retval(expect_bool_retval_callee, false);
	cute_expect_bool_retval(expect_bool_retval_callee, true);
	expect_bool_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_bool(res[0], is, true);
	cute_check_bool(res[1], is, false);
	cute_check_bool(res[2], is, true);
}

CUTE_TEST(expect_bool_retval_caller_fail_test)
{
	bool res[3] = { 0, };

	cute_expect_bool_retval(nonexistent_callee, false);
	cute_expect_bool_retval(nonexistent_callee, false);
	cute_expect_bool_retval(nonexistent_callee, false);
	expect_bool_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_bool(res[0], is, false);
	cute_check_bool(res[1], is, false);
	cute_check_bool(res[2], is, false);
}

CUTE_TEST(expect_xcess_bool_retval_fail_test)
{
	bool res[2] = { 0, };

	cute_expect_bool_retval(expect_bool_retval_callee, false);
	cute_expect_bool_retval(expect_bool_retval_callee, true);
	cute_expect_bool_retval(expect_bool_retval_callee, false);
	expect_bool_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_bool(res[0], is, false);
	cute_check_bool(res[1], is, true);
}

CUTE_TEST(expect_bool_inval_retval_fail_test)
{
	bool res[2] = { 0, };

	cute_expect_bool_parm(expect_bool_retval_callee,
	                      nonexistent_parm,
	                      is,
	                      false);
	expect_bool_retval_caller(res, sizeof(res));
}

CUTE_GROUP(expect_bool_tests) = {
	CUTE_REF(expect_bool_caller_fail_test),
	CUTE_REF(expect_bool_parm_fail_test),

	CUTE_REF(expect_bool_is_pass_test),
	CUTE_REF(expect_bool_literal_is_fail_test),
	CUTE_REF(expect_bool_var_is_fail_test),

	CUTE_REF(expect_bool_is_not_pass_test),
	CUTE_REF(expect_bool_literal_is_not_fail_test),
	CUTE_REF(expect_bool_var_is_not_fail_test),

	CUTE_REF(expect_bool_retval_pass_test),
	CUTE_REF(expect_bool_retval_caller_fail_test),
	CUTE_REF(expect_xcess_bool_retval_fail_test),
	CUTE_REF(expect_bool_inval_retval_fail_test)
};

CUTE_SUITE(expect_bool_suite, expect_bool_tests);

CUTE_MAIN(expect_bool_suite, "CUTe", NULL)
