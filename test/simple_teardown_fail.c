#include "cute/cute.h"

static void
simple_teardown_fail()
{
	cute_fail("explicit teardown failure");
}

CUTE_TEST_STATIC(simple_teardown_fail_test,
                 CUTE_NULL_SETUP,
                 simple_teardown_fail,
                 CUTE_DFLT_TMOUT)
{
}

CUTE_GROUP(simple_teardown_fail_tests) = {
	CUTE_REF(simple_teardown_fail_test),
};

CUTE_SUITE(simple_teardown_fail_suite, simple_teardown_fail_tests);

CUTE_MAIN(simple_teardown_fail_suite, "CUTe", NULL)
