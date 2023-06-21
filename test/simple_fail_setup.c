#include "cute/cute.h"

static void
simple_fail_setup()
{
	cute_fail();
}

CUTE_STATIC_TEST(simple_fail_setup_test,
                 simple_fail_setup,
                 CUTE_NULL_TEARDOWN,
                 CUTE_DFLT_TMOUT)
{
}

static CUTE_SUITE_DEFINE_TESTS(simple_fail_setup_tests) = {
	CUTE_REF(simple_fail_setup_test),
};

static CUTE_SUITE_DEFINE(simple_fail_setup_suite,
                         simple_fail_setup_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_fail_setup_suite, "CUTe", NULL)
