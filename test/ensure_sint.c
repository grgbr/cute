#include "cute/ensure.h"

CUTE_TEST(ensure_sint_equal_pass_test)
{
	int chk = 2;
	int ref = 3;

	cute_ensure_sint(0, equal, 0);
	cute_ensure_sint(chk, equal, 2);
	cute_ensure_sint(3, equal, ref);
	chk = 3;
	cute_ensure_sint(chk, equal, ref);
}

CUTE_TEST(ensure_sint_literal_equal_fail_test)
{
	cute_ensure_sint(0, equal, 1);
}

CUTE_TEST(ensure_sint_var_equal_fail_test)
{
	int chk = 2;

	cute_ensure_sint(chk, equal, -1);
}

CUTE_TEST(ensure_sint_equal_ref_fail_test)
{
	int ref = 2;

	cute_ensure_sint(-1, equal, ref);
}

CUTE_TEST(ensure_sint_var_equal_ref_fail_test)
{
	int chk = 2;
	int ref = 3;

	cute_ensure_sint(chk, equal, ref);
}

CUTE_TEST(ensure_sint_unequal_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_ensure_sint(0, unequal, 1);
	cute_ensure_sint(chk, unequal, 0);
	cute_ensure_sint(0, unequal, ref);
	chk = 0;
	cute_ensure_sint(chk, unequal, ref);
}

CUTE_TEST(ensure_sint_literal_unequal_fail_test)
{
	cute_ensure_sint(1, unequal, 1);
}

CUTE_TEST(ensure_sint_var_unequal_fail_test)
{
	int chk = 2;

	cute_ensure_sint(chk, unequal, 2);
}

CUTE_TEST(ensure_sint_unequal_ref_fail_test)
{
	int ref = 2;

	cute_ensure_sint(2, unequal, ref);
}

CUTE_TEST(ensure_sint_var_unequal_ref_fail_test)
{
	int chk = 2;
	int ref = 2;

	cute_ensure_sint(chk, unequal, ref);
}

CUTE_TEST(ensure_sint_greater_pass_test)
{
	int chk = 1;
	int ref = -1;

	cute_ensure_sint(1, greater, 0);
	cute_ensure_sint(chk, greater, 0);
	cute_ensure_sint(0, greater, ref);
	cute_ensure_sint(chk, greater, ref);
}

CUTE_TEST(ensure_sint_literal_greater_fail_test)
{
	cute_ensure_sint(1, greater, 1);
}

CUTE_TEST(ensure_sint_var_greater_fail_test)
{
	int chk = -1;

	cute_ensure_sint(chk, greater, 1);
}

CUTE_TEST(ensure_sint_greater_ref_fail_test)
{
	int ref = 1;

	cute_ensure_sint(-1, greater, ref);
}

CUTE_TEST(ensure_sint_var_greater_ref_fail_test)
{
	int chk = -1;
	int ref = -1;

	cute_ensure_sint(chk, greater, ref);
}

CUTE_TEST(ensure_sint_greater_or_equal_pass_test)
{
	int chk = 1;
	int ref = -1;

	cute_ensure_sint(1, greater_or_equal, 0);
	cute_ensure_sint(0, greater_or_equal, 0);
	cute_ensure_sint(chk, greater_or_equal, 1);
	cute_ensure_sint(chk, greater_or_equal, 0);
	cute_ensure_sint(0, greater_or_equal, ref);
	cute_ensure_sint(-1, greater_or_equal, ref);
	cute_ensure_sint(chk, greater_or_equal, ref);
}

CUTE_TEST(ensure_sint_literal_greater_or_equal_fail_test)
{
	cute_ensure_sint(0, greater_or_equal, 1);
}

CUTE_TEST(ensure_sint_var_greater_or_equal_fail_test)
{
	int chk = -1;

	cute_ensure_sint(chk, greater_or_equal, 1);
}

CUTE_TEST(ensure_sint_greater_or_equal_ref_fail_test)
{
	int ref = 1;

	cute_ensure_sint(-1, greater_or_equal, ref);
}

CUTE_TEST(ensure_sint_var_greater_or_equal_ref_fail_test)
{
	int chk = -1;
	int ref = 0;

	cute_ensure_sint(chk, greater_or_equal, ref);
}

CUTE_TEST(ensure_sint_lower_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_ensure_sint(0, lower, 1);
	cute_ensure_sint(chk, lower, 0);
	cute_ensure_sint(0, lower, ref);
	cute_ensure_sint(chk, lower, ref);
}

CUTE_TEST(ensure_sint_literal_lower_fail_test)
{
	cute_ensure_sint(0, lower, 0);
}

CUTE_TEST(ensure_sint_var_lower_fail_test)
{
	int chk = 1;

	cute_ensure_sint(chk, lower, -1);
}

CUTE_TEST(ensure_sint_lower_ref_fail_test)
{
	int ref = -1;

	cute_ensure_sint(-1, lower, ref);
}

CUTE_TEST(ensure_sint_var_lower_ref_fail_test)
{
	int chk = 0;
	int ref = -1;

	cute_ensure_sint(chk, lower, ref);
}

CUTE_TEST(ensure_sint_lower_or_equal_pass_test)
{
	int chk = -1;
	int ref = 1;

	cute_ensure_sint(0, lower_or_equal, 1);
	cute_ensure_sint(0, lower_or_equal, 0);
	cute_ensure_sint(chk, lower_or_equal, 1);
	cute_ensure_sint(chk, lower_or_equal, -1);
	cute_ensure_sint(0, lower_or_equal, ref);
	cute_ensure_sint(-1, lower_or_equal, ref);
	cute_ensure_sint(chk, lower_or_equal, ref);
}

CUTE_TEST(ensure_sint_literal_lower_or_equal_fail_test)
{
	cute_ensure_sint(1, lower_or_equal, 0);
}

CUTE_TEST(ensure_sint_var_lower_or_equal_fail_test)
{
	int chk = 1;

	cute_ensure_sint(chk, lower_or_equal, -1);
}

CUTE_TEST(ensure_sint_lower_or_equal_ref_fail_test)
{
	int ref = -1;

	cute_ensure_sint(0, lower_or_equal, ref);
}

CUTE_TEST(ensure_sint_var_lower_or_equal_ref_fail_test)
{
	int chk = 0;
	int ref = -1;

	cute_ensure_sint(chk, lower_or_equal, ref);
}

static CUTE_SUITE_DEFINE_TESTS(ensure_sint_tests) = {
	CUTE_REF(ensure_sint_equal_pass_test),
	CUTE_REF(ensure_sint_literal_equal_fail_test),
	CUTE_REF(ensure_sint_var_equal_fail_test),
	CUTE_REF(ensure_sint_equal_ref_fail_test),
	CUTE_REF(ensure_sint_var_equal_ref_fail_test),

	CUTE_REF(ensure_sint_unequal_pass_test),
	CUTE_REF(ensure_sint_literal_unequal_fail_test),
	CUTE_REF(ensure_sint_var_unequal_fail_test),
	CUTE_REF(ensure_sint_unequal_ref_fail_test),
	CUTE_REF(ensure_sint_var_unequal_ref_fail_test),

	CUTE_REF(ensure_sint_greater_pass_test),
	CUTE_REF(ensure_sint_literal_greater_fail_test),
	CUTE_REF(ensure_sint_var_greater_fail_test),
	CUTE_REF(ensure_sint_greater_ref_fail_test),
	CUTE_REF(ensure_sint_var_greater_ref_fail_test),

	CUTE_REF(ensure_sint_greater_or_equal_pass_test),
	CUTE_REF(ensure_sint_literal_greater_or_equal_fail_test),
	CUTE_REF(ensure_sint_var_greater_or_equal_fail_test),
	CUTE_REF(ensure_sint_greater_or_equal_ref_fail_test),
	CUTE_REF(ensure_sint_var_greater_or_equal_ref_fail_test),

	CUTE_REF(ensure_sint_lower_pass_test),
	CUTE_REF(ensure_sint_literal_lower_fail_test),
	CUTE_REF(ensure_sint_var_lower_fail_test),
	CUTE_REF(ensure_sint_lower_ref_fail_test),
	CUTE_REF(ensure_sint_var_lower_ref_fail_test),

	CUTE_REF(ensure_sint_lower_or_equal_pass_test),
	CUTE_REF(ensure_sint_literal_lower_or_equal_fail_test),
	CUTE_REF(ensure_sint_var_lower_or_equal_fail_test),
	CUTE_REF(ensure_sint_lower_or_equal_ref_fail_test),
	CUTE_REF(ensure_sint_var_lower_or_equal_ref_fail_test),
};

static CUTE_SUITE_DEFINE(ensure_sint_suite,
                         ensure_sint_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(ensure_sint_suite, "CUTe", NULL)
