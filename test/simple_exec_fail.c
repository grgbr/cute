#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(simple_exec_fail_test)
{
	cute_check_assert(0 == 1);
}

CUTE_GROUP(simple_exec_fail_tests) = {
	CUTE_REF(simple_exec_fail_test)
};

CUTE_SUITE(simple_exec_fail_suite, simple_exec_fail_tests);

CUTE_MAIN(simple_exec_fail_suite, "CUTe", NULL)
