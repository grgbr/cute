#include "cute/cute.h"

static void
simple_fail_setup()
{
	int * data = NULL;

	*data = 2;
}

CUTE_TEST_STATIC(simple_fail_setup_test,
                 simple_fail_setup,
                 CUTE_NULL_TEARDOWN,
                 CUTE_DFLT_TMOUT)
{
}

CUTE_GROUP(simple_fail_setup_tests) = {
	CUTE_REF(simple_fail_setup_test),
};

CUTE_SUITE(simple_fail_setup_suite, simple_fail_setup_tests);

CUTE_MAIN(simple_fail_setup_suite, "CUTe", NULL)
