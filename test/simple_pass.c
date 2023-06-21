#include "cute/cute.h"

CUTE_TEST(simple_pass)
{
	cute_ensure(0 == 0);
}

static CUTE_SUITE_DEFINE_TESTS(simple_pass_tests) = {
	CUTE_REF(simple_pass),
};

static CUTE_SUITE_DEFINE(simple_pass_suite,
                         simple_pass_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_pass_suite, "CUTe", NULL)
