#include "cute/cute.h"
#include "cute/check.h"

static int setup = 0;

static void
simple_fixture_fail_setup()
{
	setup++;
}

static void
simple_fixture_fail_teardown()
{
	setup++;
}

CUTE_STATIC_TEST(simple_fixture_fail_pass_test,
                 simple_fixture_fail_setup,
                 simple_fixture_fail_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_assert(setup == 1);
}

CUTE_STATIC_TEST(simple_fixture_fail_fail_test,
                 simple_fixture_fail_setup,
                 simple_fixture_fail_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_check_assert(setup == 3);
	cute_check_assert(0 == 1);
}

static CUTE_SUITE_DEFINE_TESTS(simple_fixture_fail_tests) = {
	CUTE_REF(simple_fixture_fail_pass_test),
	CUTE_REF(simple_fixture_fail_fail_test)
};

static CUTE_SUITE_DEFINE(simple_fixture_fail_suite,
                         simple_fixture_fail_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_fixture_fail_suite, "CUTe", NULL)
