#include "cute/cute.h"
#include "cute/check.h"
#include "cute/expect.h"

static void
expect_ptr_value_callee(void * ptr)
{
	cute_mock_ptr_parm(ptr);
}

static void
expect_ptr_value_caller(void * ptr)
{
	expect_ptr_value_callee(ptr);
}

CUTE_TEST(expect_ptr_caller_fail_test)
{
	cute_expect_ptr_parm(nonexistent_callee, ptr, equal,  NULL);
	expect_ptr_value_caller(NULL);
}

CUTE_TEST(expect_ptr_parm_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     nonexistent_parm,
	                     equal,
	                     NULL);
	expect_ptr_value_caller(NULL);
}

CUTE_TEST(expect_ptr_equal_pass_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     equal,
	                     NULL);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     equal,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     equal,
	                     ref);


	expect_ptr_value_caller(NULL);
	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_literal_equal_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     equal,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_var_equal_fail_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     equal,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_unequal_pass_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     unequal,
	                     NULL);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     unequal,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     unequal,
	                     ref);


	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_literal_unequal_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     unequal,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_var_unequal_fail_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     unequal,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_greater_pass_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater,
	                     NULL);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater,
	                     ref);


	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_literal_greater_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_var_greater_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_greater_equal_pass_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater_equal,
	                     NULL);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater_equal,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater_equal,
	                     ref);


	expect_ptr_value_caller(NULL);
	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_literal_greater_equal_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater_equal,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_var_greater_equal_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     greater_equal,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_lower_pass_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower,
	                     ref);


	expect_ptr_value_caller(NULL);
	expect_ptr_value_caller((void *)0xdeadbabe);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_literal_lower_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_var_lower_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_lower_equal_pass_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower_equal,
	                     NULL);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower_equal,
	                     (void *)0xdeadbeef);
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower_equal,
	                     ref);


	expect_ptr_value_caller(NULL);
	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_literal_lower_equal_fail_test)
{
	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower_equal,
	                     (void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_var_lower_equal_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_expect_ptr_parm(expect_ptr_value_callee,
	                     ptr,
	                     lower_equal,
	                     ref);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_in_range_pass_test)
{
	const struct cute_ptr_range range = CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                                   (void *)0xdeadc0de);

	cute_expect_ptr_range(expect_ptr_value_callee,
	                      ptr,
	                      in,
	                      CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                     (void *)0xdeadc0de));
	cute_expect_ptr_range(expect_ptr_value_callee, ptr, in, range);

	expect_ptr_value_caller((void *)0xdeadbeef);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_literal_in_range_fail_test)
{
	cute_expect_ptr_range(expect_ptr_value_callee,
	                      ptr,
	                      in,
	                      CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                     (void *)0xdeadc0de));
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_var_in_range_fail_test)
{
	const void *                rmin = (void *)0xdeadbeef;
	const void *                rmax = (void *)0xdeadc0de;
	const struct cute_ptr_range range = CUTE_PTR_RANGE(rmin, rmax);

	cute_expect_ptr_range(expect_ptr_value_callee, ptr, in, range);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_not_in_range_pass_test)
{
	const struct cute_ptr_range range = CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                                   (void *)0xdeadc0de);

	cute_expect_ptr_range(expect_ptr_value_callee,
	                      ptr,
	                      not_in,
	                      CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                     (void *)0xdeadc0de));
	cute_expect_ptr_range(expect_ptr_value_callee, ptr, not_in, range);

	expect_ptr_value_caller((void *)0xdeadbabe);
	expect_ptr_value_caller((void *)0xdeadbabe);
}

CUTE_TEST(expect_ptr_literal_not_in_range_fail_test)
{
	cute_expect_ptr_range(expect_ptr_value_callee,
	                      ptr,
	                      not_in,
	                      CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                     (void *)0xdeadc0de));
	expect_ptr_value_caller((void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_var_not_in_range_fail_test)
{
	const void *                rmin = (void *)0xdeadbeef;
	const void *                rmax = (void *)0xdeadc0de;
	const struct cute_ptr_range range = CUTE_PTR_RANGE(rmin, rmax);

	cute_expect_ptr_range(expect_ptr_value_callee, ptr, not_in, range);
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_in_set_pass_test)
{
	void *                    chk = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             (void *)0xdeadc0de,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadc0de,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadc0de,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	cute_expect_ptr_set(expect_ptr_value_callee, ptr, in, set);

	expect_ptr_value_caller((void *)0xdeadcafe);
	expect_ptr_value_caller((void *)0xdeadce11);
	expect_ptr_value_caller(chk);
}

CUTE_TEST(expect_ptr_literal_in_set_fail_test)
{
	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadc0de,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	expect_ptr_value_caller((void *)0xdeaddddd);
}

CUTE_TEST(expect_ptr_var_in_set_fail_test)
{
	const void *              beef = (void *)0xdeadbeef;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             beef,
	                                             code,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_expect_ptr_set(expect_ptr_value_callee, ptr, in, set);

	expect_ptr_value_caller((void *)0xdeaddddd);
}

CUTE_TEST(expect_ptr_not_in_set_pass_test)
{
	void *                    chk = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    not_in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    not_in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	cute_expect_ptr_set(expect_ptr_value_callee, ptr, not_in, set);

	expect_ptr_value_caller((void *)0xdeadc0de);
	expect_ptr_value_caller((void *)0xdeaddddd);
	expect_ptr_value_caller(chk);
}

CUTE_TEST(expect_ptr_literal_not_in_set_fail_test)
{
	cute_expect_ptr_set(expect_ptr_value_callee,
	                    ptr,
	                    not_in,
	                    CUTE_PTR_SET((void *)0xdeadcafe,
	                                 (void *)0xdeadbeef,
	                                 (void *)0xdeadc0de,
	                                 (void *)0xdeadbabe,
	                                 (void *)0xdeadce11));
	expect_ptr_value_caller((void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_var_not_in_set_fail_test)
{
	const void *              beef = (void *)0xdeadbeef;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             beef,
	                                             code,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_expect_ptr_set(expect_ptr_value_callee, ptr, not_in, set);

	expect_ptr_value_caller((void *)0xdeadbeef);
}

static const void *
expect_ptr_retval_callee(void)
{
	return cute_mock_ptr_retval();
}

static void
expect_ptr_retval_caller(const void ** result, size_t size)
{
	unsigned int c;

	for (c = 0; c < size; c++)
		result[c] = expect_ptr_retval_callee();
}

CUTE_TEST(expect_ptr_retval_pass_test)
{
	const void * res[2] = { NULL, };

	cute_expect_ptr_retval(expect_ptr_retval_callee, (void *)0xdeadbeef);
	cute_expect_ptr_retval(expect_ptr_retval_callee, (void *)0xdeadc0de);
	expect_ptr_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_ptr(res[0], equal, (void *)0xdeadbeef);
	cute_check_ptr(res[1], equal, (void *)0xdeadc0de);
}

CUTE_TEST(expect_ptr_retval_caller_fail_test)
{
	const void * res[2] = { NULL, };

	cute_expect_ptr_retval(nonexistent_callee, (void *)0xdeadbeef);
	cute_expect_ptr_retval(nonexistent_callee, (void *)0xdeadc0de);
	expect_ptr_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_ptr(res[0], equal, (void *)0xdeadbeef);
	cute_check_ptr(res[1], equal, (void *)0xdeadc0de);
}

CUTE_TEST(expect_xcess_ptr_retval_fail_test)
{
	const void * res[1] = { NULL, };

	cute_expect_ptr_retval(expect_ptr_retval_callee, (void *)0xdeadbeef);
	cute_expect_ptr_retval(expect_ptr_retval_callee, (void *)0xdeadc0de);
	expect_ptr_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_ptr(res[0], equal, (void *)0xdeadbeef);
}

CUTE_TEST(expect_ptr_inval_retval_fail_test)
{
	const void * res[2] = { NULL, };

	cute_expect_ptr_parm(expect_ptr_retval_callee,
	                     nonexistent_parm,
	                     equal,
	                     (void *)0xdeadbeef);
	expect_ptr_retval_caller(res, sizeof(res) / sizeof(res[0]));
}

CUTE_GROUP(expect_ptr_tests) = {
	CUTE_REF(expect_ptr_caller_fail_test),
	CUTE_REF(expect_ptr_parm_fail_test),

	CUTE_REF(expect_ptr_equal_pass_test),
	CUTE_REF(expect_ptr_literal_equal_fail_test),
	CUTE_REF(expect_ptr_var_equal_fail_test),

	CUTE_REF(expect_ptr_unequal_pass_test),
	CUTE_REF(expect_ptr_literal_unequal_fail_test),
	CUTE_REF(expect_ptr_var_unequal_fail_test),

	CUTE_REF(expect_ptr_greater_pass_test),
	CUTE_REF(expect_ptr_literal_greater_fail_test),
	CUTE_REF(expect_ptr_var_greater_fail_test),

	CUTE_REF(expect_ptr_greater_equal_pass_test),
	CUTE_REF(expect_ptr_literal_greater_equal_fail_test),
	CUTE_REF(expect_ptr_var_greater_equal_fail_test),

	CUTE_REF(expect_ptr_lower_pass_test),
	CUTE_REF(expect_ptr_literal_lower_fail_test),
	CUTE_REF(expect_ptr_var_lower_fail_test),

	CUTE_REF(expect_ptr_lower_equal_pass_test),
	CUTE_REF(expect_ptr_literal_lower_equal_fail_test),
	CUTE_REF(expect_ptr_var_lower_equal_fail_test),

	CUTE_REF(expect_ptr_in_range_pass_test),
	CUTE_REF(expect_ptr_literal_in_range_fail_test),
	CUTE_REF(expect_ptr_var_in_range_fail_test),

	CUTE_REF(expect_ptr_not_in_range_pass_test),
	CUTE_REF(expect_ptr_literal_not_in_range_fail_test),
	CUTE_REF(expect_ptr_var_not_in_range_fail_test),

	CUTE_REF(expect_ptr_in_set_pass_test),
	CUTE_REF(expect_ptr_literal_in_set_fail_test),
	CUTE_REF(expect_ptr_var_in_set_fail_test),

	CUTE_REF(expect_ptr_not_in_set_pass_test),
	CUTE_REF(expect_ptr_literal_not_in_set_fail_test),
	CUTE_REF(expect_ptr_var_not_in_set_fail_test),

	CUTE_REF(expect_ptr_retval_pass_test),
	CUTE_REF(expect_ptr_retval_caller_fail_test),
	CUTE_REF(expect_xcess_ptr_retval_fail_test),
	CUTE_REF(expect_ptr_inval_retval_fail_test)
};

CUTE_SUITE(expect_ptr_suite, expect_ptr_tests);

CUTE_MAIN(expect_ptr_suite, "CUTe", NULL)
