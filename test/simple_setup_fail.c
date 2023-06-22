#include "cute/cute.h"

static void
simple_setup_fail()
{
	cute_fail();
}

CUTE_STATIC_TEST(simple_setup_fail_test,
                 simple_setup_fail,
                 CUTE_NULL_TEARDOWN,
                 CUTE_DFLT_TMOUT)
{
}

static CUTE_SUITE_DEFINE_TESTS(simple_setup_fail_tests) = {
	CUTE_REF(simple_setup_fail_test),
};

static CUTE_SUITE_DEFINE(simple_setup_fail_suite,
                         simple_setup_fail_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_setup_fail_suite, "CUTe", NULL)
