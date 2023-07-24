#include "cute/cute.h"
#include "cute/check.h"
#include <math.h>

CUTE_TEST(check_dbl_equal_pass_test)
{
	double chk = 2.005;
	double ref = 3.1;

	cute_check_flt(0, equal, 0);
	cute_check_flt(chk, equal, 2.005);
	cute_check_flt(3.1, equal, ref);
	chk = 3.1;
	cute_check_flt(chk, equal, ref);
}

CUTE_TEST(check_dbl_literal_equal_nan_fail_test)
{
	cute_check_flt(0, equal, NAN);
}

CUTE_TEST(check_dbl_literal_equal_inf_fail_test)
{
	cute_check_flt(0, equal, -INFINITY);
}

CUTE_TEST(check_dbl_literal_equal_fail_test)
{
	cute_check_flt(0, equal, 1.0);
}

CUTE_TEST(check_dbl_var_equal_fail_test)
{
	double chk = 2.005;

	cute_check_flt(chk, equal, -1.0);
}

CUTE_TEST(check_dbl_equal_ref_fail_test)
{
	double ref = 2.005;

	cute_check_flt(-1.0, equal, ref);
}

CUTE_TEST(check_dbl_var_equal_ref_fail_test)
{
	double chk = 2.005;
	double ref = 3.1;

	cute_check_flt(chk, equal, ref);
}

CUTE_TEST(check_dbl_unequal_pass_test)
{
	double chk = -1.0;
	double ref = 1.0;

	cute_check_flt(0, unequal, 1.0);
	cute_check_flt(chk, unequal, 0);
	cute_check_flt(0, unequal, ref);
	chk = 0;
	cute_check_flt(chk, unequal, ref);
}

CUTE_TEST(check_dbl_literal_unequal_fail_test)
{
	cute_check_flt(1.0, unequal, 1.0);
}

CUTE_TEST(check_dbl_var_unequal_fail_test)
{
	double chk = 2.005;

	cute_check_flt(chk, unequal, 2.005);
}

CUTE_TEST(check_dbl_unequal_ref_fail_test)
{
	double ref = 2.005;

	cute_check_flt(2.005, unequal, ref);
}

CUTE_TEST(check_dbl_var_unequal_ref_fail_test)
{
	double chk = 2.005;
	double ref = 2.005;

	cute_check_flt(chk, unequal, ref);
}

CUTE_TEST(check_dbl_greater_pass_test)
{
	double chk = 1.0;
	double ref = -1.0;

	cute_check_flt(1.0, greater, 0);
	cute_check_flt(chk, greater, 0);
	cute_check_flt(0, greater, ref);
	cute_check_flt(chk, greater, ref);
}

CUTE_TEST(check_dbl_literal_greater_fail_test)
{
	cute_check_flt(1.0, greater, 1.0);
}

CUTE_TEST(check_dbl_var_greater_fail_test)
{
	double chk = -1.0;

	cute_check_flt(chk, greater, 1.0);
}

CUTE_TEST(check_dbl_greater_ref_fail_test)
{
	double ref = 1.0;

	cute_check_flt(-1.0, greater, ref);
}

CUTE_TEST(check_dbl_var_greater_ref_fail_test)
{
	double chk = -1.0;
	double ref = -1.0;

	cute_check_flt(chk, greater, ref);
}

CUTE_TEST(check_dbl_greater_equal_pass_test)
{
	double chk = 1.0;
	double ref = -1.0;

	cute_check_flt(1.0, greater_equal, 0);
	cute_check_flt(0, greater_equal, 0);
	cute_check_flt(chk, greater_equal, 1.0);
	cute_check_flt(chk, greater_equal, 0);
	cute_check_flt(0, greater_equal, ref);
	cute_check_flt(-1.0, greater_equal, ref);
	cute_check_flt(chk, greater_equal, ref);
}

CUTE_TEST(check_dbl_literal_greater_equal_fail_test)
{
	cute_check_flt(0, greater_equal, 1.0);
}

CUTE_TEST(check_dbl_var_greater_equal_fail_test)
{
	double chk = -1.0;

	cute_check_flt(chk, greater_equal, 1.0);
}

CUTE_TEST(check_dbl_greater_equal_ref_fail_test)
{
	double ref = 1.0;

	cute_check_flt(-1.0, greater_equal, ref);
}

CUTE_TEST(check_dbl_var_greater_equal_ref_fail_test)
{
	double chk = -1.0;
	double ref = 0;

	cute_check_flt(chk, greater_equal, ref);
}

CUTE_TEST(check_dbl_lower_pass_test)
{
	double chk = -1.0;
	double ref = 1.0;

	cute_check_flt(0, lower, 1.0);
	cute_check_flt(chk, lower, 0);
	cute_check_flt(0, lower, ref);
	cute_check_flt(chk, lower, ref);
}

CUTE_TEST(check_dbl_literal_lower_fail_test)
{
	cute_check_flt(0, lower, 0);
}

CUTE_TEST(check_dbl_var_lower_fail_test)
{
	double chk = 1.0;

	cute_check_flt(chk, lower, -1.0);
}

CUTE_TEST(check_dbl_lower_ref_fail_test)
{
	double ref = -1.0;

	cute_check_flt(-1.0, lower, ref);
}

CUTE_TEST(check_dbl_var_lower_ref_fail_test)
{
	double chk = 0;
	double ref = -1.0;

	cute_check_flt(chk, lower, ref);
}

CUTE_TEST(check_dbl_lower_equal_pass_test)
{
	double chk = -1.0;
	double ref = 1.0;

	cute_check_flt(0, lower_equal, 1.0);
	cute_check_flt(0, lower_equal, 0);
	cute_check_flt(chk, lower_equal, 1.0);
	cute_check_flt(chk, lower_equal, -1.0);
	cute_check_flt(0, lower_equal, ref);
	cute_check_flt(-1.0, lower_equal, ref);
	cute_check_flt(chk, lower_equal, ref);
}

CUTE_TEST(check_dbl_literal_lower_equal_fail_test)
{
	cute_check_flt(1.0, lower_equal, 0);
}

CUTE_TEST(check_dbl_var_lower_equal_fail_test)
{
	double chk = 1.0;

	cute_check_flt(chk, lower_equal, -1.0);
}

CUTE_TEST(check_dbl_lower_equal_ref_fail_test)
{
	double ref = -1.0;

	cute_check_flt(0, lower_equal, ref);
}

CUTE_TEST(check_dbl_var_lower_equal_ref_fail_test)
{
	double chk = 0;
	double ref = -1.0;

	cute_check_flt(chk, lower_equal, ref);
}

CUTE_TEST(check_dbl_in_range_pass_test)
{
	double                      chk = -1.0;
	double                      min_ref = -1.0;
	double                      max_ref = 1.0;
	const struct cute_flt_range range = CUTE_FLT_RANGE(min_ref, max_ref);

	cute_check_flt_range(0, in, CUTE_FLT_RANGE(-1.0, 1.0));
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(-1.0, 1.0));
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(0, in, range);
	cute_check_flt_range(chk, in, range);
}

CUTE_TEST(check_dbl_literal_in_range_fail_test)
{
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(1.0, 1.0));
}

CUTE_TEST(check_dbl_var_in_range_fail_test)
{
	double chk = 10;

	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(1.0, 1.0));
}

CUTE_TEST(check_dbl_in_range_ref_fail_test)
{
	double min_ref = 5e-3;
	double max_ref = 5e-3;

	cute_check_flt_range(5e-3, in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(0, in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_dbl_var_in_range_ref_fail_test)
{
	double chk = 5e-3;
	double min_ref = 5e-3;
	double max_ref = 5e-3;

	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
	chk = 6;
	cute_check_flt_range(chk, in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_dbl_not_in_range_pass_test)
{
	double                      chk = -1.05;
	double                      min_ref = -1.0;
	double                      max_ref = 1.0;
	const struct cute_flt_range range = CUTE_FLT_RANGE(min_ref, max_ref);

	cute_check_flt_range(1.05, not_in, CUTE_FLT_RANGE(-1.0, 1.0));
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(-1.0, 1.0));
	cute_check_flt_range(1.05, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(1.05, not_in, range);
	cute_check_flt_range(chk, not_in, range);
}

CUTE_TEST(check_dbl_literal_not_in_range_fail_test)
{
	cute_check_flt_range(1.0, not_in, CUTE_FLT_RANGE(1.0, 1.0));
}

CUTE_TEST(check_dbl_var_not_in_range_fail_test)
{
	double chk = 1.0;

	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(1.0, 1.0));
}

CUTE_TEST(check_dbl_not_in_range_ref_fail_test)
{
	double min_ref = 1.0;
	double max_ref = 1.0;

	cute_check_flt_range(0, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	cute_check_flt_range(1.0, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_dbl_var_not_in_range_ref_fail_test)
{
	double chk = 5e-3;
	double min_ref = 5e-3;
	double max_ref = 5e-3;

	cute_check_flt_range(0, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
	chk = 5e-3;
	cute_check_flt_range(chk, not_in, CUTE_FLT_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_dbl_in_set_pass_test)
{
	double                    chk = 8.26;
	double                    zero = 0;
	double                    five = 5e-3;
	const struct cute_flt_set set = CUTE_FLT_SET(0,
	                                             1.0,
	                                             2.005,
	                                             3.1,
	                                             5e-3,
	                                             8.26);

	cute_check_flt_set(0, in, CUTE_FLT_SET(0,
	                                       1.0,
	                                       2.005,
	                                       3.1,
	                                       5e-3,
	                                       8.26));
	cute_check_flt_set(chk, in, CUTE_FLT_SET(0,
	                                         1.0,
	                                         2.005,
	                                         3.1,
	                                         5e-3,
	                                         8.26));
	cute_check_flt_set(3.1, in, CUTE_FLT_SET(zero,
	                                          1.0,
	                                          2.005,
	                                          3.1,
	                                          five,
	                                          8.26));
	cute_check_flt_set(chk, in, CUTE_FLT_SET(zero,
	                                         1.0,
	                                         2.005,
	                                         3.1,
	                                         five,
	                                         8.26));
	cute_check_flt_set(5e-3, in, set);
	cute_check_flt_set(chk, in, set);
}

CUTE_TEST(check_dbl_literal_in_set_fail_test)
{
	cute_check_flt_set(-1.0, in, CUTE_FLT_SET(0,
	                                           1.0,
	                                           2.005,
	                                           3.1,
	                                           5e-3,
	                                           8.26));
}

CUTE_TEST(check_dbl_var_in_set_fail_test)
{
	double chk = 4;

	cute_check_flt_set(chk, in, CUTE_FLT_SET(0,
	                                         1.0,
	                                         2.005,
	                                         3.1,
	                                         5e-3,
	                                         8.26));
}

CUTE_TEST(check_dbl_in_set_ref_fail_test)
{
	double              ref0 = 0;
	double              ref5 = 5e-3;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0,
	                                       2.005,
	                                       3.1,
	                                       ref5,
	                                       8.26);

	cute_check_flt_set(5e-3, in, set);
	cute_check_flt_set(4, in, set);
}

CUTE_TEST(check_dbl_var_in_set_ref_fail_test)
{
	double              chk = 0;
	double              ref0 = 0;
	double              ref5 = 5e-3;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0,
	                                       2.005,
	                                       3.1,
	                                       ref5,
	                                       8.26);

	cute_check_flt_set(chk, in, set);
	chk = 1.01;
	cute_check_flt_set(chk, in, set);
}

CUTE_TEST(check_dbl_not_in_set_pass_test)
{
	double                    chk = 13.1;
	double                    zero = 0;
	double                    five = 5e-3;
	const struct cute_flt_set set = CUTE_FLT_SET(0,
	                                             1.0,
	                                             2.005,
	                                             3.1,
	                                             5e-3,
	                                             8.26);

	cute_check_flt_set(-1.0, not_in, CUTE_FLT_SET(0,
	                                               1.0,
	                                               2.005,
	                                               3.1,
	                                               5e-3,
	                                               8.26));
	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(0,
	                                             1.0,
	                                             2.005,
	                                             3.1,
	                                             5e-3,
	                                             8.26));
	cute_check_flt_set(2.25, not_in, CUTE_FLT_SET(zero,
	                                               1.0,
	                                               2.005,
	                                               3.1,
	                                               five,
	                                               8.26));
	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(zero,
	                                             1.0,
	                                             2.005,
	                                             3.1,
	                                             five,
	                                             8.26));
	cute_check_flt_set(-2.25, not_in, set);
	cute_check_flt_set(chk, not_in, set);
}

CUTE_TEST(check_dbl_literal_not_in_set_fail_test)
{
	cute_check_flt_set(1.0, not_in, CUTE_FLT_SET(0,
	                                              1.0,
	                                              2.005,
	                                              3.1,
	                                              5e-3,
	                                              8.26));
}

CUTE_TEST(check_dbl_var_not_in_set_fail_test)
{
	double chk = 8.26;

	cute_check_flt_set(chk, not_in, CUTE_FLT_SET(0,
	                                             1.0,
	                                             2.005,
	                                             3.1,
	                                             5e-3,
	                                             8.26));
}

CUTE_TEST(check_dbl_not_in_set_ref_fail_test)
{
	double              ref0 = 0;
	double              ref5 = 5e-3;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0,
	                                       2.005,
	                                       3.1,
	                                       ref5,
	                                       8.26);

	cute_check_flt_set(-1.0, not_in, set);
	cute_check_flt_set(0, not_in, set);
}

CUTE_TEST(check_dbl_var_not_in_set_ref_fail_test)
{
	double              chk = 1.0;
	double              ref0 = 0;
	double              ref5 = 5e-3;
	struct cute_flt_set set = CUTE_FLT_SET(ref0,
	                                       1.0,
	                                       2.005,
	                                       3.1,
	                                       ref5,
	                                       8.26);

	cute_check_flt_set(chk, not_in, set);
	chk = 5e-3;
	cute_check_flt_set(chk, not_in, set);
}

static CUTE_SUITE_DEFINE_TESTS(check_dbl_tests) = {
	CUTE_REF(check_dbl_equal_pass_test),
	CUTE_REF(check_dbl_literal_equal_nan_fail_test),
	CUTE_REF(check_dbl_literal_equal_inf_fail_test),
	CUTE_REF(check_dbl_literal_equal_fail_test),
	CUTE_REF(check_dbl_var_equal_fail_test),
	CUTE_REF(check_dbl_equal_ref_fail_test),
	CUTE_REF(check_dbl_var_equal_ref_fail_test),

	CUTE_REF(check_dbl_unequal_pass_test),
	CUTE_REF(check_dbl_literal_unequal_fail_test),
	CUTE_REF(check_dbl_var_unequal_fail_test),
	CUTE_REF(check_dbl_unequal_ref_fail_test),
	CUTE_REF(check_dbl_var_unequal_ref_fail_test),

	CUTE_REF(check_dbl_greater_pass_test),
	CUTE_REF(check_dbl_literal_greater_fail_test),
	CUTE_REF(check_dbl_var_greater_fail_test),
	CUTE_REF(check_dbl_greater_ref_fail_test),
	CUTE_REF(check_dbl_var_greater_ref_fail_test),

	CUTE_REF(check_dbl_greater_equal_pass_test),
	CUTE_REF(check_dbl_literal_greater_equal_fail_test),
	CUTE_REF(check_dbl_var_greater_equal_fail_test),
	CUTE_REF(check_dbl_greater_equal_ref_fail_test),
	CUTE_REF(check_dbl_var_greater_equal_ref_fail_test),

	CUTE_REF(check_dbl_lower_pass_test),
	CUTE_REF(check_dbl_literal_lower_fail_test),
	CUTE_REF(check_dbl_var_lower_fail_test),
	CUTE_REF(check_dbl_lower_ref_fail_test),
	CUTE_REF(check_dbl_var_lower_ref_fail_test),

	CUTE_REF(check_dbl_lower_equal_pass_test),
	CUTE_REF(check_dbl_literal_lower_equal_fail_test),
	CUTE_REF(check_dbl_var_lower_equal_fail_test),
	CUTE_REF(check_dbl_lower_equal_ref_fail_test),
	CUTE_REF(check_dbl_var_lower_equal_ref_fail_test),

	CUTE_REF(check_dbl_in_range_pass_test),
	CUTE_REF(check_dbl_literal_in_range_fail_test),
	CUTE_REF(check_dbl_var_in_range_fail_test),
	CUTE_REF(check_dbl_in_range_ref_fail_test),
	CUTE_REF(check_dbl_var_in_range_ref_fail_test),

	CUTE_REF(check_dbl_not_in_range_pass_test),
	CUTE_REF(check_dbl_literal_not_in_range_fail_test),
	CUTE_REF(check_dbl_var_not_in_range_fail_test),
	CUTE_REF(check_dbl_not_in_range_ref_fail_test),
	CUTE_REF(check_dbl_var_not_in_range_ref_fail_test),

	CUTE_REF(check_dbl_in_set_pass_test),
	CUTE_REF(check_dbl_literal_in_set_fail_test),
	CUTE_REF(check_dbl_var_in_set_fail_test),
	CUTE_REF(check_dbl_in_set_ref_fail_test),
	CUTE_REF(check_dbl_var_in_set_ref_fail_test),

	CUTE_REF(check_dbl_not_in_set_pass_test),
	CUTE_REF(check_dbl_literal_not_in_set_fail_test),
	CUTE_REF(check_dbl_var_not_in_set_fail_test),
	CUTE_REF(check_dbl_not_in_set_ref_fail_test),
	CUTE_REF(check_dbl_var_not_in_set_ref_fail_test)
};

static CUTE_SUITE_DEFINE(check_dbl_suite,
                         check_dbl_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(check_dbl_suite, "CUTe", NULL)
