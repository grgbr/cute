#include "cute/cute.h"
#include "cute/check.h"
#include "cute/expect.h"

static void
expect_flt_value_callee(float sfloat, double dfloat, long double ldfloat)
{
	cute_mock_flt_parm(sfloat);
	cute_mock_flt_parm(dfloat);
	cute_mock_flt_parm(ldfloat);
}

static void
expect_flt_value_caller(float sfloat, double dfloat, long double ldfloat)
{
	expect_flt_value_callee(sfloat, dfloat, ldfloat);
}

CUTE_TEST(expect_flt_caller_fail_test)
{
	cute_expect_flt_parm(nonexistent_callee, sfloat, equal,  0.0f);
	cute_expect_flt_parm(nonexistent_callee, dfloat, equal, -1.0);
	cute_expect_flt_parm(nonexistent_callee, ldfloat, equal,  2.0L);
	expect_flt_value_caller(0.0f, -1.0, 2.0L);
}

CUTE_TEST(expect_flt_parm_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee, sfloat, equal,  0.0f);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, equal, -1.0);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     nonexistent_parm,
	                     equal,
	                     2.0L);
	expect_flt_value_caller(0.0f, -1.0, 2.0L);
}

CUTE_TEST(expect_flt_equal_pass_test)
{
	float       sref = 0.05f;
	double      dref = -1.2345;
	long double ldref = 2.000327L;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, equal, 0.05f);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, equal, -1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     equal,
	                     2.000327L);

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, equal,  sref);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, equal, dref);
	cute_expect_flt_parm(expect_flt_value_callee, ldfloat, equal,  ldref);

	expect_flt_value_caller(0.05f, -1.2345, 2.000327L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_equal_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee, sfloat, equal, 1);
	expect_flt_value_caller(0.05f, -1, 2);
}

CUTE_TEST(expect_flt_var_equal_fail_test)
{
	float sref = -1.0f;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, equal, sref);
	expect_flt_value_caller(0.05f, -1.2345, 2.000327L);
}

CUTE_TEST(expect_flt_unequal_pass_test)
{
	float       sref = -10.5f + 1;
	double      dref = 20.5 - 1;
	long double ldref = -30.5L + 1;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, unequal, -1.0f);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, unequal, 1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     unequal,
	                     -2.000327L);

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, unequal, - sref);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, unequal, - dref);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     unequal,
	                     - ldref);

	expect_flt_value_caller(1.0f, -1.2345, 2.000327L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_unequal_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee, sfloat, unequal, 0.05f);
	expect_flt_value_caller(0.05f, -1, 2);
}

CUTE_TEST(expect_flt_var_unequal_fail_test)
{
	float sref = 0.05f;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, unequal, sref);
	expect_flt_value_caller(0.05f, -1, 2);
}

CUTE_TEST(expect_flt_greater_pass_test)
{
	float       sref = 10.5f;
	double      dref = 20.5;
	long double ldref = 30.5L;

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater,
	                     -1.2345f);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     greater,
	                     1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     greater,
	                     -2.000327L);

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater,
	                     sref - 1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     greater,
	                     dref - 1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     greater,
	                     ldref - 1);

	expect_flt_value_caller(0.05f, 2.0, -1.2345L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_greater_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee, sfloat, greater, 0.05f);
	expect_flt_value_caller(-1.2345f, -1, 2L);
}

CUTE_TEST(expect_flt_var_greater_fail_test)
{
	float sref = 0.05f;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, greater, sref);
	expect_flt_value_caller(-1.0f, -1.0, 2L);
}

CUTE_TEST(expect_flt_greater_equal_pass_test)
{
	float       sref = 10.5f;
	double      dref = 20.5;
	long double ldref = 30.5L;

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater_equal,
	                     -1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     greater_equal,
	                     1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     greater_equal,
	                     -2);

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater_equal,
	                     sref);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     greater_equal,
	                     dref - 1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     greater_equal,
	                     ldref);

	expect_flt_value_caller(0.05f, 1.2345, -1.2345L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_greater_equal_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater_equal,
	                     0.05f);
	expect_flt_value_caller(-1.0f, -1.0, 2L);
}

CUTE_TEST(expect_flt_var_greater_equal_fail_test)
{
	float sref = 0.05f;

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     greater,
	                     sref);
	expect_flt_value_caller(-1.0f, -1.0, 2.0L);
}

CUTE_TEST(expect_flt_lower_pass_test)
{
	float       sref = -10.5f;
	double      dref = -20.5;
	long double ldref = -30.5L;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, lower,  -1.2345f);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, lower, 1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     lower,
	                     -2.000327L);

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, lower,  sref + 1);
	cute_expect_flt_parm(expect_flt_value_callee, dfloat, lower, dref + 1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     lower,
	                     ldref + 1);

	expect_flt_value_caller(-2.0f, 0.05, -3.0L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_lower_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee, sfloat, lower, 0.05f);
	expect_flt_value_caller(1.0f, -1.0, 2L);
}

CUTE_TEST(expect_flt_var_lower_fail_test)
{
	float sref = 0.05f;

	cute_expect_flt_parm(expect_flt_value_callee, sfloat, lower, sref);
	expect_flt_value_caller(1.0f, -1.0, 2L);
}

CUTE_TEST(expect_flt_lower_equal_pass_test)
{
	float       sref = -10.5f;
	double      dref = -20.5;
	long double ldref = -30.5L;

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     lower_equal,
	                     -1.2345f);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     lower_equal,
	                     1.2345);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     lower_equal,
	                     -2.000327L);

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     lower_equal,
	                     sref + 1);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     dfloat,
	                     lower_equal,
	                     dref);
	cute_expect_flt_parm(expect_flt_value_callee,
	                     ldfloat,
	                     lower_equal,
	                     ldref + 1);

	expect_flt_value_caller(-2.000327f, 1.2345, -3.0L);

	expect_flt_value_caller(sref, dref, ldref);
}

CUTE_TEST(expect_flt_literal_lower_equal_fail_test)
{
	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     lower_equal,
	                     0.05f);
	expect_flt_value_caller(1.2345f, -1.0, 2.0L);
}

CUTE_TEST(expect_flt_var_lower_equal_fail_test)
{
	float sref = 0.05f;

	cute_expect_flt_parm(expect_flt_value_callee,
	                     sfloat,
	                     lower_equal,
	                     sref);
	expect_flt_value_caller(1.2345f, -1.0, 2.0L);
}

CUTE_TEST(expect_flt_in_range_pass_test)
{
	const struct cute_flt_range range = CUTE_FLT_RANGE(-3.0f, 2.000327L);

	cute_expect_flt_range(expect_flt_value_callee, sfloat,
	                      in,
	                      CUTE_FLT_RANGE(-10.5f, 10.5f));
	cute_expect_flt_range(expect_flt_value_callee, dfloat,
	                      in,
	                      CUTE_FLT_RANGE(-10.5, 10.5));
	cute_expect_flt_range(expect_flt_value_callee, ldfloat,
	                      in,
	                      CUTE_FLT_RANGE(-10.5L, 10.5L));

	cute_expect_flt_range(expect_flt_value_callee, sfloat, in, range);
	cute_expect_flt_range(expect_flt_value_callee, dfloat, in, range);
	cute_expect_flt_range(expect_flt_value_callee, ldfloat, in, range);

	expect_flt_value_caller(0.05f, -1.2345, 2.000327L);

	expect_flt_value_caller(0.05f, -1.2345, 2.000327L);
}

CUTE_TEST(expect_flt_literal_in_range_fail_test)
{
	cute_expect_flt_range(expect_flt_value_callee, sfloat,
	                      in,
	                      CUTE_FLT_RANGE(1.2345f, 10.0f));
	expect_flt_value_caller(0.05f, -1.2345, 2.0L);
}

CUTE_TEST(expect_flt_var_in_range_fail_test)
{
	float                       rmin = 1.2345f;
	float                       rmax = 10.5f;
	const struct cute_flt_range range = CUTE_FLT_RANGE(rmin, rmax);

	cute_expect_flt_range(expect_flt_value_callee, sfloat, in, range);
	expect_flt_value_caller(0.05f, -1.2345, 2.0L);
}

CUTE_TEST(expect_flt_not_in_range_pass_test)
{
	double                      rmin = -3.0;
	const struct cute_flt_range range = CUTE_FLT_RANGE(rmin, 2.000327);

	cute_expect_flt_range(expect_flt_value_callee, sfloat,
	                      not_in,
	                      CUTE_FLT_RANGE(-10.5f, 10.5f));
	cute_expect_flt_range(expect_flt_value_callee, dfloat,
	                      not_in,
	                      CUTE_FLT_RANGE(-10.5, 10.5));
	cute_expect_flt_range(expect_flt_value_callee, ldfloat,
	                      not_in,
	                      CUTE_FLT_RANGE(-10.5L, 10.5L));

	cute_expect_flt_range(expect_flt_value_callee, sfloat, not_in, range);
	cute_expect_flt_range(expect_flt_value_callee, dfloat, not_in, range);
	cute_expect_flt_range(expect_flt_value_callee, ldfloat, not_in, range);

	expect_flt_value_caller(11.5f, -11.5, 20.5L);

	expect_flt_value_caller(11.5f, -11.5, 20.L);
}

CUTE_TEST(expect_flt_literal_not_in_range_fail_test)
{
	cute_expect_flt_range(expect_flt_value_callee, sfloat,
	                      not_in,
	                      CUTE_FLT_RANGE(0.05f, 10.5f));
	expect_flt_value_caller(0.05f, -1.2345, 2.0L);
}

CUTE_TEST(expect_flt_var_not_in_range_fail_test)
{
	long double                 rmin = 0.05L;
	long double                 rmax = 10.5L;
	const struct cute_flt_range range = CUTE_FLT_RANGE(rmin, rmax);

	cute_expect_flt_range(expect_flt_value_callee, sfloat, not_in, range);
	expect_flt_value_caller(10.5f, -1.2345, 2.0L);
}

CUTE_TEST(expect_flt_in_set_pass_test)
{
	const struct cute_flt_set set = CUTE_FLT_SET(-3.0f,
	                                             -1.2345,
	                                             1.2345L,
	                                             3.5f);

	cute_expect_flt_set(expect_flt_value_callee,
	                    sfloat,
	                    in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));
	cute_expect_flt_set(expect_flt_value_callee,
	                    dfloat,
	                    in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));
	cute_expect_flt_set(expect_flt_value_callee,
	                    ldfloat,
	                    in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));

	cute_expect_flt_set(expect_flt_value_callee, sfloat, in, set);
	cute_expect_flt_set(expect_flt_value_callee, dfloat, in, set);
	cute_expect_flt_set(expect_flt_value_callee, ldfloat, in, set);

	expect_flt_value_caller(-3.0f, -1.2345, 1.2345L);

	expect_flt_value_caller(-3.0f, -1.2345, 1.2345L);
}

CUTE_TEST(expect_flt_literal_in_set_fail_test)
{
	cute_expect_flt_set(expect_flt_value_callee,
	                    sfloat,
	                    in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));

	expect_flt_value_caller(0.05f, -1.2345, 3.5L);
}

CUTE_TEST(expect_flt_var_in_set_fail_test)
{
	float                     ref0 = -3.0f;
	long double               ref2 = 1.2345L;
	const struct cute_flt_set set = CUTE_FLT_SET(ref0, -1.2345, ref2, 3.0f);

	cute_expect_flt_set(expect_flt_value_callee, sfloat, in, set);

	expect_flt_value_caller(0.05f, -1.2345, 1.2345L);
}

CUTE_TEST(expect_flt_not_in_set_pass_test)
{
	const struct cute_flt_set set = CUTE_FLT_SET(-3.0f,
	                                             -1.2345,
	                                             1.2345L,
	                                             3.5f);

	cute_expect_flt_set(expect_flt_value_callee,
	                    sfloat,
	                    not_in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));
	cute_expect_flt_set(expect_flt_value_callee,
	                    dfloat,
	                    not_in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));
	cute_expect_flt_set(expect_flt_value_callee,
	                    ldfloat,
	                    not_in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));

	cute_expect_flt_set(expect_flt_value_callee, sfloat, not_in, set);
	cute_expect_flt_set(expect_flt_value_callee, dfloat, not_in, set);
	cute_expect_flt_set(expect_flt_value_callee, ldfloat, not_in, set);

	expect_flt_value_caller(-4.5f, -2.5, 2.0L);

	expect_flt_value_caller(-4.5f, -2.5, 2.0L);
}

CUTE_TEST(expect_flt_literal_not_in_set_fail_test)
{
	cute_expect_flt_set(expect_flt_value_callee,
	                    sfloat,
	                    not_in,
	                    CUTE_FLT_SET(-3.0f, -1.2345, 1.2345L, 3.5f));

	expect_flt_value_caller(-3.0f, -1, 1);
}

CUTE_TEST(expect_flt_var_not_in_set_fail_test)
{
	float                     ref0 = -3.0f;
	long double               ref2 = 1.2345L;
	const struct cute_flt_set set = CUTE_FLT_SET(ref0, -1.2345, ref2, 3.5f);

	cute_expect_flt_set(expect_flt_value_callee, sfloat, not_in, set);

	expect_flt_value_caller(-3.0f, -1, 1);
}

static float
expect_flt_retval_callee(void)
{
	return (float)cute_mock_flt_retval();
}

static void
expect_flt_retval_caller(float * result, size_t size)
{
	unsigned int c;

	for (c = 0; c < size; c++)
		result[c] = (float)expect_flt_retval_callee();
}

CUTE_TEST(expect_flt_retval_pass_test)
{
	float res[2] = { 0, };

	cute_expect_flt_retval(expect_flt_retval_callee, 1.2345f);
	cute_expect_flt_retval(expect_flt_retval_callee, 2.000327f);
	expect_flt_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_flt(res[0], equal, 1.2345f);
	cute_check_flt(res[1], equal, 2.000327f);
}

CUTE_TEST(expect_flt_retval_caller_fail_test)
{
	float res[2] = { 0, };

	cute_expect_flt_retval(nonexistent_callee, 1.2345f);
	cute_expect_flt_retval(nonexistent_callee, 2.000327f);
	expect_flt_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_flt(res[0], equal, 1.2345f);
	cute_check_flt(res[1], equal, 2.000327f);
}

CUTE_TEST(expect_xcess_flt_retval_fail_test)
{
	float res[1] = { 0, };

	cute_expect_flt_retval(expect_flt_retval_callee, 1.2345f);
	cute_expect_flt_retval(expect_flt_retval_callee, 2.000327f);
	expect_flt_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_flt(res[0], equal, 1.2345f);
}

CUTE_TEST(expect_flt_inval_retval_fail_test)
{
	float res[2] = { 0, };

	cute_expect_flt_parm(expect_flt_retval_callee,
	                     nonexistent_parm,
	                     equal,
	                     0.0f);
	expect_flt_retval_caller(res, sizeof(res) / sizeof(res[0]));
}

static CUTE_SUITE_DEFINE_TESTS(expect_flt_tests) = {
	CUTE_REF(expect_flt_caller_fail_test),
	CUTE_REF(expect_flt_parm_fail_test),

	CUTE_REF(expect_flt_equal_pass_test),
	CUTE_REF(expect_flt_literal_equal_fail_test),
	CUTE_REF(expect_flt_var_equal_fail_test),

	CUTE_REF(expect_flt_unequal_pass_test),
	CUTE_REF(expect_flt_literal_unequal_fail_test),
	CUTE_REF(expect_flt_var_unequal_fail_test),

	CUTE_REF(expect_flt_greater_pass_test),
	CUTE_REF(expect_flt_literal_greater_fail_test),
	CUTE_REF(expect_flt_var_greater_fail_test),

	CUTE_REF(expect_flt_greater_equal_pass_test),
	CUTE_REF(expect_flt_literal_greater_equal_fail_test),
	CUTE_REF(expect_flt_var_greater_equal_fail_test),

	CUTE_REF(expect_flt_lower_pass_test),
	CUTE_REF(expect_flt_literal_lower_fail_test),
	CUTE_REF(expect_flt_var_lower_fail_test),

	CUTE_REF(expect_flt_lower_equal_pass_test),
	CUTE_REF(expect_flt_literal_lower_equal_fail_test),
	CUTE_REF(expect_flt_var_lower_equal_fail_test),

	CUTE_REF(expect_flt_in_range_pass_test),
	CUTE_REF(expect_flt_literal_in_range_fail_test),
	CUTE_REF(expect_flt_var_in_range_fail_test),

	CUTE_REF(expect_flt_not_in_range_pass_test),
	CUTE_REF(expect_flt_literal_not_in_range_fail_test),
	CUTE_REF(expect_flt_var_not_in_range_fail_test),

	CUTE_REF(expect_flt_in_set_pass_test),
	CUTE_REF(expect_flt_literal_in_set_fail_test),
	CUTE_REF(expect_flt_var_in_set_fail_test),

	CUTE_REF(expect_flt_not_in_set_pass_test),
	CUTE_REF(expect_flt_literal_not_in_set_fail_test),
	CUTE_REF(expect_flt_var_not_in_set_fail_test),

	CUTE_REF(expect_flt_retval_pass_test),
	CUTE_REF(expect_flt_retval_caller_fail_test),
	CUTE_REF(expect_xcess_flt_retval_fail_test),
	CUTE_REF(expect_flt_inval_retval_fail_test)
};

static CUTE_SUITE_DEFINE(expect_flt_suite,
                         expect_flt_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(expect_flt_suite, "CUTe", NULL)
