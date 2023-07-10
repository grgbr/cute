#include "cute/expect.h"
#include <limits.h>

static void
expect_sint_value_callee(char byte, short hword, int word, long long dword)
{
	cute_mock_sint(byte);
	cute_mock_sint(hword);
	cute_mock_sint(word);
	cute_mock_sint(dword);
}

static void
expect_sint_value_caller(char byte, short hword, int word, long long dword)
{
	expect_sint_value_callee(byte, hword, word, dword);
}

CUTE_TEST(expect_sint_caller_fail_test)
{
	cute_expect_sint(nonexistent_callee, byte, equal,  0);
	cute_expect_sint(nonexistent_callee, hword, equal, -1);
	cute_expect_sint(nonexistent_callee, word, equal,  2);
	cute_expect_sint(nonexistent_callee, dword, equal, -3);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_parm_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, equal,  0);
	cute_expect_sint(expect_sint_value_callee, hword, equal, -1);
	cute_expect_sint(expect_sint_value_callee, nonexistent_parm, equal,  2);
	cute_expect_sint(expect_sint_value_callee, dword, equal, -3);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_equal_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MAX;
	int       wref = INT_MIN;
	long long dref = LLONG_MAX;

	cute_expect_sint(expect_sint_value_callee, byte, equal,  0);
	cute_expect_sint(expect_sint_value_callee, hword, equal, -1);
	cute_expect_sint(expect_sint_value_callee, word, equal,  2);
	cute_expect_sint(expect_sint_value_callee, dword, equal, -3);

	cute_expect_sint(expect_sint_value_callee, byte, equal,  bref);
	cute_expect_sint(expect_sint_value_callee, hword, equal, href);
	cute_expect_sint(expect_sint_value_callee, word, equal,  wref);
	cute_expect_sint(expect_sint_value_callee, dword, equal, dref);

	expect_sint_value_caller(0, -1, 2, -3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_equal_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, equal, 1);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_equal_fail_test)
{
	char bref = -1;

	cute_expect_sint(expect_sint_value_callee, byte, equal, bref);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_unequal_pass_test)
{
	char      bref = SCHAR_MIN + 1;
	short     href = SHRT_MAX - 1;
	int       wref = INT_MIN + 1;
	long long dref = LLONG_MAX - 1;

	cute_expect_sint(expect_sint_value_callee, byte, unequal,  -1);
	cute_expect_sint(expect_sint_value_callee, hword, unequal, 1);
	cute_expect_sint(expect_sint_value_callee, word, unequal,  -2);
	cute_expect_sint(expect_sint_value_callee, dword, unequal, 3);

	cute_expect_sint(expect_sint_value_callee, byte, unequal,  - bref);
	cute_expect_sint(expect_sint_value_callee, hword, unequal, - href);
	cute_expect_sint(expect_sint_value_callee, word, unequal,  - wref);
	cute_expect_sint(expect_sint_value_callee, dword, unequal, - dref);

	expect_sint_value_caller(1, -1, 2, -3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_unequal_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, unequal, 0);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_unequal_fail_test)
{
	char bref = 0;

	cute_expect_sint(expect_sint_value_callee, byte, unequal, bref);
	expect_sint_value_caller(0, -1, 2, -3);
}

CUTE_TEST(expect_sint_greater_pass_test)
{
	char      bref = SCHAR_MAX;
	short     href = SHRT_MAX;
	int       wref = INT_MAX;
	long long dref = LLONG_MAX;

	cute_expect_sint(expect_sint_value_callee, byte, greater,  -1);
	cute_expect_sint(expect_sint_value_callee, hword, greater, 1);
	cute_expect_sint(expect_sint_value_callee, word, greater,  -2);
	cute_expect_sint(expect_sint_value_callee, dword, greater, 3);

	cute_expect_sint(expect_sint_value_callee, byte, greater,  bref - 1);
	cute_expect_sint(expect_sint_value_callee, hword, greater, href - 1);
	cute_expect_sint(expect_sint_value_callee, word, greater,  wref - 1);
	cute_expect_sint(expect_sint_value_callee, dword, greater, dref - 1);

	expect_sint_value_caller(0, 2, -1, 4);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_greater_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, greater, 0);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_greater_fail_test)
{
	char bref = 0;

	cute_expect_sint(expect_sint_value_callee, byte, greater, bref);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_greater_equal_pass_test)
{
	char      bref = SCHAR_MAX;
	short     href = SHRT_MAX;
	int       wref = INT_MAX;
	long long dref = LLONG_MAX;

	cute_expect_sint(expect_sint_value_callee, byte, greater_equal,  -1);
	cute_expect_sint(expect_sint_value_callee, hword, greater_equal, 1);
	cute_expect_sint(expect_sint_value_callee, word, greater_equal,  -2);
	cute_expect_sint(expect_sint_value_callee, dword, greater_equal, 3);

	cute_expect_sint(expect_sint_value_callee, byte, greater_equal,  bref);
	cute_expect_sint(expect_sint_value_callee, hword, greater_equal, href - 1);
	cute_expect_sint(expect_sint_value_callee, word, greater_equal,  wref);
	cute_expect_sint(expect_sint_value_callee, dword, greater_equal, dref - 1);

	expect_sint_value_caller(0, 1, -1, 3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_greater_equal_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, greater_equal, 0);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_greater_equal_fail_test)
{
	char bref = 0;

	cute_expect_sint(expect_sint_value_callee, byte, greater, bref);
	expect_sint_value_caller(-1, -1, 2, -3);
}

CUTE_TEST(expect_sint_lower_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MIN;
	int       wref = INT_MIN;
	long long dref = LLONG_MIN;

	cute_expect_sint(expect_sint_value_callee, byte, lower,  -1);
	cute_expect_sint(expect_sint_value_callee, hword, lower, 1);
	cute_expect_sint(expect_sint_value_callee, word, lower,  -2);
	cute_expect_sint(expect_sint_value_callee, dword, lower, 3);

	cute_expect_sint(expect_sint_value_callee, byte, lower,  bref + 1);
	cute_expect_sint(expect_sint_value_callee, hword, lower, href + 1);
	cute_expect_sint(expect_sint_value_callee, word, lower,  wref + 1);
	cute_expect_sint(expect_sint_value_callee, dword, lower, dref + 1);

	expect_sint_value_caller(-2, 0, -3, 2);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_lower_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, lower, 0);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_lower_fail_test)
{
	char bref = 0;

	cute_expect_sint(expect_sint_value_callee, byte, lower, bref);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_lower_equal_pass_test)
{
	char      bref = SCHAR_MIN;
	short     href = SHRT_MIN;
	int       wref = INT_MIN;
	long long dref = LLONG_MIN;

	cute_expect_sint(expect_sint_value_callee, byte, lower_equal,  -1);
	cute_expect_sint(expect_sint_value_callee, hword, lower_equal, 1);
	cute_expect_sint(expect_sint_value_callee, word, lower_equal,  -2);
	cute_expect_sint(expect_sint_value_callee, dword, lower_equal, 3);

	cute_expect_sint(expect_sint_value_callee, byte, lower_equal,  bref + 1);
	cute_expect_sint(expect_sint_value_callee, hword, lower_equal, href);
	cute_expect_sint(expect_sint_value_callee, word, lower_equal,  wref + 1);
	cute_expect_sint(expect_sint_value_callee, dword, lower_equal, dref);

	expect_sint_value_caller(-2, 1, -3, 3);

	expect_sint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_sint_literal_lower_equal_fail_test)
{
	cute_expect_sint(expect_sint_value_callee, byte, lower_equal, 0);
	expect_sint_value_caller(1, -1, 2, -3);
}

CUTE_TEST(expect_sint_var_lower_equal_fail_test)
{
	char bref = 0;

	cute_expect_sint(expect_sint_value_callee, byte, lower_equal, bref);
	expect_sint_value_caller(1, -1, 2, -3);
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
};

static CUTE_SUITE_DEFINE(expect_sint_suite,
                         expect_sint_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(expect_sint_suite, "CUTe", NULL)
