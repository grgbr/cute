#include "cute/cute.h"

static int setup = 0;

static void
simple_setup()
{
	setup++;
}

static void
simple_teardown()
{
	setup++;
}

CUTE_STATIC_TEST(simple_fixture_pass_exec_test,
                 simple_setup,
                 simple_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_ensure(setup == 1);
}

CUTE_STATIC_TEST(simple_fixture_fail_exec_test,
                 simple_setup,
                 simple_teardown,
                 CUTE_DFLT_TMOUT)
{
	cute_ensure(setup == 3);
	cute_ensure(0 == 1);
}

static CUTE_SUITE_DEFINE_TESTS(simple_fixture_exec_tests) = {
	CUTE_REF(simple_fixture_pass_exec_test),
	CUTE_REF(simple_fixture_fail_exec_test)
};

static CUTE_SUITE_DEFINE(simple_fixture_exec_suite,
                         simple_fixture_exec_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_fixture_exec_suite, "CUTe")
