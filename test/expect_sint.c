#include "cute/expect.h"
#include <limits.h>

static void
expect_sint_value_callee(char byte, short hword, int word, long long dword)
{
	cute_mock_sint_parm(byte);
	cute_mock_sint_parm(hword);
	cute_mock_sint_parm(word);
	cute_mock_sint_parm(dword);
}

static void
expect_sint_value_caller(char byte, short hword, int word, long long dword)
{
	expect_sint_value_callee(byte, hword, word, dword);
}

CUTE_TEST(expect_sint_caller_fail_test)
{
	cute_expect_sint_parm(nonexistent_callee, byte, equal,  0);
	cute_expect_sint_parm(nonexistent_callee, hword, equal, -1);
	cute_expect_sint_parm(nonexistent_callee, word, equal,  2);
	cute_expect_sint_parm(nonexistent_callee, dword, equal, -3);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_parm_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, equal,  0);
	cute_expect_sint_parm(expect_sint_value_callee, hword, equal, -1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      nonexistent_parm,
	                      equal,
	                      2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, equal, -3);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_equal_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MAX;
	int       wref = INT_MIN;
	long long dref = LLONG_MAX;

	cute_expect_sint_parm(expect_sint_value_callee, byte, equal,  0);
	cute_expect_sint_parm(expect_sint_value_callee, hword, equal, -1);
	cute_expect_sint_parm(expect_sint_value_callee, word, equal,  2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, equal, -3);

	cute_expect_sint_parm(expect_sint_value_callee, byte, equal,  bref);
	cute_expect_sint_parm(expect_sint_value_callee, hword, equal, href);
	cute_expect_sint_parm(expect_sint_value_callee, word, equal,  wref);
	cute_expect_sint_parm(expect_sint_value_callee, dword, equal, dref);

	expect_sint_value_caller(0, -1, 2, -3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_equal_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, equal, 1);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_equal_fail_test)
{
	char bref = -1;

	cute_expect_sint_parm(expect_sint_value_callee, byte, equal, bref);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_unequal_pass_test)
{
	char      bref = SCHAR_MIN + 1;
	short     href = SHRT_MAX - 1;
	int       wref = INT_MIN + 1;
	long long dref = LLONG_MAX - 1;

	cute_expect_sint_parm(expect_sint_value_callee, byte, unequal,  -1);
	cute_expect_sint_parm(expect_sint_value_callee, hword, unequal, 1);
	cute_expect_sint_parm(expect_sint_value_callee, word, unequal,  -2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, unequal, 3);

	cute_expect_sint_parm(expect_sint_value_callee, byte, unequal,  - bref);
	cute_expect_sint_parm(expect_sint_value_callee, hword, unequal, - href);
	cute_expect_sint_parm(expect_sint_value_callee, word, unequal,  - wref);
	cute_expect_sint_parm(expect_sint_value_callee, dword, unequal, - dref);

	expect_sint_value_caller(1, -1, 2, -3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_unequal_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, unequal, 0);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_unequal_fail_test)
{
	char bref = 0;

	cute_expect_sint_parm(expect_sint_value_callee, byte, unequal, bref);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_greater_pass_test)
{
	char      bref = SCHAR_MAX;
	short     href = SHRT_MAX;
	int       wref = INT_MAX;
	long long dref = LLONG_MAX;

	cute_expect_sint_parm(expect_sint_value_callee, byte, greater,  -1);
	cute_expect_sint_parm(expect_sint_value_callee, hword, greater, 1);
	cute_expect_sint_parm(expect_sint_value_callee, word, greater,  -2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, greater, 3);

	cute_expect_sint_parm(expect_sint_value_callee,
	                      byte,
	                      greater,
	                      bref - 1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      hword,
	                      greater,
	                      href - 1);
	cute_expect_sint_parm(expect_sint_value_callee,
			      word,
			      greater,
			      wref - 1);
	cute_expect_sint_parm(expect_sint_value_callee,
			      dword,
			      greater,
			      dref - 1);

	expect_sint_value_caller(0, 2, -1, 4);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_greater_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, greater, 0);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_greater_fail_test)
{
	char bref = 0;

	cute_expect_sint_parm(expect_sint_value_callee, byte, greater, bref);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_greater_equal_pass_test)
{
	char      bref = SCHAR_MAX;
	short     href = SHRT_MAX;
	int       wref = INT_MAX;
	long long dref = LLONG_MAX;

	cute_expect_sint_parm(expect_sint_value_callee,
	                      byte,
	                      greater_equal,
	                      -1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      hword,
	                      greater_equal,
	                      1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      word,
	                      greater_equal,
	                      -2);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      dword,
	                      greater_equal,
	                      3);

	cute_expect_sint_parm(expect_sint_value_callee,
	                      byte,
	                      greater_equal,
	                      bref);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      hword,
	                      greater_equal,
	                      href - 1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      word,
	                      greater_equal,
	                      wref);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      dword,
	                      greater_equal,
	                      dref - 1);

	expect_sint_value_caller(0, 1, -1, 3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_greater_equal_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, greater_equal, 0);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_greater_equal_fail_test)
{
	char bref = 0;

	cute_expect_sint_parm(expect_sint_value_callee, byte, greater, bref);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_lower_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MIN;
	int       wref = INT_MIN;
	long long dref = LLONG_MIN;

	cute_expect_sint_parm(expect_sint_value_callee, byte, lower,  -1);
	cute_expect_sint_parm(expect_sint_value_callee, hword, lower, 1);
	cute_expect_sint_parm(expect_sint_value_callee, word, lower,  -2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, lower, 3);

	cute_expect_sint_parm(expect_sint_value_callee, byte, lower,  bref + 1);
	cute_expect_sint_parm(expect_sint_value_callee, hword, lower, href + 1);
	cute_expect_sint_parm(expect_sint_value_callee, word, lower,  wref + 1);
	cute_expect_sint_parm(expect_sint_value_callee, dword, lower, dref + 1);

	expect_sint_value_caller(-2, 0, -3, 2);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_lower_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, lower, 0);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_lower_fail_test)
{
	char bref = 0;

	cute_expect_sint_parm(expect_sint_value_callee, byte, lower, bref);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_lower_equal_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MIN;
	int       wref = INT_MIN;
	long long dref = LLONG_MIN;

	cute_expect_sint_parm(expect_sint_value_callee, byte, lower_equal,  -1);
	cute_expect_sint_parm(expect_sint_value_callee, hword, lower_equal, 1);
	cute_expect_sint_parm(expect_sint_value_callee, word, lower_equal,  -2);
	cute_expect_sint_parm(expect_sint_value_callee, dword, lower_equal, 3);

	cute_expect_sint_parm(expect_sint_value_callee,
	                      byte,
	                      lower_equal,
	                      bref + 1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      hword,
	                      lower_equal,
	                      href);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      word,
	                      lower_equal,
	                      wref + 1);
	cute_expect_sint_parm(expect_sint_value_callee,
	                      dword,
	                      lower_equal,
	                      dref);

	expect_sint_value_caller(-2, 1, -3, 3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_lower_equal_fail_test)
{
	cute_expect_sint_parm(expect_sint_value_callee, byte, lower_equal, 0);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_lower_equal_fail_test)
{
	char bref = 0;

	cute_expect_sint_parm(expect_sint_value_callee,
	                      byte,
	                      lower_equal,
	                      bref);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_in_range_pass_test)
{
	const struct cute_sint_range range = CUTE_SINT_RANGE(-3, 2);

	cute_expect_sint_range(expect_sint_value_callee, byte,
	                       in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, hword,
	                       in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, word,
	                       in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, dword,
	                       in,
	                       CUTE_SINT_RANGE(-10, 10));

	cute_expect_sint_range(expect_sint_value_callee, byte,  in, range);
	cute_expect_sint_range(expect_sint_value_callee, hword, in, range);
	cute_expect_sint_range(expect_sint_value_callee, word,  in, range);
	cute_expect_sint_range(expect_sint_value_callee, dword, in, range);

	expect_sint_value_caller(0, -1, 2, -3);

	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_literal_in_range_fail_test)
{
	cute_expect_sint_range(expect_sint_value_callee, byte,
	                       in,
	                       CUTE_SINT_RANGE(1, 10));
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_in_range_fail_test)
{
	int                          rmin = 1;
	int                          rmax = 10;
	const struct cute_sint_range range = CUTE_SINT_RANGE(rmin, rmax);

	cute_expect_sint_range(expect_sint_value_callee, byte, in, range);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_not_in_range_pass_test)
{
	int                          rmin = -3;
	const struct cute_sint_range range = CUTE_SINT_RANGE(rmin, 2);

	cute_expect_sint_range(expect_sint_value_callee, byte,
	                       not_in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, hword,
	                       not_in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, word,
	                       not_in,
	                       CUTE_SINT_RANGE(-10, 10));
	cute_expect_sint_range(expect_sint_value_callee, dword,
	                       not_in,
	                       CUTE_SINT_RANGE(-10, 10));

	cute_expect_sint_range(expect_sint_value_callee, byte,  not_in, range);
	cute_expect_sint_range(expect_sint_value_callee, hword, not_in, range);
	cute_expect_sint_range(expect_sint_value_callee, word,  not_in, range);
	cute_expect_sint_range(expect_sint_value_callee, dword, not_in, range);

	expect_sint_value_caller(11, -11, 20, -20);

	expect_sint_value_caller(11, -11, 20, -20);
}

CUTE_TEST(expect_sint_literal_not_in_range_fail_test)
{
	cute_expect_sint_range(expect_sint_value_callee, byte,
	                       not_in,
	                       CUTE_SINT_RANGE(0, 10));
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_not_in_range_fail_test)
{
	int                          rmin = 0;
	int                          rmax = 10;
	const struct cute_sint_range range = CUTE_SINT_RANGE(rmin, rmax);

	cute_expect_sint_range(expect_sint_value_callee, byte, not_in, range);
	expect_sint_value_caller(10, -1, 2, -3);
}

CUTE_TEST(expect_sint_in_set_pass_test)
{
	const struct cute_sint_set set = CUTE_SINT_SET(-3, -1, 1, 3);

	cute_expect_sint_set(expect_sint_value_callee, byte,
	                     in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, hword,
	                     in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, word,
	                     in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, dword,
	                     in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));

	cute_expect_sint_set(expect_sint_value_callee, byte,  in, set);
	cute_expect_sint_set(expect_sint_value_callee, hword, in, set);
	cute_expect_sint_set(expect_sint_value_callee, word,  in, set);
	cute_expect_sint_set(expect_sint_value_callee, dword, in, set);

	expect_sint_value_caller(-3, -1, 1, -3);

	expect_sint_value_caller(-3, -1, 1, -3);
}

CUTE_TEST(expect_sint_literal_in_set_fail_test)
{
	cute_expect_sint_set(expect_sint_value_callee, byte,
	                     in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));

	expect_sint_value_caller(0, -1, 1, -3);
}

CUTE_TEST(expect_sint_var_in_set_fail_test)
{
	int                        ref0 = -3;
	int                        ref2 = 1;
	const struct cute_sint_set set = CUTE_SINT_SET(ref0, -1, ref2, 3);

	cute_expect_sint_set(expect_sint_value_callee, byte, in, set);

	expect_sint_value_caller(0, -1, 1, -3);
}

CUTE_TEST(expect_sint_not_in_set_pass_test)
{
	const struct cute_sint_set set = CUTE_SINT_SET(-3, -1, 1, 3);

	cute_expect_sint_set(expect_sint_value_callee, byte,
	                     not_in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, hword,
	                     not_in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, word,
	                     not_in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));
	cute_expect_sint_set(expect_sint_value_callee, dword,
	                     not_in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));

	cute_expect_sint_set(expect_sint_value_callee, byte,  not_in, set);
	cute_expect_sint_set(expect_sint_value_callee, hword, not_in, set);
	cute_expect_sint_set(expect_sint_value_callee, word,  not_in, set);
	cute_expect_sint_set(expect_sint_value_callee, dword, not_in, set);

	expect_sint_value_caller(-4, -2, 2, -4);

	expect_sint_value_caller(-4, -2, 2, -4);
}

CUTE_TEST(expect_sint_literal_not_in_set_fail_test)
{
	cute_expect_sint_set(expect_sint_value_callee, byte,
	                     not_in,
	                     CUTE_SINT_SET(-3, -1, 1, 3));

	expect_sint_value_caller(-3, -1, 1, -3);
}

CUTE_TEST(expect_sint_var_not_in_set_fail_test)
{
	int                        ref0 = -3;
	int                        ref2 = 1;
	const struct cute_sint_set set = CUTE_SINT_SET(ref0, -1, ref2, 3);

	cute_expect_sint_set(expect_sint_value_callee, byte, not_in, set);

	expect_sint_value_caller(-3, -1, 1, -3);
}

static int
expect_sint_retval_callee(void)
{
	return (int)cute_mock_sint_retval();
}

static void
expect_sint_retval_caller(char * result, size_t size)
{
	unsigned int c;

	for (c = 0; c < (size - 1); c++)
		result[c] = (char)expect_sint_retval_callee();
	
	result[c] = '\0';
}

CUTE_TEST(expect_sint_retval_pass_test)
{
	char res[3] = { 0, };

	cute_expect_sint_retval(expect_sint_retval_callee, 'a');
	cute_expect_sint_retval(expect_sint_retval_callee, 'b');
	expect_sint_retval_caller(res, sizeof(res));

	cute_check_sint(res[0], equal, 'a');
	cute_check_sint(res[1], equal, 'b');
	cute_check_sint(res[2], equal, '\0');
}

CUTE_TEST(expect_sint_retval_caller_fail_test)
{
	char res[3] = { 0, };

	cute_expect_sint_retval(nonexistent_callee, 'a');
	cute_expect_sint_retval(nonexistent_callee, 'b');
	expect_sint_retval_caller(res, sizeof(res));

	cute_check_sint(res[0], equal, 'a');
	cute_check_sint(res[1], equal, 'b');
	cute_check_sint(res[2], equal, '\0');
}

CUTE_TEST(expect_xcess_sint_retval_fail_test)
{
	char res[2] = { 0, };

	cute_expect_sint_retval(expect_sint_retval_callee, 'a');
	cute_expect_sint_retval(expect_sint_retval_callee, 'b');
	expect_sint_retval_caller(res, sizeof(res));

	cute_check_sint(res[0], equal, 'a');
	cute_check_sint(res[1], equal, '\0');
}

CUTE_TEST(expect_sint_inval_retval_fail_test)
{
	char res[2] = { 0, };

	cute_expect_sint_parm(expect_sint_retval_callee,
	                      nonexistent_parm,
	                      equal,
	                      0);
	expect_sint_retval_caller(res, sizeof(res));
}

static CUTE_SUITE_DEFINE_TESTS(expect_sint_tests) = {
	CUTE_REF(expect_sint_caller_fail_test),
	CUTE_REF(expect_sint_parm_fail_test),

	CUTE_REF(expect_sint_equal_pass_test),
	CUTE_REF(expect_sint_literal_equal_fail_test),
	CUTE_REF(expect_sint_var_equal_fail_test),

	CUTE_REF(expect_sint_unequal_pass_test),
	CUTE_REF(expect_sint_literal_unequal_fail_test),
	CUTE_REF(expect_sint_var_unequal_fail_test),

	CUTE_REF(expect_sint_greater_pass_test),
	CUTE_REF(expect_sint_literal_greater_fail_test),
	CUTE_REF(expect_sint_var_greater_fail_test),

	CUTE_REF(expect_sint_greater_equal_pass_test),
	CUTE_REF(expect_sint_literal_greater_equal_fail_test),
	CUTE_REF(expect_sint_var_greater_equal_fail_test),

	CUTE_REF(expect_sint_lower_pass_test),
	CUTE_REF(expect_sint_literal_lower_fail_test),
	CUTE_REF(expect_sint_var_lower_fail_test),

	CUTE_REF(expect_sint_lower_equal_pass_test),
	CUTE_REF(expect_sint_literal_lower_equal_fail_test),
	CUTE_REF(expect_sint_var_lower_equal_fail_test),

	CUTE_REF(expect_sint_in_range_pass_test),
	CUTE_REF(expect_sint_literal_in_range_fail_test),
	CUTE_REF(expect_sint_var_in_range_fail_test),

	CUTE_REF(expect_sint_not_in_range_pass_test),
	CUTE_REF(expect_sint_literal_not_in_range_fail_test),
	CUTE_REF(expect_sint_var_not_in_range_fail_test),

	CUTE_REF(expect_sint_in_set_pass_test),
	CUTE_REF(expect_sint_literal_in_set_fail_test),
	CUTE_REF(expect_sint_var_in_set_fail_test),

	CUTE_REF(expect_sint_not_in_set_pass_test),
	CUTE_REF(expect_sint_literal_not_in_set_fail_test),
	CUTE_REF(expect_sint_var_not_in_set_fail_test),

	CUTE_REF(expect_sint_retval_pass_test),
	CUTE_REF(expect_sint_retval_caller_fail_test),
	CUTE_REF(expect_xcess_sint_retval_fail_test),
	CUTE_REF(expect_sint_inval_retval_fail_test)
};

static CUTE_SUITE_DEFINE(expect_sint_suite,
                         expect_sint_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(expect_sint_suite, "CUTe", NULL)
