/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include <cute/cute.h>
#include <cute/check.h>
#include <stdlib.h>

static void
test_skipped(void)
{
	cute_skip("requested explicit skip");
}

static CUTE_TEST_DEFN(skipped,
                      test_skipped,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static void
test_failed(void)
{
	cute_fail("requested explicit fail");
}

static CUTE_TEST_DEFN(failed,
                      test_failed,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static void
test_failed_ensure(void)
{
	cute_check_assert(0 == 0);
	cute_check_assert(0 == 1);
	cute_check_assert(1 == 1);
}

static CUTE_TEST_DEFN(failed_ensure,
                      test_failed_ensure,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static void
test_passed(void)
{
	cute_check_assert(0 == 0);
}

static CUTE_TEST_DEFN(passed,
                      test_passed,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);


CUTE_GROUP(hello_suite_tests) = {
	CUTE_REF(skipped),
	CUTE_REF(failed),
	CUTE_REF(failed_ensure),
	CUTE_REF(passed)
};

static CUTE_SUITE_DEFN(hello_suite,
                       hello_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_MAIN(hello_suite, "CUTe hello sample", CUTE_VERSION_STRING)
