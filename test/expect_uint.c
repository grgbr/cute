#include "cute/expect.h"
#include <limits.h>

static void
expect_uint_value_callee(unsigned char      byte,
                         unsigned short     hword,
                         unsigned int       word,
                         unsigned long long dword)
{
	cute_mock_uint_parm(byte);
	cute_mock_uint_parm(hword);
	cute_mock_uint_parm(word);
	cute_mock_uint_parm(dword);
}

static void
expect_uint_value_caller(unsigned char      byte,
                         unsigned short     hword,
                         unsigned int       word,
                         unsigned long long dword)
{
	expect_uint_value_callee(byte, hword, word, dword);
}

CUTE_TEST(expect_uint_caller_fail_test)
{
	cute_expect_uint_parm(nonexistent_callee, byte, equal,  0);
	cute_expect_uint_parm(nonexistent_callee, hword, equal, 1);
	cute_expect_uint_parm(nonexistent_callee, word, equal,  2);
	cute_expect_uint_parm(nonexistent_callee, dword, equal, 3);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_parm_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, equal,  0);
	cute_expect_uint_parm(expect_uint_value_callee, hword, equal, 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      nonexistent_parm,
	                      equal,
	                      2);
	cute_expect_uint_parm(expect_uint_value_callee, dword, equal, 3);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_equal_pass_test)
{
	unsigned char      bref = 0;
	unsigned short     href = USHRT_MAX;
	unsigned int       wref = 0;
	unsigned long long dref = ULLONG_MAX;

	cute_expect_uint_parm(expect_uint_value_callee, byte, equal,  0);
	cute_expect_uint_parm(expect_uint_value_callee, hword, equal, 1);
	cute_expect_uint_parm(expect_uint_value_callee, word, equal,  2);
	cute_expect_uint_parm(expect_uint_value_callee, dword, equal, 3);

	cute_expect_uint_parm(expect_uint_value_callee, byte, equal,  bref);
	cute_expect_uint_parm(expect_uint_value_callee, hword, equal, href);
	cute_expect_uint_parm(expect_uint_value_callee, word, equal,  wref);
	cute_expect_uint_parm(expect_uint_value_callee, dword, equal, dref);

	expect_uint_value_caller(0, 1, 2, 3);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_equal_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, equal, 1);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_equal_fail_test)
{
	unsigned char bref = 1;

	cute_expect_uint_parm(expect_uint_value_callee, byte, equal, bref);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_unequal_pass_test)
{
	unsigned char      bref = 1;
	unsigned short     href = USHRT_MAX - 1;
	unsigned int       wref = 1;
	unsigned long long dref = ULLONG_MAX - 1;

	cute_expect_uint_parm(expect_uint_value_callee, byte, unequal,  0);
	cute_expect_uint_parm(expect_uint_value_callee, hword, unequal, 1);
	cute_expect_uint_parm(expect_uint_value_callee, word, unequal,  2);
	cute_expect_uint_parm(expect_uint_value_callee, dword, unequal, 3);

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      unequal,
	                      (unsigned)-bref);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      unequal,
	                      (unsigned)-href);
	cute_expect_uint_parm(expect_uint_value_callee, word, unequal, -wref);
	cute_expect_uint_parm(expect_uint_value_callee, dword, unequal, -dref);

	expect_uint_value_caller(UCHAR_MAX, USHRT_MAX, UINT_MAX, ULLONG_MAX);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_unequal_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, unequal, 0);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_unequal_fail_test)
{
	unsigned char bref = 0;

	cute_expect_uint_parm(expect_uint_value_callee, byte, unequal, bref);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_greater_pass_test)
{
	unsigned char      bref = UCHAR_MAX;
	unsigned short     href = USHRT_MAX;
	unsigned int       wref = UINT_MAX;
	unsigned long long dref = ULLONG_MAX;

	cute_expect_uint_parm(expect_uint_value_callee, byte, greater,  0);
	cute_expect_uint_parm(expect_uint_value_callee, hword, greater, 1);
	cute_expect_uint_parm(expect_uint_value_callee, word, greater,  2);
	cute_expect_uint_parm(expect_uint_value_callee, dword, greater, 3);

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      greater,
	                      (unsigned)bref - 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      greater,
	                      (unsigned)href - 1);
	cute_expect_uint_parm(expect_uint_value_callee,
			      word,
			      greater,
			      wref - 1);
	cute_expect_uint_parm(expect_uint_value_callee,
			      dword,
			      greater,
			      dref - 1);

	expect_uint_value_caller(1, 2, 3, 4);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_greater_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, greater, 1);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_greater_fail_test)
{
	unsigned char bref = 1;

	cute_expect_uint_parm(expect_uint_value_callee, byte, greater, bref);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_greater_equal_pass_test)
{
	unsigned char      bref = UCHAR_MAX;
	unsigned short     href = USHRT_MAX;
	unsigned int       wref = UINT_MAX;
	unsigned long long dref = ULLONG_MAX;

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      greater_equal,
	                      0);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      greater_equal,
	                      1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      word,
	                      greater_equal,
	                      2);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      dword,
	                      greater_equal,
	                      3);

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      greater_equal,
	                      bref);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      greater_equal,
	                      (unsigned)href - 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      word,
	                      greater_equal,
	                      wref);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      dword,
	                      greater_equal,
	                      dref - 1);

	expect_uint_value_caller(1, 1, 3, 3);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_greater_equal_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, greater_equal, 1);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_greater_equal_fail_test)
{
	unsigned char bref = 1;

	cute_expect_uint_parm(expect_uint_value_callee, byte, greater, bref);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_lower_pass_test)
{
	unsigned char      bref = UCHAR_MAX / 2;
	unsigned short     href = USHRT_MAX / 2;
	unsigned int       wref = UINT_MAX / 2;
	unsigned long long dref = ULLONG_MAX / 2;

	cute_expect_uint_parm(expect_uint_value_callee, byte, lower,  1);
	cute_expect_uint_parm(expect_uint_value_callee, hword, lower, 2);
	cute_expect_uint_parm(expect_uint_value_callee, word, lower,  3);
	cute_expect_uint_parm(expect_uint_value_callee, dword, lower, 4);

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      lower,
	                      (unsigned)bref + 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      lower,
	                      (unsigned)href + 1);
	cute_expect_uint_parm(expect_uint_value_callee, word, lower,  wref + 1);
	cute_expect_uint_parm(expect_uint_value_callee, dword, lower, dref + 1);

	expect_uint_value_caller(0, 1, 2, 3);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_lower_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, lower, 0);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_lower_fail_test)
{
	unsigned char bref = 0;

	cute_expect_uint_parm(expect_uint_value_callee, byte, lower, bref);
	expect_uint_value_caller(1, 2, 3, 4);
}

CUTE_TEST(expect_uint_lower_equal_pass_test)
{
	unsigned char      bref = UCHAR_MAX / 2;
	unsigned short     href = USHRT_MAX / 2;
	unsigned int       wref = UINT_MAX / 2;
	unsigned long long dref = ULLONG_MAX / 2;

	cute_expect_uint_parm(expect_uint_value_callee, byte, lower_equal,  0);
	cute_expect_uint_parm(expect_uint_value_callee, hword, lower_equal, 1);
	cute_expect_uint_parm(expect_uint_value_callee, word, lower_equal,  2);
	cute_expect_uint_parm(expect_uint_value_callee, dword, lower_equal, 3);

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      lower_equal,
	                      (unsigned)bref + 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      hword,
	                      lower_equal,
	                      href);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      word,
	                      lower_equal,
	                      wref + 1);
	cute_expect_uint_parm(expect_uint_value_callee,
	                      dword,
	                      lower_equal,
	                      dref);

	expect_uint_value_caller(0, 0, 1, 2);

	expect_uint_value_caller(bref, href, wref, dref);
}

CUTE_TEST(expect_uint_literal_lower_equal_fail_test)
{
	cute_expect_uint_parm(expect_uint_value_callee, byte, lower_equal, 0);
	expect_uint_value_caller(1, 2, 3, 4);
}

CUTE_TEST(expect_uint_var_lower_equal_fail_test)
{
	unsigned char bref = 0;

	cute_expect_uint_parm(expect_uint_value_callee,
	                      byte,
	                      lower_equal,
	                      bref);
	expect_uint_value_caller(1, 2, 3, 4);
}

CUTE_TEST(expect_uint_in_range_pass_test)
{
	const struct cute_uint_range range = CUTE_UINT_RANGE(5, 10);

	cute_expect_uint_range(expect_uint_value_callee, byte,
	                       in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, hword,
	                       in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, word,
	                       in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, dword,
	                       in,
	                       CUTE_UINT_RANGE(0, 3));

	cute_expect_uint_range(expect_uint_value_callee, byte,  in, range);
	cute_expect_uint_range(expect_uint_value_callee, hword, in, range);
	cute_expect_uint_range(expect_uint_value_callee, word,  in, range);
	cute_expect_uint_range(expect_uint_value_callee, dword, in, range);

	expect_uint_value_caller(0, 1, 2, 3);

	expect_uint_value_caller(5, 7, 9, 10);
}

CUTE_TEST(expect_uint_literal_in_range_fail_test)
{
	cute_expect_uint_range(expect_uint_value_callee, byte,
	                       in,
	                       CUTE_UINT_RANGE(1, 10));
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_var_in_range_fail_test)
{
	unsigned int                 rmin = 1;
	unsigned int                 rmax = 10;
	const struct cute_uint_range range = CUTE_UINT_RANGE(rmin, rmax);

	cute_expect_uint_range(expect_uint_value_callee, byte, in, range);
	expect_uint_value_caller(0, 1, 2, 3);
}

CUTE_TEST(expect_uint_not_in_range_pass_test)
{
	unsigned int                 rmin = 5;
	const struct cute_uint_range range = CUTE_UINT_RANGE(rmin, 9);

	cute_expect_uint_range(expect_uint_value_callee, byte,
	                       not_in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, hword,
	                       not_in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, word,
	                       not_in,
	                       CUTE_UINT_RANGE(0, 3));
	cute_expect_uint_range(expect_uint_value_callee, dword,
	                       not_in,
	                       CUTE_UINT_RANGE(0, 3));

	cute_expect_uint_range(expect_uint_value_callee, byte,  not_in, range);
	cute_expect_uint_range(expect_uint_value_callee, hword, not_in, range);
	cute_expect_uint_range(expect_uint_value_callee, word,  not_in, range);
	cute_expect_uint_range(expect_uint_value_callee, dword, not_in, range);

	expect_uint_value_caller(5, 10, 15, 20);

	expect_uint_value_caller(0, 11, 15, 20);
}

CUTE_TEST(expect_uint_literal_not_in_range_fail_test)
{
	cute_expect_uint_range(expect_uint_value_callee, byte,
	                       not_in,
	                       CUTE_UINT_RANGE(5, 9));
	expect_uint_value_caller(5, 6, 8, 9);
}

CUTE_TEST(expect_uint_var_not_in_range_fail_test)
{
	unsigned int                 rmin = 5;
	unsigned int                 rmax = 9;
	const struct cute_uint_range range = CUTE_UINT_RANGE(rmin, rmax);

	cute_expect_uint_range(expect_uint_value_callee, byte, not_in, range);
	expect_uint_value_caller(5, 6, 8, 9);
}

CUTE_TEST(expect_uint_in_set_pass_test)
{
	const struct cute_uint_set set = CUTE_UINT_SET(1, 5, 10, 15);

	cute_expect_uint_set(expect_uint_value_callee, byte,
	                     in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, hword,
	                     in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, word,
	                     in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, dword,
	                     in,
	                     CUTE_UINT_SET(1, 5, 10, 15));

	cute_expect_uint_set(expect_uint_value_callee, byte,  in, set);
	cute_expect_uint_set(expect_uint_value_callee, hword, in, set);
	cute_expect_uint_set(expect_uint_value_callee, word,  in, set);
	cute_expect_uint_set(expect_uint_value_callee, dword, in, set);

	expect_uint_value_caller(1, 5, 10, 15);

	expect_uint_value_caller(1, 5, 10, 15);
}

CUTE_TEST(expect_uint_literal_in_set_fail_test)
{
	cute_expect_uint_set(expect_uint_value_callee, byte,
	                     in,
	                     CUTE_UINT_SET(1, 5, 10, 15));

	expect_uint_value_caller(0, 5, 10, 15);
}

CUTE_TEST(expect_uint_var_in_set_fail_test)
{
	unsigned int               ref0 = 1;
	unsigned int               ref2 = 10;
	const struct cute_uint_set set = CUTE_UINT_SET(ref0, 5, ref2, 15);

	cute_expect_uint_set(expect_uint_value_callee, byte, in, set);

	expect_uint_value_caller(0, 5, 10, 15);
}

CUTE_TEST(expect_uint_not_in_set_pass_test)
{
	const struct cute_uint_set set = CUTE_UINT_SET(1, 5, 10, 15);

	cute_expect_uint_set(expect_uint_value_callee, byte,
	                     not_in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, hword,
	                     not_in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, word,
	                     not_in,
	                     CUTE_UINT_SET(1, 5, 10, 15));
	cute_expect_uint_set(expect_uint_value_callee, dword,
	                     not_in,
	                     CUTE_UINT_SET(1, 5, 10, 15));

	cute_expect_uint_set(expect_uint_value_callee, byte,  not_in, set);
	cute_expect_uint_set(expect_uint_value_callee, hword, not_in, set);
	cute_expect_uint_set(expect_uint_value_callee, word,  not_in, set);
	cute_expect_uint_set(expect_uint_value_callee, dword, not_in, set);

	expect_uint_value_caller(2, 6, 11, 16);

	expect_uint_value_caller(2, 6, 11, 16);
}

CUTE_TEST(expect_uint_literal_not_in_set_fail_test)
{
	cute_expect_uint_set(expect_uint_value_callee, byte,
	                     not_in,
	                     CUTE_UINT_SET(1, 5, 10, 15));

	expect_uint_value_caller(1, 6, 11, 16);
}

CUTE_TEST(expect_uint_var_not_in_set_fail_test)
{
	unsigned int               ref0 = 1;
	unsigned int               ref2 = 10;
	const struct cute_uint_set set = CUTE_UINT_SET(ref0, 5, ref2, 15);

	cute_expect_uint_set(expect_uint_value_callee, byte, not_in, set);

	expect_uint_value_caller(1, 6, 11, 16);
}

static int
expect_uint_retval_callee(void)
{
	return (int)cute_mock_uint_retval();
}

static void
expect_uint_retval_caller(unsigned int * result, size_t size)
{
	unsigned int c;

	for (c = 0; c < size; c++)
		result[c] = (unsigned int)expect_uint_retval_callee();
}

CUTE_TEST(expect_uint_retval_pass_test)
{
	unsigned int res[2] = { 0, };

	cute_expect_uint_retval(expect_uint_retval_callee, 10);
	cute_expect_uint_retval(expect_uint_retval_callee, 20);
	expect_uint_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_uint(res[0], equal, 10);
	cute_check_uint(res[1], equal, 20);
}

CUTE_TEST(expect_uint_retval_caller_fail_test)
{
	unsigned int res[2] = { 0, };

	cute_expect_uint_retval(nonexistent_callee, 10);
	cute_expect_uint_retval(nonexistent_callee, 20);
	expect_uint_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_uint(res[0], equal, 10);
	cute_check_uint(res[1], equal, 20);
}

CUTE_TEST(expect_xcess_uint_retval_fail_test)
{
	unsigned int res[1] = { 0, };

	cute_expect_uint_retval(expect_uint_retval_callee, 10);
	cute_expect_uint_retval(expect_uint_retval_callee, 20);
	expect_uint_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_uint(res[0], equal, 10);
}

CUTE_TEST(expect_uint_inval_retval_fail_test)
{
	unsigned int res[2] = { 0, };

	cute_expect_uint_parm(expect_uint_retval_callee,
	                      nonexistent_parm,
	                      equal,
	                      0);
	expect_uint_retval_caller(res, sizeof(res) / sizeof(res[0]));
}

static CUTE_SUITE_DEFINE_TESTS(expect_uint_tests) = {
	CUTE_REF(expect_uint_caller_fail_test),
	CUTE_REF(expect_uint_parm_fail_test),

	CUTE_REF(expect_uint_equal_pass_test),
	CUTE_REF(expect_uint_literal_equal_fail_test),
	CUTE_REF(expect_uint_var_equal_fail_test),

	CUTE_REF(expect_uint_unequal_pass_test),
	CUTE_REF(expect_uint_literal_unequal_fail_test),
	CUTE_REF(expect_uint_var_unequal_fail_test),

	CUTE_REF(expect_uint_greater_pass_test),
	CUTE_REF(expect_uint_literal_greater_fail_test),
	CUTE_REF(expect_uint_var_greater_fail_test),

	CUTE_REF(expect_uint_greater_equal_pass_test),
	CUTE_REF(expect_uint_literal_greater_equal_fail_test),
	CUTE_REF(expect_uint_var_greater_equal_fail_test),

	CUTE_REF(expect_uint_lower_pass_test),
	CUTE_REF(expect_uint_literal_lower_fail_test),
	CUTE_REF(expect_uint_var_lower_fail_test),

	CUTE_REF(expect_uint_lower_equal_pass_test),
	CUTE_REF(expect_uint_literal_lower_equal_fail_test),
	CUTE_REF(expect_uint_var_lower_equal_fail_test),

	CUTE_REF(expect_uint_in_range_pass_test),
	CUTE_REF(expect_uint_literal_in_range_fail_test),
	CUTE_REF(expect_uint_var_in_range_fail_test),

	CUTE_REF(expect_uint_not_in_range_pass_test),
	CUTE_REF(expect_uint_literal_not_in_range_fail_test),
	CUTE_REF(expect_uint_var_not_in_range_fail_test),

	CUTE_REF(expect_uint_in_set_pass_test),
	CUTE_REF(expect_uint_literal_in_set_fail_test),
	CUTE_REF(expect_uint_var_in_set_fail_test),

	CUTE_REF(expect_uint_not_in_set_pass_test),
	CUTE_REF(expect_uint_literal_not_in_set_fail_test),
	CUTE_REF(expect_uint_var_not_in_set_fail_test),

	CUTE_REF(expect_uint_retval_pass_test),
	CUTE_REF(expect_uint_retval_caller_fail_test),
	CUTE_REF(expect_xcess_uint_retval_fail_test),
	CUTE_REF(expect_uint_inval_retval_fail_test)
};

static CUTE_SUITE_DEFINE(expect_uint_suite,
                         expect_uint_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(expect_uint_suite, "CUTe", NULL)
