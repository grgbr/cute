#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(check_sint_equal_pass_test)
{
	int chk = 2;
	int ref = 3;

	cute_check_sint(0, equal, 0);
	cute_check_sint(chk, equal, 2);
	cute_check_sint(3, equal, ref);
	chk = 3;
	cute_check_sint(chk, equal, ref);
}

CUTE_TEST(check_sint_literal_equal_fail_test)
{
	cute_check_sint(0, equal, 1);
}

CUTE_TEST(check_sint_var_equal_fail_test)
{
	int chk = 2;

	cute_check_sint(chk, equal, -1);
}

CUTE_TEST(check_sint_equal_ref_fail_test)
{
	int ref = 2;

	cute_check_sint(-1, equal, ref);
}

CUTE_TEST(check_sint_var_equal_ref_fail_test)
{
	int chk = 2;
	int ref = 3;

	cute_check_sint(chk, equal, ref);
}

CUTE_TEST(check_sint_unequal_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_check_sint(0, unequal, 1);
	cute_check_sint(chk, unequal, 0);
	cute_check_sint(0, unequal, ref);
	chk = 0;
	cute_check_sint(chk, unequal, ref);
}

CUTE_TEST(check_sint_literal_unequal_fail_test)
{
	cute_check_sint(1, unequal, 1);
}

CUTE_TEST(check_sint_var_unequal_fail_test)
{
	int chk = 2;

	cute_check_sint(chk, unequal, 2);
}

CUTE_TEST(check_sint_unequal_ref_fail_test)
{
	int ref = 2;

	cute_check_sint(2, unequal, ref);
}

CUTE_TEST(check_sint_var_unequal_ref_fail_test)
{
	int chk = 2;
	int ref = 2;

	cute_check_sint(chk, unequal, ref);
}

CUTE_TEST(check_sint_greater_pass_test)
{
	int chk = 1;
	int ref = -1;

	cute_check_sint(1, greater, 0);
	cute_check_sint(chk, greater, 0);
	cute_check_sint(0, greater, ref);
	cute_check_sint(chk, greater, ref);
}

CUTE_TEST(check_sint_literal_greater_fail_test)
{
	cute_check_sint(1, greater, 1);
}

CUTE_TEST(check_sint_var_greater_fail_test)
{
	int chk = -1;

	cute_check_sint(chk, greater, 1);
}

CUTE_TEST(check_sint_greater_ref_fail_test)
{
	int ref = 1;

	cute_check_sint(-1, greater, ref);
}

CUTE_TEST(check_sint_var_greater_ref_fail_test)
{
	int chk = -1;
	int ref = -1;

	cute_check_sint(chk, greater, ref);
}

CUTE_TEST(check_sint_greater_equal_pass_test)
{
	int chk = 1;
	int ref = -1;

	cute_check_sint(1, greater_equal, 0);
	cute_check_sint(0, greater_equal, 0);
	cute_check_sint(chk, greater_equal, 1);
	cute_check_sint(chk, greater_equal, 0);
	cute_check_sint(0, greater_equal, ref);
	cute_check_sint(-1, greater_equal, ref);
	cute_check_sint(chk, greater_equal, ref);
}

CUTE_TEST(check_sint_literal_greater_equal_fail_test)
{
	cute_check_sint(0, greater_equal, 1);
}

CUTE_TEST(check_sint_var_greater_equal_fail_test)
{
	int chk = -1;

	cute_check_sint(chk, greater_equal, 1);
}

CUTE_TEST(check_sint_greater_equal_ref_fail_test)
{
	int ref = 1;

	cute_check_sint(-1, greater_equal, ref);
}

CUTE_TEST(check_sint_var_greater_equal_ref_fail_test)
{
	int chk = -1;
	int ref = 0;

	cute_check_sint(chk, greater_equal, ref);
}

CUTE_TEST(check_sint_lower_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_check_sint(0, lower, 1);
	cute_check_sint(chk, lower, 0);
	cute_check_sint(0, lower, ref);
	cute_check_sint(chk, lower, ref);
}

CUTE_TEST(check_sint_literal_lower_fail_test)
{
	cute_check_sint(0, lower, 0);
}

CUTE_TEST(check_sint_var_lower_fail_test)
{
	int chk = 1;

	cute_check_sint(chk, lower, -1);
}

CUTE_TEST(check_sint_lower_ref_fail_test)
{
	int ref = -1;

	cute_check_sint(-1, lower, ref);
}

CUTE_TEST(check_sint_var_lower_ref_fail_test)
{
	int chk = 0;
	int ref = -1;

	cute_check_sint(chk, lower, ref);
}

CUTE_TEST(check_sint_lower_equal_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_check_sint(0, lower_equal, 1);
	cute_check_sint(0, lower_equal, 0);
	cute_check_sint(chk, lower_equal, 1);
	cute_check_sint(chk, lower_equal, -1);
	cute_check_sint(0, lower_equal, ref);
	cute_check_sint(-1, lower_equal, ref);
	cute_check_sint(chk, lower_equal, ref);
}

CUTE_TEST(check_sint_literal_lower_equal_fail_test)
{
	cute_check_sint(1, lower_equal, 0);
}

CUTE_TEST(check_sint_var_lower_equal_fail_test)
{
	int chk = 1;

	cute_check_sint(chk, lower_equal, -1);
}

CUTE_TEST(check_sint_lower_equal_ref_fail_test)
{
	int ref = -1;

	cute_check_sint(0, lower_equal, ref);
}

CUTE_TEST(check_sint_var_lower_equal_ref_fail_test)
{
	int chk = 0;
	int ref = -1;

	cute_check_sint(chk, lower_equal, ref);
}

CUTE_TEST(check_sint_in_range_pass_test)
{
	int                          chk = -1;
	int                          min_ref = -10;
	int                          max_ref = 10;
	const struct cute_sint_range range = CUTE_SINT_RANGE(min_ref, max_ref);

	cute_check_sint_range(0, in, CUTE_SINT_RANGE(-100, 100));
	cute_check_sint_range(chk, in, CUTE_SINT_RANGE(-100, 100));
	cute_check_sint_range(0, in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(chk, in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(0, in, range);
	cute_check_sint_range(chk, in, range);
}

CUTE_TEST(check_sint_literal_in_range_fail_test)
{
	cute_check_sint_range(0, in, CUTE_SINT_RANGE(1, 10));
}

CUTE_TEST(check_sint_var_in_range_fail_test)
{
	int chk = 11;

	cute_check_sint_range(chk, in, CUTE_SINT_RANGE(1, 10));
}

CUTE_TEST(check_sint_in_range_ref_fail_test)
{
	int min_ref = 5;
	int max_ref = 5;

	cute_check_sint_range(5, in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(0, in, CUTE_SINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_sint_var_in_range_ref_fail_test)
{
	int chk = 5;
	int min_ref = 5;
	int max_ref = 5;

	cute_check_sint_range(chk, in, CUTE_SINT_RANGE(min_ref, max_ref));
	chk = 6;
	cute_check_sint_range(chk, in, CUTE_SINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_sint_not_in_range_pass_test)
{
	int                          chk = -1000;
	int                          min_ref = -10;
	int                          max_ref = 10;
	const struct cute_sint_range range = CUTE_SINT_RANGE(min_ref, max_ref);

	cute_check_sint_range(1000, not_in, CUTE_SINT_RANGE(-100, 100));
	cute_check_sint_range(chk, not_in, CUTE_SINT_RANGE(-100, 100));
	cute_check_sint_range(1000, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(chk, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(1000, not_in, range);
	cute_check_sint_range(chk, not_in, range);
}

CUTE_TEST(check_sint_literal_not_in_range_fail_test)
{
	cute_check_sint_range(1, not_in, CUTE_SINT_RANGE(1, 10));
}

CUTE_TEST(check_sint_var_not_in_range_fail_test)
{
	int chk = 10;
	cute_check_sint_range(chk, not_in, CUTE_SINT_RANGE(1, 10));
}

CUTE_TEST(check_sint_not_in_range_ref_fail_test)
{
	int min_ref = 1;
	int max_ref = 10;

	cute_check_sint_range(0, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
	cute_check_sint_range(5, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_sint_var_not_in_range_ref_fail_test)
{
	int chk = 5;
	int min_ref = 5;
	int max_ref = 5;

	cute_check_sint_range(0, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
	chk = 5;
	cute_check_sint_range(chk, not_in, CUTE_SINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_sint_in_set_pass_test)
{
	int                        chk = 8;
	int                        zero = 0;
	int                        five = 5;
	const struct cute_sint_set set = CUTE_SINT_SET(0, 1, 2, 3, 5, 8);

	cute_check_sint_set(0, in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_sint_set(chk, in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_sint_set(3, in, CUTE_SINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_sint_set(chk, in, CUTE_SINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_sint_set(5, in, set);
	cute_check_sint_set(chk, in, set);
}

CUTE_TEST(check_sint_literal_in_set_fail_test)
{
	cute_check_sint_set(-1, in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_sint_var_in_set_fail_test)
{
	int chk = 4;

	cute_check_sint_set(chk, in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_sint_in_set_ref_fail_test)
{
	int                  ref0 = 0;
	int                  ref5 = 5;
	struct cute_sint_set set = CUTE_SINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_sint_set(5, in, set);
	cute_check_sint_set(4, in, set);
}

CUTE_TEST(check_sint_var_in_set_ref_fail_test)
{
	int                  chk = 0;
	int                  ref0 = 0;
	int                  ref5 = 5;
	struct cute_sint_set set = CUTE_SINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_sint_set(chk, in, set);
	chk = 10;
	cute_check_sint_set(chk, in, set);
}

CUTE_TEST(check_sint_not_in_set_pass_test)
{
	int                        chk = 13;
	int                        zero = 0;
	int                        five = 5;
	const struct cute_sint_set set = CUTE_SINT_SET(0, 1, 2, 3, 5, 8);

	cute_check_sint_set(-1, not_in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_sint_set(chk, not_in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_sint_set(21, not_in, CUTE_SINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_sint_set(chk, not_in, CUTE_SINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_sint_set(-2, not_in, set);
	cute_check_sint_set(chk, not_in, set);
}

CUTE_TEST(check_sint_literal_not_in_set_fail_test)
{
	cute_check_sint_set(1, not_in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_sint_var_not_in_set_fail_test)
{
	int chk = 8;

	cute_check_sint_set(chk, not_in, CUTE_SINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_sint_not_in_set_ref_fail_test)
{
	int                  ref0 = 0;
	int                  ref5 = 5;
	struct cute_sint_set set = CUTE_SINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_sint_set(-1, not_in, set);
	cute_check_sint_set(0, not_in, set);
}

CUTE_TEST(check_sint_var_not_in_set_ref_fail_test)
{
	int                  chk = 10;
	int                  ref0 = 0;
	int                  ref5 = 5;
	struct cute_sint_set set = CUTE_SINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_sint_set(chk, not_in, set);
	chk = 5;
	cute_check_sint_set(chk, not_in, set);
}

static CUTE_SUITE_DEFINE_TESTS(check_sint_tests) = {
	CUTE_REF(check_sint_equal_pass_test),
	CUTE_REF(check_sint_literal_equal_fail_test),
	CUTE_REF(check_sint_var_equal_fail_test),
	CUTE_REF(check_sint_equal_ref_fail_test),
	CUTE_REF(check_sint_var_equal_ref_fail_test),

	CUTE_REF(check_sint_unequal_pass_test),
	CUTE_REF(check_sint_literal_unequal_fail_test),
	CUTE_REF(check_sint_var_unequal_fail_test),
	CUTE_REF(check_sint_unequal_ref_fail_test),
	CUTE_REF(check_sint_var_unequal_ref_fail_test),

	CUTE_REF(check_sint_greater_pass_test),
	CUTE_REF(check_sint_literal_greater_fail_test),
	CUTE_REF(check_sint_var_greater_fail_test),
	CUTE_REF(check_sint_greater_ref_fail_test),
	CUTE_REF(check_sint_var_greater_ref_fail_test),

	CUTE_REF(check_sint_greater_equal_pass_test),
	CUTE_REF(check_sint_literal_greater_equal_fail_test),
	CUTE_REF(check_sint_var_greater_equal_fail_test),
	CUTE_REF(check_sint_greater_equal_ref_fail_test),
	CUTE_REF(check_sint_var_greater_equal_ref_fail_test),

	CUTE_REF(check_sint_lower_pass_test),
	CUTE_REF(check_sint_literal_lower_fail_test),
	CUTE_REF(check_sint_var_lower_fail_test),
	CUTE_REF(check_sint_lower_ref_fail_test),
	CUTE_REF(check_sint_var_lower_ref_fail_test),

	CUTE_REF(check_sint_lower_equal_pass_test),
	CUTE_REF(check_sint_literal_lower_equal_fail_test),
	CUTE_REF(check_sint_var_lower_equal_fail_test),
	CUTE_REF(check_sint_lower_equal_ref_fail_test),
	CUTE_REF(check_sint_var_lower_equal_ref_fail_test),

	CUTE_REF(check_sint_in_range_pass_test),
	CUTE_REF(check_sint_literal_in_range_fail_test),
	CUTE_REF(check_sint_var_in_range_fail_test),
	CUTE_REF(check_sint_in_range_ref_fail_test),
	CUTE_REF(check_sint_var_in_range_ref_fail_test),

	CUTE_REF(check_sint_not_in_range_pass_test),
	CUTE_REF(check_sint_literal_not_in_range_fail_test),
	CUTE_REF(check_sint_var_not_in_range_fail_test),
	CUTE_REF(check_sint_not_in_range_ref_fail_test),
	CUTE_REF(check_sint_var_not_in_range_ref_fail_test),

	CUTE_REF(check_sint_in_set_pass_test),
	CUTE_REF(check_sint_literal_in_set_fail_test),
	CUTE_REF(check_sint_var_in_set_fail_test),
	CUTE_REF(check_sint_in_set_ref_fail_test),
	CUTE_REF(check_sint_var_in_set_ref_fail_test),

	CUTE_REF(check_sint_not_in_set_pass_test),
	CUTE_REF(check_sint_literal_not_in_set_fail_test),
	CUTE_REF(check_sint_var_not_in_set_fail_test),
	CUTE_REF(check_sint_not_in_set_ref_fail_test),
	CUTE_REF(check_sint_var_not_in_set_ref_fail_test)
};

static CUTE_SUITE_DEFINE(check_sint_suite,
                         check_sint_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(check_sint_suite, "CUTe", NULL)
