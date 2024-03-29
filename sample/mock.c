/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include <cute/cute.h>
#include <cute/expect.h>
#include <stdlib.h>

typedef void (callee_fn)(void);

static void
mocked_callee(void)
{
	cute_mock_call();
}

static void
caller(callee_fn * callee)
{
	callee();
}

CUTE_TEST(mock_call_pass_test)
{
	cute_expect_call(mocked_callee);
	caller(mocked_callee);
}

CUTE_TEST(mock_call_inval_fail_test)
{
	cute_expect_call(nonexistent_callee);
	caller(mocked_callee);
}

CUTE_TEST(mock_call_xcess_fail_test)
{
	cute_expect_call(mocked_callee);
	cute_expect_call(mocked_callee);
	caller(mocked_callee);
}

typedef void (parm_callee_fn)(int);

static void
mocked_parm_callee(int parm)
{
	cute_mock_sint_parm(parm);
}

static void
parm_caller(parm_callee_fn * callee, int parm)
{
	callee(parm);
}

CUTE_TEST(mock_parm_pass_test)
{
	cute_expect_sint_parm(mocked_parm_callee, parm, equal, 2);
	parm_caller(mocked_parm_callee, 2);
}

CUTE_TEST(mock_parm_value_fail_test)
{
	cute_expect_sint_parm(mocked_parm_callee, parm, equal, 3);
	parm_caller(mocked_parm_callee, 2);
}

CUTE_TEST(mock_parm_name_fail_test)
{
	cute_expect_sint_parm(mocked_parm_callee, nonexistent, equal, 3);
	parm_caller(mocked_parm_callee, 2);
}

CUTE_TEST(mock_parm_xcess_fail_test)
{
	cute_expect_sint_parm(mocked_parm_callee, parm, equal, 3);
	cute_expect_sint_parm(mocked_parm_callee, parm, equal, 3);
	parm_caller(mocked_parm_callee, 3);
}

CUTE_TEST(mock_inval_parm_fail_test)
{
	cute_expect_sint_parm(mocked_parm_callee, parm, equal, 3);
	caller(mocked_callee);
}

CUTE_GROUP(mock_suite_tests) = {
	CUTE_REF(mock_call_pass_test),
	CUTE_REF(mock_call_inval_fail_test),
	CUTE_REF(mock_call_xcess_fail_test),
	CUTE_REF(mock_parm_pass_test),
	CUTE_REF(mock_parm_value_fail_test),
	CUTE_REF(mock_parm_name_fail_test),
	CUTE_REF(mock_parm_xcess_fail_test),
	CUTE_REF(mock_inval_parm_fail_test)
};

static CUTE_SUITE_DEFN(mock_suite,
                       mock_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_MAIN(mock_suite, "CUTe mocking sample", CUTE_VERSION_STRING)
