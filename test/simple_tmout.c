#include "cute/cute.h"
#include <unistd.h>

CUTE_TEST(simple_tmout_dflt_pass_test)
{
	usleep(CUTE_DFLT_TMOUT * 1000000U / 2);
}

CUTE_TEST(simple_tmout_dflt_fail_test)
{
	usleep((CUTE_DFLT_TMOUT + 1) * 1000000U);
}

CUTE_STATIC_TEST(simple_tmout_explicit_pass_test,
                 CUTE_NULL_SETUP,
                 CUTE_NULL_TEARDOWN,
                 1U)
{
	usleep(1000000U / 2);
}

CUTE_STATIC_TEST(simple_tmout_explicit_fail_test,
                 CUTE_NULL_SETUP,
                 CUTE_NULL_TEARDOWN,
                 1U)
{
	usleep(2000000U);
}

CUTE_STATIC_TEST(simple_tmout_inhr_pass_test,
                 CUTE_NULL_SETUP,
                 CUTE_NULL_TEARDOWN,
                 CUTE_INHR_TMOUT)
{
	usleep(1500000U);
}

CUTE_STATIC_TEST(simple_tmout_inhr_fail_test,
                 CUTE_NULL_SETUP,
                 CUTE_NULL_TEARDOWN,
                 CUTE_INHR_TMOUT)
{
	usleep(2500000U);
}

static CUTE_SUITE_DEFINE_TESTS(simple_tmout_tests) = {
	CUTE_REF(simple_tmout_dflt_pass_test),
	CUTE_REF(simple_tmout_dflt_fail_test),
	CUTE_REF(simple_tmout_explicit_pass_test),
	CUTE_REF(simple_tmout_explicit_fail_test),
	CUTE_REF(simple_tmout_inhr_pass_test),
	CUTE_REF(simple_tmout_inhr_fail_test),
};

static CUTE_SUITE_DEFINE(simple_tmout_suite,
                         simple_tmout_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         2U);

CUTE_MAIN(simple_tmout_suite, "CUTe", NULL)
