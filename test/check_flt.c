#include "cute/cute.h"
#include "cute/check.h"
#include <math.h>

CUTE_TEST(check_flt_equal_pass_test)
{
	float chk = 2.005f;
	float ref = 3.1f;

	cute_check_flt(0, equal, 0);
	cute_check_flt(chk, equal, 2.005f);
	cute_check_flt(3.1f, equal, ref);
	chk = 3.1f;
	cute_check_flt(chk, equal, ref);
}

CUTE_TEST(check_flt_literal_equal_nan_fail_test)
{
	cute_check_flt(0, equal, NAN);
}

CUTE_TEST(check_flt_literal_equal_inf_fail_test)
{
	cute_check_flt(0, equal, -INFINITY);
}

CUTE_TEST(check_flt_literal_equal_fail_test)
{
	cute_check_flt(0, equal, 1.0f);
}

CUTE_TEST(check_flt_var_equal_fail_test)
{
	float chk = 2.005f;

	cute_check_flt(chk, equal, -1.0f);
}

CUTE_TEST(check_flt_equal_ref_fail_test)
{
	float ref = 2.005f;

	cute_check_flt(-1.0f, equal, ref);
}

CUTE_TEST(check_flt_var_equal_ref_fail_test)
{
	float chk = 2.005f;
	float ref = 3.1f;

	cute_check_flt(chk, equal, ref);
}

CUTE_TEST(check_flt_unequal_pass_test)
{
	float chk = -1.0f;
	float ref = 1.0f;

	cute_check_flt(0, unequal, 1.0f);
	cute_check_flt(chk, unequal, 0);
	cute_check_flt(0, unequal, ref);
	chk = 0;
	cute_check_flt(chk, unequal, ref);
}

CUTE_TEST(check_flt_literal_unequal_fail_test)
{
	cute_check_flt(1.0f, unequal, 1.0f);
}

CUTE_TEST(check_flt_var_unequal_fail_test)
{
	float chk = 2.005f;

	cute_check_flt(chk, unequal, 2.005f);
}

CUTE_TEST(check_flt_unequal_ref_fail_test)
{
	float ref = 2.005f;

	cute_check_flt(2.005f, unequal, ref);
}

CUTE_TEST(check_flt_var_unequal_ref_fail_test)
{
	float chk = 2.005f;
	float ref = 2.005f;

	cute_check_flt(chk, unequal, ref);
}

CUTE_TEST(check_flt_greater_pass_test)
{
	float chk = 1.0f;
	float ref = -1.0f;

	cute_check_flt(1.0f, greater, 0);
	cute_check_flt(chk, greater, 0);
	cute_check_flt(0, greater, ref);
	cute_check_flt(chk, greater, ref);
}

CUTE_TEST(check_flt_literal_greater_fail_test)
{
	cute_check_flt(1.0f, greater, 1.0f);
}

CUTE_TEST(check_flt_var_greater_fail_test)
{
	float chk = -1.0f;

	cute_check_flt(chk, greater, 1.0f);
}

CUTE_TEST(check_flt_greater_ref_fail_test)
{
	float ref = 1.0f;

	cute_check_flt(-1.0f, greater, ref);
}

CUTE_TEST(check_flt_var_greater_ref_fail_test)
{
	float chk = -1.0f;
	float ref = -1.0f;

	cute_check_flt(chk, greater, ref);
}

CUTE_TEST(check_flt_greater_equal_pass_test)
{
	float chk = 1.0f;
	float ref = -1.0f;

	cute_check_flt(1.0f, greater_equal, 0);
	cute_check_flt(0, greater_equal, 0);
	cute_check_flt(chk, greater_equal, 1.0f);
	cute_check_flt(chk, greater_equal, 0);
	cute_check_flt(0, greater_equal, ref);
	cute_check_flt(-1.0f, greater_equal, ref);
	cute_check_flt(chk, greater_equal, ref);
}

CUTE_TEST(check_flt_literal_greater_equal_fail_test)
{
	cute_check_flt(0, greater_equal, 1.0f);
}

CUTE_TEST(check_flt_var_greater_equal_fail_test)
{
	float chk = -1.0f;

	cute_check_flt(chk, greater_equal, 1.0f);
}

CUTE_TEST(check_flt_greater_equal_ref_fail_test)
{
	float ref = 1.0f;

	cute_check_flt(-1.0f, greater_equal, ref);
}

CUTE_TEST(check_flt_var_greater_equal_ref_fail_test)
{
	float chk = -1.0f;
	float ref = 0;

	cute_check_flt(chk, greater_equal, ref);
}

CUTE_TEST(check_flt_lower_pass_test)
{
	float chk = -1.0f;
	float ref = 1.0f;

	cute_check_flt(0, lower, 1.0f);
	cute_check_flt(chk, lower, 0);
	cute_check_flt(0, lower, ref);
	cute_check_flt(chk, lower, ref);
}

CUTE_TEST(check_flt_literal_lower_fail_test)
{
	cute_check_flt(0, lower, 0);
}

CUTE_TEST(check_flt_var_lower_fail_test)
{
	float chk = 1.0f;

	cute_check_flt(chk, lower, -1.0f);
}

CUTE_TEST(check_flt_lower_ref_fail_test)
{
	float ref = -1.0f;

	cute_check_flt(-1.0f, lower, ref);
}

CUTE_TEST(check_flt_var_lower_ref_fail_test)
{
	float chk = 0;
	float ref = -1.0f;

	cute_check_flt(chk, lower, ref);
}

CUTE_TEST(check_flt_lower_equal_pass_test)
{
	float chk = -1.0f;
	float ref = 1.0f;

	cute_check_flt(0, lower_equal, 1.0f);
	cute_check_flt(0, lower_equal, 0);
	cute_check_flt(chk, lower_equal, 1.0f);
	cute_check_flt(chk, lower_equal, -1.0f);
	cute_check_flt(0, lower_equal, ref);
	cute_check_flt(-1.0f, lower_equal, ref);
	cute_check_flt(chk, lower_equal, ref);
}

CUTE_TEST(check_flt_literal_lower_equal_fail_test)
{
	cute_check_flt(1.0f, lower_equal, 0);
}

CUTE_TEST(check_flt_var_lower_equal_fail_test)
{
	float chk = 1.0f;

	cute_check_flt(chk, lower_equal, -1.0f);
}

CUTE_TEST(check_flt_lower_equal_ref_fail_test)
{
	float ref = -1.0f;

	cute_check_flt(0, lower_equal, ref);
}

CUTE_TEST(check_flt_var_lower_equal_ref_fail_test)
{
	float chk = 0;
	float ref = -1.0f;

	cute_check_flt(chk, lower_equal, ref);
}

CUTE_TEST(check_flt_in_range_pass_test)
{
	float                       chk = -1.0f;
	float                       min_ref = -1.0f;
	float                       max_ref = 1.0f;
	const struct cute_flt_range range = CUTE_FLT_RANGE(min_ref, max_ref);

	cute_check_flt_range(0, in, CUTE_FLT_RANGE(-1.0f, 1.0f));
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(-1.0f, 1.0f));
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(0, in, range);
	cute_check_flt_range(chk, in, range);
}

CUTE_TEST(check_flt_literal_in_range_fail_test)
{
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(1.0f, 1.0f));
}

CUTE_TEST(check_flt_var_in_range_fail_test)
{
	float chk = 10;

	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(1.0f, 1.0f));
}

CUTE_TEST(check_flt_in_range_ref_fail_test)
{
	float min_ref = 5e-3f;
	float max_ref = 5e-3f;

	cute_check_flt_range(5e-3f, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_flt_var_in_range_ref_fail_test)
{
	float chk = 5e-3f;
	float min_ref = 5e-3f;
	float max_ref = 5e-3f;

	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
	chk = 6;
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_flt_not_in_range_pass_test)
{
	float                       chk = -1.05f;
	float                       min_ref = -1.0f;
	float                       max_ref = 1.0f;
	const struct cute_flt_range range = CUTE_FLT_RANGE(min_ref, max_ref);

	cute_check_flt_range(1.05f, not_in, CUTE_FLT_RANGE(-1.0f, 1.0f));
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(-1.0f, 1.0f));
	cute_check_flt_range(1.05f, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(1.05f, not_in, range);
	cute_check_flt_range(chk, not_in, range);
}

CUTE_TEST(check_flt_literal_not_in_range_fail_test)
{
	cute_check_flt_range(1.0f, not_in, CUTE_FLT_RANGE(1.0f, 1.0f));
}

CUTE_TEST(check_flt_var_not_in_range_fail_test)
{
	float chk = 1.0f;

	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(1.0f, 1.0f));
}

CUTE_TEST(check_flt_not_in_range_ref_fail_test)
{
	float min_ref = 1.0f;
	float max_ref = 1.0f;

	cute_check_flt_range(0, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(1.0f, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_flt_var_not_in_range_ref_fail_test)
{
	float chk = 5e-3f;
	float min_ref = 5e-3f;
	float max_ref = 5e-3f;

	cute_check_flt_range(0, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	chk = 5e-3f;
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_flt_in_set_pass_test)
{
	float                     chk = 8.26f;
	float                     zero = 0;
	float                     five = 5e-3f;
	const struct cute_flt_set set = CUTE_FLT_SET(0,
	                                             1.0f,
	                                             2.005f,
	                                             3.1f,
	                                             5e-3f,
	                                             8.26f);

	cute_check_flt_set(0, in, CUTE_FLT_SET(0,
	                                       1.0f,
	                                       2.005f,
	                                       3.1f,
	                                       5e-3f,
	                                       8.26f));
	cute_check_flt_set(chk, in, CUTE_FLT_SET(0,
	                                         1.0f,
	                                         2.005f,
	                                         3.1f,
	                                         5e-3f,
	                                         8.26f));
	cute_check_flt_set(3.1f, in, CUTE_FLT_SET(zero,
	                                          1.0f,
	                                          2.005f,
	                                          3.1f,
	                                          five,
	                                          8.26f));
	cute_check_flt_set(chk, in, CUTE_FLT_SET(zero,
	                                         1.0f,
	                                         2.005f,
	                                         3.1f,
	                                         five,
	                                         8.26f));
	cute_check_flt_set(5e-3f, in, set);
	cute_check_flt_set(chk, in, set);
}

CUTE_TEST(check_flt_literal_in_set_fail_test)
{
	cute_check_flt_set(-1.0f, in, CUTE_FLT_SET(0,
	                                           1.0f,
	                                           2.005f,
	                                           3.1f,
	                                           5e-3f,
	                                           8.26f));
}

CUTE_TEST(check_flt_var_in_set_fail_test)
{
	float chk = 4;

	cute_check_flt_set(chk, in, CUTE_FLT_SET(0,
	                                         1.0f,
	                                         2.005f,
	                                         3.1f,
	                                         5e-3f,
	                                         8.26f));
}

CUTE_TEST(check_flt_in_set_ref_fail_test)
{
	float               ref0 = 0;
	float               ref5 = 5e-3f;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0f,
	                                       2.005f,
	                                       3.1f,
	                                       ref5,
	                                       8.26f);

	cute_check_flt_set(5e-3f, in, set);
	cute_check_flt_set(4, in, set);
}

CUTE_TEST(check_flt_var_in_set_ref_fail_test)
{
	float               chk = 0;
	float               ref0 = 0;
	float               ref5 = 5e-3f;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0f,
	                                       2.005f,
	                                       3.1f,
	                                       ref5,
	                                       8.26f);

	cute_check_flt_set(chk, in, set);
	chk = 1.01f;
	cute_check_flt_set(chk, in, set);
}

CUTE_TEST(check_flt_not_in_set_pass_test)
{
	float                     chk = 13.1f;
	float                     zero = 0;
	float                     five = 5e-3f;
	const struct cute_flt_set set = CUTE_FLT_SET(0,
	                                             1.0f,
	                                             2.005f,
	                                             3.1f,
	                                             5e-3f,
	                                             8.26f);

	cute_check_flt_set(-1.0f, not_in, CUTE_FLT_SET(0,
	                                               1.0f,
	                                               2.005f,
	                                               3.1f,
	                                               5e-3f,
	                                               8.26f));
	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(0,
	                                             1.0f,
	                                             2.005f,
	                                             3.1f,
	                                             5e-3f,
	                                             8.26f));
	cute_check_flt_set(2.25f, not_in, CUTE_FLT_SET(zero,
	                                               1.0f,
	                                               2.005f,
	                                               3.1f,
	                                               five,
	                                               8.26f));
	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(zero,
	                                             1.0f,
	                                             2.005f,
	                                             3.1f,
	                                             five,
	                                             8.26f));
	cute_check_flt_set(-2.25f, not_in, set);
	cute_check_flt_set(chk, not_in, set);
}

CUTE_TEST(check_flt_literal_not_in_set_fail_test)
{
	cute_check_flt_set(1.0f, not_in, CUTE_FLT_SET(0,
	                                              1.0f,
	                                              2.005f,
	                                              3.1f,
	                                              5e-3f,
	                                              8.26f));
}

CUTE_TEST(check_flt_var_not_in_set_fail_test)
{
	float chk = 8.26f;

	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(0,
	                                             1.0f,
	                                             2.005f,
	                                             3.1f,
	                                             5e-3f,
	                                             8.26f));
}

CUTE_TEST(check_flt_not_in_set_ref_fail_test)
{
	float               ref0 = 0;
	float               ref5 = 5e-3f;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0f,
	                                       2.005f,
	                                       3.1f,
	                                       ref5,
	                                       8.26f);

	cute_check_flt_set(-1.0f, not_in, set);
	cute_check_flt_set(0, not_in, set);
}

CUTE_TEST(check_flt_var_not_in_set_ref_fail_test)
{
	float               chk = 1.0f;
	float               ref0 = 0;
	float               ref5 = 5e-3f;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0f,
	                                       2.005f,
	                                       3.1f,
	                                       ref5,
	                                       8.26f);

	cute_check_flt_set(chk, not_in, set);
	chk = 5e-3f;
	cute_check_flt_set(chk, not_in, set);
}

CUTE_GROUP(check_flt_tests) = {
	CUTE_REF(check_flt_equal_pass_test),
	CUTE_REF(check_flt_literal_equal_nan_fail_test),
	CUTE_REF(check_flt_literal_equal_inf_fail_test),
	CUTE_REF(check_flt_literal_equal_fail_test),
	CUTE_REF(check_flt_var_equal_fail_test),
	CUTE_REF(check_flt_equal_ref_fail_test),
	CUTE_REF(check_flt_var_equal_ref_fail_test),

	CUTE_REF(check_flt_unequal_pass_test),
	CUTE_REF(check_flt_literal_unequal_fail_test),
	CUTE_REF(check_flt_var_unequal_fail_test),
	CUTE_REF(check_flt_unequal_ref_fail_test),
	CUTE_REF(check_flt_var_unequal_ref_fail_test),

	CUTE_REF(check_flt_greater_pass_test),
	CUTE_REF(check_flt_literal_greater_fail_test),
	CUTE_REF(check_flt_var_greater_fail_test),
	CUTE_REF(check_flt_greater_ref_fail_test),
	CUTE_REF(check_flt_var_greater_ref_fail_test),

	CUTE_REF(check_flt_greater_equal_pass_test),
	CUTE_REF(check_flt_literal_greater_equal_fail_test),
	CUTE_REF(check_flt_var_greater_equal_fail_test),
	CUTE_REF(check_flt_greater_equal_ref_fail_test),
	CUTE_REF(check_flt_var_greater_equal_ref_fail_test),

	CUTE_REF(check_flt_lower_pass_test),
	CUTE_REF(check_flt_literal_lower_fail_test),
	CUTE_REF(check_flt_var_lower_fail_test),
	CUTE_REF(check_flt_lower_ref_fail_test),
	CUTE_REF(check_flt_var_lower_ref_fail_test),

	CUTE_REF(check_flt_lower_equal_pass_test),
	CUTE_REF(check_flt_literal_lower_equal_fail_test),
	CUTE_REF(check_flt_var_lower_equal_fail_test),
	CUTE_REF(check_flt_lower_equal_ref_fail_test),
	CUTE_REF(check_flt_var_lower_equal_ref_fail_test),

	CUTE_REF(check_flt_in_range_pass_test),
	CUTE_REF(check_flt_literal_in_range_fail_test),
	CUTE_REF(check_flt_var_in_range_fail_test),
	CUTE_REF(check_flt_in_range_ref_fail_test),
	CUTE_REF(check_flt_var_in_range_ref_fail_test),

	CUTE_REF(check_flt_not_in_range_pass_test),
	CUTE_REF(check_flt_literal_not_in_range_fail_test),
	CUTE_REF(check_flt_var_not_in_range_fail_test),
	CUTE_REF(check_flt_not_in_range_ref_fail_test),
	CUTE_REF(check_flt_var_not_in_range_ref_fail_test),

	CUTE_REF(check_flt_in_set_pass_test),
	CUTE_REF(check_flt_literal_in_set_fail_test),
	CUTE_REF(check_flt_var_in_set_fail_test),
	CUTE_REF(check_flt_in_set_ref_fail_test),
	CUTE_REF(check_flt_var_in_set_ref_fail_test),

	CUTE_REF(check_flt_not_in_set_pass_test),
	CUTE_REF(check_flt_literal_not_in_set_fail_test),
	CUTE_REF(check_flt_var_not_in_set_fail_test),
	CUTE_REF(check_flt_not_in_set_ref_fail_test),
	CUTE_REF(check_flt_var_not_in_set_ref_fail_test)
};

CUTE_SUITE(check_flt_suite, check_flt_tests);

CUTE_MAIN(check_flt_suite, "CUTe", NULL)
