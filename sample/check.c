#include <cute/check.h>
#include <stdlib.h>

CUTE_TEST(sint_literal_pass_test)
{
	cute_check_sint(0, equal, 0);
	cute_check_sint(0, unequal, 1);
	cute_check_sint(0, greater, -1);
	cute_check_sint(0, greater_equal, 0);
	cute_check_sint(-1, lower, 0);
	cute_check_sint(0, lower_equal, 0);
	cute_check_sint_range(0, in, CUTE_SINT_RANGE(-1, 1));
	cute_check_sint_range(2, not_in, CUTE_SINT_RANGE(-1, 1));
	cute_check_sint_set(0, in, CUTE_SINT_SET(-1, 0, 1));
	cute_check_sint_set(2, not_in, CUTE_SINT_SET(-1, 0, 1));
}

CUTE_TEST(sint_literal_equal_fail_test)
{
	cute_check_sint(-1, equal, 0);
}

CUTE_TEST(sint_literal_unequal_fail_test)
{
	cute_check_sint(0, unequal, 0);
}

CUTE_TEST(sint_literal_greater_fail_test)
{
	cute_check_sint(-1, greater, 0);
}

CUTE_TEST(sint_literal_greater_equal_fail_test)
{
	cute_check_sint(-1, greater_equal, 0);
}

CUTE_TEST(sint_literal_lower_fail_test)
{
	cute_check_sint(1, lower, 0);
}

CUTE_TEST(sint_literal_lower_equal_fail_test)
{
	cute_check_sint(1, lower_equal, 0);
}

CUTE_TEST(sint_literal_in_range_fail_test)
{
	cute_check_sint_range(2, in, CUTE_SINT_RANGE(-1, 1));
}

CUTE_TEST(sint_literal_not_in_range_fail_test)
{
	cute_check_sint_range(1, not_in, CUTE_SINT_RANGE(-1, 1));
}

CUTE_TEST(sint_literal_in_set_fail_test)
{
	cute_check_sint_set(10, in, CUTE_SINT_SET(-1, 2, 1, 0));
}

CUTE_TEST(sint_literal_not_in_set_fail_test)
{
	cute_check_sint_set(1, not_in, CUTE_SINT_SET(-1, 2, 1, 0));
}

static CUTE_SUITE_DEFINE_TESTS(check_sint_suite_tests) = {
	CUTE_REF(sint_literal_pass_test),
	CUTE_REF(sint_literal_equal_fail_test),
	CUTE_REF(sint_literal_unequal_fail_test),
	CUTE_REF(sint_literal_greater_fail_test),
	CUTE_REF(sint_literal_greater_equal_fail_test),
	CUTE_REF(sint_literal_lower_fail_test),
	CUTE_REF(sint_literal_lower_equal_fail_test),
	CUTE_REF(sint_literal_in_range_fail_test),
	CUTE_REF(sint_literal_not_in_range_fail_test),
	CUTE_REF(sint_literal_in_set_fail_test),
	CUTE_REF(sint_literal_not_in_set_fail_test),
};

static CUTE_SUITE_DEFINE(check_suite,
                         check_sint_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(check_suite, "CUTe check sample", CUTE_VERSION_STRING)
