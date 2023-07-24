#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(check_uint_equal_pass_test)
{
	unsigned int chk = 2;
	unsigned int ref = 3;

	cute_check_uint(0, equal, 0);
	cute_check_uint(chk, equal, 2);
	cute_check_uint(3, equal, ref);
	chk = 3;
	cute_check_uint(chk, equal, ref);
}

CUTE_TEST(check_uint_literal_equal_fail_test)
{
	cute_check_uint(0, equal, 1);
}

CUTE_TEST(check_uint_var_equal_fail_test)
{
	unsigned int chk = 2;

	cute_check_uint(chk, equal, 1);
}

CUTE_TEST(check_uint_equal_ref_fail_test)
{
	unsigned int ref = 2;

	cute_check_uint(1, equal, ref);
}

CUTE_TEST(check_uint_var_equal_ref_fail_test)
{
	unsigned int chk = 2;
	unsigned int ref = 3;

	cute_check_uint(chk, equal, ref);
}

CUTE_TEST(check_uint_unequal_pass_test)
{
	unsigned int chk = 2;
	unsigned int ref = 1;

	cute_check_uint(0, unequal, 1);
	cute_check_uint(chk, unequal, 0);
	cute_check_uint(0, unequal, ref);
	chk = 0;
	cute_check_uint(chk, unequal, ref);
}

CUTE_TEST(check_uint_literal_unequal_fail_test)
{
	cute_check_uint(1, unequal, 1);
}

CUTE_TEST(check_uint_var_unequal_fail_test)
{
	unsigned int chk = 2;

	cute_check_uint(chk, unequal, 2);
}

CUTE_TEST(check_uint_unequal_ref_fail_test)
{
	unsigned int ref = 2;

	cute_check_uint(2, unequal, ref);
}

CUTE_TEST(check_uint_var_unequal_ref_fail_test)
{
	unsigned int chk = 2;
	unsigned int ref = 2;

	cute_check_uint(chk, unequal, ref);
}

CUTE_TEST(check_uint_greater_pass_test)
{
	unsigned int chk = 1;
	unsigned int ref = 0;

	cute_check_uint(1, greater, 0);
	cute_check_uint(chk, greater, 0);
	cute_check_uint(2, greater, ref);
	cute_check_uint(chk, greater, ref);
}

CUTE_TEST(check_uint_literal_greater_fail_test)
{
	cute_check_uint(1, greater, 1);
}

CUTE_TEST(check_uint_var_greater_fail_test)
{
	unsigned int chk = 0;

	cute_check_uint(chk, greater, 1);
}

CUTE_TEST(check_uint_greater_ref_fail_test)
{
	unsigned int ref = 1;

	cute_check_uint(0, greater, ref);
}

CUTE_TEST(check_uint_var_greater_ref_fail_test)
{
	unsigned int chk = 1;
	unsigned int ref = 10;

	cute_check_uint(chk, greater, ref);
}

CUTE_TEST(check_uint_greater_equal_pass_test)
{
	unsigned int chk = 1;
	unsigned int ref = 0;

	cute_check_uint(1, greater_equal, 0);
	cute_check_uint(0, greater_equal, 0);
	cute_check_uint(chk, greater_equal, 1);
	cute_check_uint(chk, greater_equal, 0);
	cute_check_uint(2, greater_equal, ref);
	cute_check_uint(0, greater_equal, ref);
	cute_check_uint(chk, greater_equal, ref);
}

CUTE_TEST(check_uint_literal_greater_equal_fail_test)
{
	cute_check_uint(0, greater_equal, 1);
}

CUTE_TEST(check_uint_var_greater_equal_fail_test)
{
	unsigned int chk = 0;

	cute_check_uint(chk, greater_equal, 1);
}

CUTE_TEST(check_uint_greater_equal_ref_fail_test)
{
	unsigned int ref = 1;

	cute_check_uint(0, greater_equal, ref);
}

CUTE_TEST(check_uint_var_greater_equal_ref_fail_test)
{
	unsigned int chk = 0;
	unsigned int ref = 10;

	cute_check_uint(chk, greater_equal, ref);
}

CUTE_TEST(check_uint_lower_pass_test)
{
	unsigned int chk = 1;
	unsigned int ref = 10;

	cute_check_uint(0, lower, 10);
	cute_check_uint(chk, lower, 10);
	cute_check_uint(0, lower, ref);
	cute_check_uint(chk, lower, ref);
}

CUTE_TEST(check_uint_literal_lower_fail_test)
{
	cute_check_uint(0, lower, 0);
}

CUTE_TEST(check_uint_var_lower_fail_test)
{
	unsigned int chk = 1;

	cute_check_uint(chk, lower, 0);
}

CUTE_TEST(check_uint_lower_ref_fail_test)
{
	unsigned int ref = 10;

	cute_check_uint(10, lower, ref);
}

CUTE_TEST(check_uint_var_lower_ref_fail_test)
{
	unsigned int chk = 1;
	unsigned int ref = 0;

	cute_check_uint(chk, lower, ref);
}

CUTE_TEST(check_uint_lower_equal_pass_test)
{
	unsigned int chk = 0;
	unsigned int ref = 2;

	cute_check_uint(0, lower_equal, 1);
	cute_check_uint(0, lower_equal, 0);
	cute_check_uint(chk, lower_equal, 1);
	cute_check_uint(chk, lower_equal, 0);
	cute_check_uint(0, lower_equal, ref);
	cute_check_uint(2, lower_equal, ref);
	cute_check_uint(chk, lower_equal, ref);
}

CUTE_TEST(check_uint_literal_lower_equal_fail_test)
{
	cute_check_uint(1, lower_equal, 0);
}

CUTE_TEST(check_uint_var_lower_equal_fail_test)
{
	unsigned int chk = 1;

	cute_check_uint(chk, lower_equal, 0);
}

CUTE_TEST(check_uint_lower_equal_ref_fail_test)
{
	unsigned int ref = 1;

	cute_check_uint(10, lower_equal, ref);
}

CUTE_TEST(check_uint_var_lower_equal_ref_fail_test)
{
	unsigned int chk = 10;
	unsigned int ref = 0;

	cute_check_uint(chk, lower_equal, ref);
}

CUTE_TEST(check_uint_in_range_pass_test)
{
	unsigned int                 chk = 1;
	unsigned int                 min_ref = 0;
	unsigned int                 max_ref = 10;
	const struct cute_uint_range range = CUTE_UINT_RANGE(min_ref, max_ref);

	cute_check_uint_range(0, in, CUTE_UINT_RANGE(0, 100));
	cute_check_uint_range(chk, in, CUTE_UINT_RANGE(0, 1));
	cute_check_uint_range(0, in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(chk, in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(0, in, range);
	cute_check_uint_range(chk, in, range);
}

CUTE_TEST(check_uint_literal_in_range_fail_test)
{
	cute_check_uint_range(0, in, CUTE_UINT_RANGE(1, 10));
}

CUTE_TEST(check_uint_var_in_range_fail_test)
{
	unsigned int chk = 11;

	cute_check_uint_range(chk, in, CUTE_UINT_RANGE(1, 10));
}

CUTE_TEST(check_uint_in_range_ref_fail_test)
{
	unsigned int min_ref = 5;
	unsigned int max_ref = 5;

	cute_check_uint_range(5, in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(0, in, CUTE_UINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_uint_var_in_range_ref_fail_test)
{
	unsigned int chk = 5;
	unsigned int min_ref = 5;
	unsigned int max_ref = 5;

	cute_check_uint_range(chk, in, CUTE_UINT_RANGE(min_ref, max_ref));
	chk = 6;
	cute_check_uint_range(chk, in, CUTE_UINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_uint_not_in_range_pass_test)
{
	unsigned int                 chk = 0;
	unsigned int                 min_ref = 5;
	unsigned int                 max_ref = 10;
	const struct cute_uint_range range = CUTE_UINT_RANGE(min_ref, max_ref);

	cute_check_uint_range(1000, not_in, CUTE_UINT_RANGE(10, 100));
	cute_check_uint_range(chk, not_in, CUTE_UINT_RANGE(10, 100));
	cute_check_uint_range(1000, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(chk, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(1000, not_in, range);
	cute_check_uint_range(chk, not_in, range);
}

CUTE_TEST(check_uint_literal_not_in_range_fail_test)
{
	cute_check_uint_range(1, not_in, CUTE_UINT_RANGE(1, 10));
}

CUTE_TEST(check_uint_var_not_in_range_fail_test)
{
	unsigned int chk = 10;

	cute_check_uint_range(chk, not_in, CUTE_UINT_RANGE(1, 10));
}

CUTE_TEST(check_uint_not_in_range_ref_fail_test)
{
	unsigned int min_ref = 1;
	unsigned int max_ref = 10;

	cute_check_uint_range(0, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
	cute_check_uint_range(5, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_uint_var_not_in_range_ref_fail_test)
{
	unsigned int chk = 5;
	unsigned int min_ref = 5;
	unsigned int max_ref = 5;

	cute_check_uint_range(0, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
	chk = 5;
	cute_check_uint_range(chk, not_in, CUTE_UINT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_uint_in_set_pass_test)
{
	unsigned int               chk = 8;
	unsigned int               zero = 0;
	unsigned int               five = 5;
	const struct cute_uint_set set = CUTE_UINT_SET(0, 1, 2, 3, 5, 8);

	cute_check_uint_set(0, in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_uint_set(chk, in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_uint_set(3, in, CUTE_UINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_uint_set(chk, in, CUTE_UINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_uint_set(5, in, set);
	cute_check_uint_set(chk, in, set);
}

CUTE_TEST(check_uint_literal_in_set_fail_test)
{
	cute_check_uint_set(10, in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_uint_var_in_set_fail_test)
{
	unsigned int chk = 4;

	cute_check_uint_set(chk, in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_uint_in_set_ref_fail_test)
{
	unsigned int         ref0 = 0;
	unsigned int         ref5 = 5;
	struct cute_uint_set set = CUTE_UINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_uint_set(5, in, set);
	cute_check_uint_set(4, in, set);
}

CUTE_TEST(check_uint_var_in_set_ref_fail_test)
{
	unsigned int         chk = 0;
	unsigned int         ref0 = 0;
	unsigned int         ref5 = 5;
	struct cute_uint_set set = CUTE_UINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_uint_set(chk, in, set);
	chk = 10;
	cute_check_uint_set(chk, in, set);
}

CUTE_TEST(check_uint_not_in_set_pass_test)
{
	unsigned int               chk = 13;
	unsigned int               zero = 0;
	unsigned int               five = 5;
	const struct cute_uint_set set = CUTE_UINT_SET(0, 1, 2, 3, 5, 8);

	cute_check_uint_set(7, not_in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_uint_set(chk, not_in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
	cute_check_uint_set(21, not_in, CUTE_UINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_uint_set(chk, not_in, CUTE_UINT_SET(zero, 1, 2, 3, five, 8));
	cute_check_uint_set(7, not_in, set);
	cute_check_uint_set(chk, not_in, set);
}

CUTE_TEST(check_uint_literal_not_in_set_fail_test)
{
	cute_check_uint_set(1, not_in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_uint_var_not_in_set_fail_test)
{
	unsigned int chk = 8;

	cute_check_uint_set(chk, not_in, CUTE_UINT_SET(0, 1, 2, 3, 5, 8));
}

CUTE_TEST(check_uint_not_in_set_ref_fail_test)
{
	unsigned int         ref0 = 0;
	unsigned int         ref5 = 5;
	struct cute_uint_set set = CUTE_UINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_uint_set(6, not_in, set);
	cute_check_uint_set(0, not_in, set);
}

CUTE_TEST(check_uint_var_not_in_set_ref_fail_test)
{
	unsigned int         chk = 10;
	unsigned int         ref0 = 0;
	unsigned int         ref5 = 5;
	struct cute_uint_set set = CUTE_UINT_SET(ref0, 1, 2, 3, ref5, 8);

	cute_check_uint_set(chk, not_in, set);
	chk = 5;
	cute_check_uint_set(chk, not_in, set);
}

CUTE_GROUP(check_uint_tests) = {
	CUTE_REF(check_uint_equal_pass_test),
	CUTE_REF(check_uint_literal_equal_fail_test),
	CUTE_REF(check_uint_var_equal_fail_test),
	CUTE_REF(check_uint_equal_ref_fail_test),
	CUTE_REF(check_uint_var_equal_ref_fail_test),

	CUTE_REF(check_uint_unequal_pass_test),
	CUTE_REF(check_uint_literal_unequal_fail_test),
	CUTE_REF(check_uint_var_unequal_fail_test),
	CUTE_REF(check_uint_unequal_ref_fail_test),
	CUTE_REF(check_uint_var_unequal_ref_fail_test),

	CUTE_REF(check_uint_greater_pass_test),
	CUTE_REF(check_uint_literal_greater_fail_test),
	CUTE_REF(check_uint_var_greater_fail_test),
	CUTE_REF(check_uint_greater_ref_fail_test),
	CUTE_REF(check_uint_var_greater_ref_fail_test),

	CUTE_REF(check_uint_greater_equal_pass_test),
	CUTE_REF(check_uint_literal_greater_equal_fail_test),
	CUTE_REF(check_uint_var_greater_equal_fail_test),
	CUTE_REF(check_uint_greater_equal_ref_fail_test),
	CUTE_REF(check_uint_var_greater_equal_ref_fail_test),

	CUTE_REF(check_uint_lower_pass_test),
	CUTE_REF(check_uint_literal_lower_fail_test),
	CUTE_REF(check_uint_var_lower_fail_test),
	CUTE_REF(check_uint_lower_ref_fail_test),
	CUTE_REF(check_uint_var_lower_ref_fail_test),

	CUTE_REF(check_uint_lower_equal_pass_test),
	CUTE_REF(check_uint_literal_lower_equal_fail_test),
	CUTE_REF(check_uint_var_lower_equal_fail_test),
	CUTE_REF(check_uint_lower_equal_ref_fail_test),
	CUTE_REF(check_uint_var_lower_equal_ref_fail_test),

	CUTE_REF(check_uint_in_range_pass_test),
	CUTE_REF(check_uint_literal_in_range_fail_test),
	CUTE_REF(check_uint_var_in_range_fail_test),
	CUTE_REF(check_uint_in_range_ref_fail_test),
	CUTE_REF(check_uint_var_in_range_ref_fail_test),

	CUTE_REF(check_uint_not_in_range_pass_test),
	CUTE_REF(check_uint_literal_not_in_range_fail_test),
	CUTE_REF(check_uint_var_not_in_range_fail_test),
	CUTE_REF(check_uint_not_in_range_ref_fail_test),
	CUTE_REF(check_uint_var_not_in_range_ref_fail_test),

	CUTE_REF(check_uint_in_set_pass_test),
	CUTE_REF(check_uint_literal_in_set_fail_test),
	CUTE_REF(check_uint_var_in_set_fail_test),
	CUTE_REF(check_uint_in_set_ref_fail_test),
	CUTE_REF(check_uint_var_in_set_ref_fail_test),

	CUTE_REF(check_uint_not_in_set_pass_test),
	CUTE_REF(check_uint_literal_not_in_set_fail_test),
	CUTE_REF(check_uint_var_not_in_set_fail_test),
	CUTE_REF(check_uint_not_in_set_ref_fail_test),
	CUTE_REF(check_uint_var_not_in_set_ref_fail_test)
};

CUTE_SUITE(check_uint_suite, check_uint_tests);

CUTE_MAIN(check_uint_suite, "CUTe", NULL)
