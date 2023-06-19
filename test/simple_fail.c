#include "cute/cute.h"

CUTE_TEST(simple_fail)
{
	cute_ensure(0 == 1);
}

static CUTE_SUITE_DEFINE_TESTS(simple_fail_tests) = {
	CUTE_REF(simple_fail),
};

static CUTE_SUITE_DEFINE(simple_fail_suite,
                         simple_fail_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_fail_suite, "CUTe")
