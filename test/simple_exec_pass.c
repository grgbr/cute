#include "cute/check.h"

CUTE_TEST(simple_exec_pass_test)
{
	cute_check_assert(0 == 0);
}

static CUTE_SUITE_DEFINE_TESTS(simple_exec_pass_tests) = {
	CUTE_REF(simple_exec_pass_test),
};

static CUTE_SUITE_DEFINE(simple_exec_pass_suite,
                         simple_exec_pass_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_exec_pass_suite, "CUTe", NULL)
