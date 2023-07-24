#include "cute/cute.h"
#include "cute/check.h"
#include "cute/expect.h"

static void
expect_str_value_callee(const char * str)
{
	cute_mock_str_parm(str);
}

static void
expect_str_value_caller(const char * str)
{
	expect_str_value_callee(str);
}

CUTE_TEST(expect_str_caller_fail_test)
{
	cute_expect_str_parm(nonexistent_callee, str, equal,  "Lorem");
	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_parm_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee,
	                      nonexistent_parm,
	                      equal,
	                      "Lorem");
	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_equal_pass_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, equal,  "Lorem");

	cute_expect_str_parm(expect_str_value_callee, str, equal,  ref);

	expect_str_value_caller("Lorem");

	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_null_equal_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, equal, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_equal_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, equal, "Lorem");
	expect_str_value_caller("ipsum");
}

CUTE_TEST(expect_str_var_equal_fail_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, equal, ref);

	expect_str_value_caller("ipsum");
}

CUTE_TEST(expect_str_unequal_pass_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, unequal,  "Lorem");

	cute_expect_str_parm(expect_str_value_callee, str, unequal,  ref);

	expect_str_value_caller("ipsum");

	expect_str_value_caller("ipsum");
}

CUTE_TEST(expect_str_null_unequal_pass_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, unequal, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_unequal_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, unequal, "Lorem");
	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_var_unequal_fail_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, unequal, ref);

	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_begin_pass_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, begin,  "Lorem");

	cute_expect_str_parm(expect_str_value_callee, str, begin,  ref);

	expect_str_value_caller("Lorem ipsum");

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_null_begin_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, begin, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_begin_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, begin, "ipsum");
	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_var_begin_fail_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, begin, ref);

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_not_begin_pass_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, not_begin,  "ipsum");

	cute_expect_str_parm(expect_str_value_callee, str, not_begin,  ref);

	expect_str_value_caller("Lorem ipsum");

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_null_not_begin_pass_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, not_begin, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_not_begin_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, not_begin, "Lorem");
	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_var_not_begin_fail_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, not_begin, ref);

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_end_pass_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, end,  "ipsum");

	cute_expect_str_parm(expect_str_value_callee, str, end,  ref);

	expect_str_value_caller("Lorem ipsum");

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_null_end_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, end, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_end_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, end, "dolor");
	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_var_end_fail_test)
{
	const char * ref = "dolor";

	cute_expect_str_parm(expect_str_value_callee, str, end, ref);

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_not_end_pass_test)
{
	const char * ref = "Lorem";

	cute_expect_str_parm(expect_str_value_callee, str, not_end,  "Lorem");

	cute_expect_str_parm(expect_str_value_callee, str, not_end,  ref);

	expect_str_value_caller("Lorem ipsum");

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_null_not_end_pass_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, not_end, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_not_end_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, not_end, "ipsum");
	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_var_not_end_fail_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, not_end, ref);

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_contain_pass_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, contain,  "ipsum");

	cute_expect_str_parm(expect_str_value_callee, str, contain,  ref);

	expect_str_value_caller("Lorem ipsum dolor sit amet.");

	expect_str_value_caller("Lorem ipsum dolor sit amet.");
}

CUTE_TEST(expect_str_null_contain_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, contain, "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_contain_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee, str, contain, "dolor");
	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_var_contain_fail_test)
{
	const char * ref = "dolor";

	cute_expect_str_parm(expect_str_value_callee, str, contain, ref);

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_not_contain_pass_test)
{
	const char * ref = "dolor";

	cute_expect_str_parm(expect_str_value_callee,
	                     str,
	                     not_contain,
	                     "dolor");

	cute_expect_str_parm(expect_str_value_callee, str, not_contain,  ref);

	expect_str_value_caller("Lorem ipsum");

	expect_str_value_caller("Lorem ipsum");
}

CUTE_TEST(expect_str_null_not_contain_pass_test)
{
	cute_expect_str_parm(expect_str_value_callee,
	                     str,
	                     not_contain,
	                     "Lorem");
	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_not_contain_fail_test)
{
	cute_expect_str_parm(expect_str_value_callee,
	                     str,
	                     not_contain,
	                     "ipsum");
	expect_str_value_caller("Lorem ipsum dolor sit amet.");
}

CUTE_TEST(expect_str_var_not_contain_fail_test)
{
	const char * ref = "ipsum";

	cute_expect_str_parm(expect_str_value_callee, str, not_contain, ref);

	expect_str_value_caller("Lorem ipsum dolor sit amet.");
}

CUTE_TEST(expect_str_in_set_pass_test)
{
	const struct cute_str_set set = CUTE_STR_SET("Lorem", "ipsum", "dolor");

	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_expect_str_set(expect_str_value_callee, str, in, set);

	expect_str_value_caller("Lorem");
	expect_str_value_caller("dolor");
}

CUTE_TEST(expect_str_null_in_set_fail_test)
{
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));

	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_in_set_fail_test)
{
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));

	expect_str_value_caller("Lorem");
	expect_str_value_caller("sit");
}

CUTE_TEST(expect_str_var_in_set_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_expect_str_set(expect_str_value_callee, str, in, set);
	cute_expect_str_set(expect_str_value_callee, str, in, set);
	cute_expect_str_set(expect_str_value_callee, str, in, set);

	expect_str_value_caller("Lorem");
	expect_str_value_caller(dolor);
	expect_str_value_caller("sit");
}

CUTE_TEST(expect_str_not_in_set_pass_test)
{
	const char *              amet = "amet";
	const struct cute_str_set set = CUTE_STR_SET("Lorem", "ipsum", "dolor");

	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    not_in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_expect_str_set(expect_str_value_callee, str, not_in, set);

	expect_str_value_caller("sit");
	expect_str_value_caller(amet);
}

CUTE_TEST(expect_str_null_not_in_set_pass_test)
{
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    not_in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));

	expect_str_value_caller(NULL);
}

CUTE_TEST(expect_str_literal_not_in_set_fail_test)
{
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    not_in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_expect_str_set(expect_str_value_callee,
	                    str,
	                    not_in,
	                    CUTE_STR_SET("Lorem", "ipsum", "dolor"));

	expect_str_value_caller("sit");
	expect_str_value_caller("Lorem");
}

CUTE_TEST(expect_str_var_not_in_set_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const char *              amet = "amet";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_expect_str_set(expect_str_value_callee, str, not_in, set);
	cute_expect_str_set(expect_str_value_callee, str, not_in, set);
	cute_expect_str_set(expect_str_value_callee, str, not_in, set);

	expect_str_value_caller("sit");
	expect_str_value_caller(amet);
	expect_str_value_caller("dolor");
}

static const char *
expect_str_retval_callee(void)
{
	return cute_mock_str_retval();
}

static void
expect_str_retval_caller(const char ** result, size_t size)
{
	unsigned int c;

	for (c = 0; c < size; c++)
		result[c] = expect_str_retval_callee();
}

CUTE_TEST(expect_str_retval_pass_test)
{
	const char * res[3] = { NULL, };

	cute_expect_str_retval(expect_str_retval_callee, "aaa");
	cute_expect_str_retval(expect_str_retval_callee, "bbb");
	cute_expect_str_retval(expect_str_retval_callee, "ccc");
	expect_str_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_str(res[0], equal, "aaa");
	cute_check_str(res[1], equal, "bbb");
	cute_check_str(res[2], equal, "ccc");
}

CUTE_TEST(expect_null_str_retval_pass_test)
{
	const char * res[1] = { (const char *)0xdeadbeef };

	cute_expect_str_retval(expect_str_retval_callee, NULL);
	expect_str_retval_caller(res, 1);

	cute_check_assert(res[0] == NULL);
}

CUTE_TEST(expect_str_retval_caller_fail_test)
{
	const char * res[3] = { NULL, };

	cute_expect_str_retval(expect_str_retval_callee, "aaa");
	cute_expect_str_retval(nonexistent_callee, "bbb");
	cute_expect_str_retval(expect_str_retval_callee, "ccc");
	expect_str_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_str(res[0], equal, "aaa");
	cute_check_str(res[1], equal, "bbb");
	cute_check_str(res[2], equal, "ccc");
}

CUTE_TEST(expect_xcess_str_retval_fail_test)
{
	const char * res[2] = { NULL, };

	cute_expect_str_retval(expect_str_retval_callee, "aaa");
	cute_expect_str_retval(expect_str_retval_callee, "bbb");
	cute_expect_str_retval(expect_str_retval_callee, "ccc");
	expect_str_retval_caller(res, sizeof(res) / sizeof(res[0]));

	cute_check_str(res[0], equal, "aaa");
	cute_check_str(res[1], equal, "bbb");
}

CUTE_TEST(expect_str_inval_retval_fail_test)
{
	const char * res[2] = { NULL, };

	cute_expect_str_parm(expect_str_retval_callee,
	                     str,
	                     equal,
	                     "");
	expect_str_retval_caller(res, sizeof(res) / sizeof(res[0]));
}

CUTE_GROUP(expect_str_tests) = {
	CUTE_REF(expect_str_caller_fail_test),
	CUTE_REF(expect_str_parm_fail_test),

	CUTE_REF(expect_str_equal_pass_test),
	CUTE_REF(expect_str_null_equal_fail_test),
	CUTE_REF(expect_str_literal_equal_fail_test),
	CUTE_REF(expect_str_var_equal_fail_test),

	CUTE_REF(expect_str_unequal_pass_test),
	CUTE_REF(expect_str_null_unequal_pass_test),
	CUTE_REF(expect_str_literal_unequal_fail_test),
	CUTE_REF(expect_str_var_unequal_fail_test),

	CUTE_REF(expect_str_begin_pass_test),
	CUTE_REF(expect_str_null_begin_fail_test),
	CUTE_REF(expect_str_literal_begin_fail_test),
	CUTE_REF(expect_str_var_begin_fail_test),

	CUTE_REF(expect_str_not_begin_pass_test),
	CUTE_REF(expect_str_null_not_begin_pass_test),
	CUTE_REF(expect_str_literal_not_begin_fail_test),
	CUTE_REF(expect_str_var_not_begin_fail_test),

	CUTE_REF(expect_str_end_pass_test),
	CUTE_REF(expect_str_null_end_fail_test),
	CUTE_REF(expect_str_literal_end_fail_test),
	CUTE_REF(expect_str_var_end_fail_test),

	CUTE_REF(expect_str_not_end_pass_test),
	CUTE_REF(expect_str_null_not_end_pass_test),
	CUTE_REF(expect_str_literal_not_end_fail_test),
	CUTE_REF(expect_str_var_not_end_fail_test),

	CUTE_REF(expect_str_contain_pass_test),
	CUTE_REF(expect_str_null_contain_fail_test),
	CUTE_REF(expect_str_literal_contain_fail_test),
	CUTE_REF(expect_str_var_contain_fail_test),

	CUTE_REF(expect_str_not_contain_pass_test),
	CUTE_REF(expect_str_null_not_contain_pass_test),
	CUTE_REF(expect_str_literal_not_contain_fail_test),
	CUTE_REF(expect_str_var_not_contain_fail_test),

	CUTE_REF(expect_str_in_set_pass_test),
	CUTE_REF(expect_str_null_in_set_fail_test),
	CUTE_REF(expect_str_literal_in_set_fail_test),
	CUTE_REF(expect_str_var_in_set_fail_test),

	CUTE_REF(expect_str_not_in_set_pass_test),
	CUTE_REF(expect_str_null_not_in_set_pass_test),
	CUTE_REF(expect_str_literal_not_in_set_fail_test),
	CUTE_REF(expect_str_var_not_in_set_fail_test),

	CUTE_REF(expect_str_retval_pass_test),
	CUTE_REF(expect_null_str_retval_pass_test),
	CUTE_REF(expect_str_retval_caller_fail_test),
	CUTE_REF(expect_xcess_str_retval_fail_test),
	CUTE_REF(expect_str_inval_retval_fail_test)
};

CUTE_SUITE(expect_str_suite, expect_str_tests);

CUTE_MAIN(expect_str_suite, "CUTe", NULL)
