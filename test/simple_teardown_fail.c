#include "cute/cute.h"

static void
simple_teardown_fail()
{
	cute_fail("explicit teardown failure");
}

CUTE_STATIC_TEST(simple_teardown_fail_test,
                 CUTE_NULL_SETUP,
                 simple_teardown_fail,
                 CUTE_DFLT_TMOUT)
{
}

static CUTE_SUITE_DEFINE_TESTS(simple_teardown_fail_tests) = {
	CUTE_REF(simple_teardown_fail_test),
};

static CUTE_SUITE_DEFINE(simple_teardown_fail_suite,
                         simple_teardown_fail_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_teardown_fail_suite, "CUTe", NULL)
