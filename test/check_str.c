#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(check_str_equal_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem ipsum";

	cute_check_str("", equal, "");
	cute_check_str("Lorem ipsum", equal, "Lorem ipsum");
	cute_check_str(chk, equal, "Lorem ipsum");
	cute_check_str("Lorem ipsum", equal, ref);
	cute_check_str(chk, equal, ref);
}

CUTE_TEST(check_str_null_equal_fail_test)
{
	cute_check_str(NULL, equal, "");
}

CUTE_TEST(check_str_literal_equal_fail_test)
{
	cute_check_str("Lorem ipsum", equal, "");
}

CUTE_TEST(check_str_var_equal_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, equal, "");
}

CUTE_TEST(check_str_equal_ref_fail_test)
{
	const char * ref = "Lorem ipsum";

	cute_check_str("", equal, ref);
}

CUTE_TEST(check_str_var_equal_ref_fail_test)
{
	const char * chk = "Lorem";
	const char * ref = "Lorem ipsum";

	cute_check_str(chk, equal, ref);
}

CUTE_TEST(check_str_unequal_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "dolor sit amet.";

	cute_check_str("", unequal, "dolor sit amet.");
	cute_check_str("Lorem ipsum", unequal, "dolor sit amet.");
	cute_check_str(chk, unequal, "dolor sit amet.");
	cute_check_str("Lorem ipsum", unequal, ref);
	cute_check_str(chk, unequal, ref);
}

CUTE_TEST(check_str_null_unequal_pass_test)
{
	cute_check_str(NULL, unequal, "");
}

CUTE_TEST(check_str_literal_unequal_fail_test)
{
	cute_check_str("Lorem ipsum", unequal, "Lorem ipsum");
}

CUTE_TEST(check_str_var_unequal_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, unequal, "Lorem ipsum");
}

CUTE_TEST(check_str_unequal_ref_fail_test)
{
	const char * ref = "Lorem ipsum";

	cute_check_str("Lorem ipsum", unequal, ref);
}

CUTE_TEST(check_str_var_unequal_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem ipsum";

	cute_check_str(chk, unequal, ref);
}

CUTE_TEST(check_str_begin_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem";

	cute_check_str("", begin, "");
	cute_check_str("Lorem ipsum", begin, "Lorem ipsum");
	cute_check_str("Lorem ipsum", begin, "Lorem");
	cute_check_str(chk, begin, "Lorem");
	cute_check_str("Lorem ipsum", begin, ref);
	cute_check_str(chk, begin, ref);
}

CUTE_TEST(check_str_null_begin_fail_test)
{
	cute_check_str(NULL, begin, "");
}

CUTE_TEST(check_str_literal_begin_fail_test)
{
	cute_check_str("Lorem ipsum", begin, "ipsum");
}

CUTE_TEST(check_str_var_begin_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, begin, "ipsum");
}

CUTE_TEST(check_str_begin_ref_fail_test)
{
	const char * ref = "ipsum";

	cute_check_str("Lorem ipsum", begin, ref);
}

CUTE_TEST(check_str_var_begin_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "ipsum";

	cute_check_str(chk, begin, ref);
}

CUTE_TEST(check_str_not_begin_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "dolor sit amet.";

	cute_check_str("", not_begin, "dolor sit amet.");
	cute_check_str("Lorem ipsum", not_begin, "dolor sit amet.");
	cute_check_str(chk, not_begin, "dolor sit amet.");
	cute_check_str("Lorem ipsum", not_begin, ref);
	cute_check_str(chk, not_begin, ref);
}

CUTE_TEST(check_str_null_not_begin_pass_test)
{
	cute_check_str(NULL, not_begin, "");
}

CUTE_TEST(check_str_literal_not_begin_fail_test)
{
	cute_check_str("Lorem ipsum", not_begin, "Lorem");
}

CUTE_TEST(check_str_var_not_begin_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, not_begin, "Lorem");
}

CUTE_TEST(check_str_not_begin_ref_fail_test)
{
	const char * ref = "Lorem";

	cute_check_str("Lorem ipsum", not_begin, ref);
}

CUTE_TEST(check_str_var_not_begin_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem";

	cute_check_str(chk, not_begin, ref);
}

CUTE_TEST(check_str_end_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "ipsum";

	cute_check_str("", end, "");
	cute_check_str("Lorem ipsum", end, "Lorem ipsum");
	cute_check_str("Lorem ipsum", end, "ipsum");
	cute_check_str(chk, end, "ipsum");
	cute_check_str("Lorem ipsum", end, ref);
	cute_check_str(chk, end, ref);
}

CUTE_TEST(check_str_null_end_fail_test)
{
	cute_check_str(NULL, end, "");
}

CUTE_TEST(check_str_literal_end_fail_test)
{
	cute_check_str("Lorem ipsum", end, "Lorem");
}

CUTE_TEST(check_str_var_end_fail_test)
{
	const char * chk = "Lorem";

	cute_check_str(chk, end, "Lorem ipsum");
}

CUTE_TEST(check_str_end_ref_fail_test)
{
	const char * ref = "Lorem";

	cute_check_str("Lorem ipsum", end, ref);
}

CUTE_TEST(check_str_var_end_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem";

	cute_check_str(chk, end, ref);
}

CUTE_TEST(check_str_not_end_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem";

	cute_check_str("", not_end, "Lorem");
	cute_check_str("Lorem ipsum", not_end, "Lorem");
	cute_check_str(chk, not_end, "Lorem");
	cute_check_str("Lorem ipsum", not_end, ref);
	cute_check_str(chk, not_end, ref);
}

CUTE_TEST(check_str_null_not_end_pass_test)
{
	cute_check_str(NULL, not_end, "");
}

CUTE_TEST(check_str_literal_not_end_fail_test)
{
	cute_check_str("Lorem ipsum", not_end, "ipsum");
}

CUTE_TEST(check_str_var_not_end_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, not_end, "ipsum");
}

CUTE_TEST(check_str_not_end_ref_fail_test)
{
	const char * ref = "ipsum";

	cute_check_str("Lorem ipsum", not_end, ref);
}

CUTE_TEST(check_str_var_not_end_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "ipsum";

	cute_check_str(chk, not_end, ref);
}

CUTE_TEST(check_str_contain_pass_test)
{
	const char * chk = "Lorem ipsum dolor sit amet.";
	const char * ref = "dolor";

	cute_check_str("", contain, "");
	cute_check_str("Lorem ipsum dolor sit amet.", contain, "dolor");
	cute_check_str("Lorem ipsum", contain, "Lorem ipsum");
	cute_check_str(chk, contain, "dolor");
	cute_check_str("Lorem ipsum dolor sit amet.", contain, ref);
	cute_check_str(chk, contain, ref);
}

CUTE_TEST(check_str_null_contain_fail_test)
{
	cute_check_str(NULL, contain, "dolor");
}

CUTE_TEST(check_str_literal_contain_fail_test)
{
	cute_check_str("Lorem ipsum", contain, "dolor");
}

CUTE_TEST(check_str_var_contain_fail_test)
{
	const char * chk = "Lorem ipsum";

	cute_check_str(chk, contain, "dolor");
}

CUTE_TEST(check_str_contain_ref_fail_test)
{
	const char * ref = "dolor";

	cute_check_str("Lorem ipsum", contain, ref);
}

CUTE_TEST(check_str_var_contain_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem ipsum dolor sit amet.";

	cute_check_str(chk, contain, ref);
}

CUTE_TEST(check_str_not_contain_pass_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "dolor";

	cute_check_str("", not_contain, "dolor");
	cute_check_str("Lorem ipsum", not_contain, "dolor");
	cute_check_str(chk, not_contain, "dolor");
	cute_check_str("Lorem ipsum", not_contain, ref);
	cute_check_str(chk, not_contain, ref);
}

CUTE_TEST(check_str_null_not_contain_pass_test)
{
	cute_check_str(NULL, not_contain, "dolor");
}

CUTE_TEST(check_str_literal_not_contain_fail_test)
{
	cute_check_str("Lorem ipsum dolor sit amet.", not_contain, "dolor");
}

CUTE_TEST(check_str_var_not_contain_fail_test)
{
	const char * chk = "Lorem ipsum dolor sit amet.";

	cute_check_str(chk, not_contain, "dolor");
}

CUTE_TEST(check_str_not_contain_ref_fail_test)
{
	const char * ref = "dolor";

	cute_check_str("Lorem ipsum dolor sit amet.", not_contain, ref);
}

CUTE_TEST(check_str_var_not_contain_ref_fail_test)
{
	const char * chk = "Lorem ipsum";
	const char * ref = "Lorem ipsum";

	cute_check_str(chk, not_contain, ref);
}

CUTE_TEST(check_str_in_set_pass_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const struct cute_str_set set = CUTE_STR_SET("Lorem", "ipsum", "dolor");

	cute_check_str_set("dolor",
	                   in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_check_str_set(lorem, in, CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_check_str_set(dolor, in, CUTE_STR_SET(lorem, "ipsum", dolor));
	cute_check_str_set("Lorem", in, set);
	cute_check_str_set(dolor, in, set);
}

CUTE_TEST(check_str_null_in_set_fail_test)
{
	cute_check_str_set(NULL, in, CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_literal_in_set_fail_test)
{
	cute_check_str_set("sit", in, CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_var_in_set_fail_test)
{
	const char * sit = "sit";

	cute_check_str_set(sit, in, CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_in_set_ref_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_check_str_set("Lorem", in, set);
	cute_check_str_set("sit", in, set);
}

CUTE_TEST(check_str_var_in_set_ref_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const char *              sit = "sit";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_check_str_set(lorem, in, set);
	cute_check_str_set(sit, in, set);
}

CUTE_TEST(check_str_not_in_set_pass_test)
{
	const char *              sit = "sit";
	const struct cute_str_set set = CUTE_STR_SET("Lorem", "ipsum", "dolor");

	cute_check_str_set("sit",
	                   not_in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_check_str_set(sit,
	                   not_in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
	cute_check_str_set("sit", not_in, set);
	cute_check_str_set(sit, not_in, set);
}

CUTE_TEST(check_str_null_not_in_set_pass_test)
{
	cute_check_str_set(NULL,
	                   not_in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_literal_not_in_set_fail_test)
{
	cute_check_str_set("ipsum",
	                   not_in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_var_not_in_set_fail_test)
{
	const char * ipsum = "ipsum";

	cute_check_str_set(ipsum,
	                   not_in,
	                   CUTE_STR_SET("Lorem", "ipsum", "dolor"));
}

CUTE_TEST(check_str_not_in_set_ref_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_check_str_set("sit", not_in, set);
	cute_check_str_set("Lorem", not_in, set);
}

CUTE_TEST(check_str_var_not_in_set_ref_fail_test)
{
	const char *              lorem = "Lorem";
	const char *              dolor = "dolor";
	const char *              sit = "sit";
	const struct cute_str_set set = CUTE_STR_SET(lorem, "ipsum", dolor);

	cute_check_str_set(sit, not_in, set);
	cute_check_str_set(lorem, not_in, set);
}

static CUTE_SUITE_DEFINE_TESTS(check_str_tests) = {
	CUTE_REF(check_str_equal_pass_test),
	CUTE_REF(check_str_null_equal_fail_test),
	CUTE_REF(check_str_literal_equal_fail_test),
	CUTE_REF(check_str_var_equal_fail_test),
	CUTE_REF(check_str_equal_ref_fail_test),
	CUTE_REF(check_str_var_equal_ref_fail_test),

	CUTE_REF(check_str_unequal_pass_test),
	CUTE_REF(check_str_null_unequal_pass_test),
	CUTE_REF(check_str_literal_unequal_fail_test),
	CUTE_REF(check_str_var_unequal_fail_test),
	CUTE_REF(check_str_unequal_ref_fail_test),
	CUTE_REF(check_str_var_unequal_ref_fail_test),

	CUTE_REF(check_str_begin_pass_test),
	CUTE_REF(check_str_null_begin_fail_test),
	CUTE_REF(check_str_literal_begin_fail_test),
	CUTE_REF(check_str_var_begin_fail_test),
	CUTE_REF(check_str_begin_ref_fail_test),
	CUTE_REF(check_str_var_begin_ref_fail_test),

	CUTE_REF(check_str_not_begin_pass_test),
	CUTE_REF(check_str_null_not_begin_pass_test),
	CUTE_REF(check_str_literal_not_begin_fail_test),
	CUTE_REF(check_str_var_not_begin_fail_test),
	CUTE_REF(check_str_not_begin_ref_fail_test),
	CUTE_REF(check_str_var_not_begin_ref_fail_test),

	CUTE_REF(check_str_end_pass_test),
	CUTE_REF(check_str_null_end_fail_test),
	CUTE_REF(check_str_literal_end_fail_test),
	CUTE_REF(check_str_var_end_fail_test),
	CUTE_REF(check_str_end_ref_fail_test),
	CUTE_REF(check_str_var_end_ref_fail_test),

	CUTE_REF(check_str_not_end_pass_test),
	CUTE_REF(check_str_null_not_end_pass_test),
	CUTE_REF(check_str_literal_not_end_fail_test),
	CUTE_REF(check_str_var_not_end_fail_test),
	CUTE_REF(check_str_not_end_ref_fail_test),
	CUTE_REF(check_str_var_not_end_ref_fail_test),

	CUTE_REF(check_str_contain_pass_test),
	CUTE_REF(check_str_null_contain_fail_test),
	CUTE_REF(check_str_literal_contain_fail_test),
	CUTE_REF(check_str_var_contain_fail_test),
	CUTE_REF(check_str_contain_ref_fail_test),
	CUTE_REF(check_str_var_contain_ref_fail_test),

	CUTE_REF(check_str_not_contain_pass_test),
	CUTE_REF(check_str_null_not_contain_pass_test),
	CUTE_REF(check_str_literal_not_contain_fail_test),
	CUTE_REF(check_str_var_not_contain_fail_test),
	CUTE_REF(check_str_not_contain_ref_fail_test),
	CUTE_REF(check_str_var_not_contain_ref_fail_test),

	CUTE_REF(check_str_in_set_pass_test),
	CUTE_REF(check_str_null_in_set_fail_test),
	CUTE_REF(check_str_literal_in_set_fail_test),
	CUTE_REF(check_str_var_in_set_fail_test),
	CUTE_REF(check_str_in_set_ref_fail_test),
	CUTE_REF(check_str_var_in_set_ref_fail_test),

	CUTE_REF(check_str_not_in_set_pass_test),
	CUTE_REF(check_str_null_not_in_set_pass_test),
	CUTE_REF(check_str_literal_not_in_set_fail_test),
	CUTE_REF(check_str_var_not_in_set_fail_test),
	CUTE_REF(check_str_not_in_set_ref_fail_test),
	CUTE_REF(check_str_var_not_in_set_ref_fail_test)
};

static CUTE_SUITE_DEFINE(check_str_suite,
                         check_str_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(check_str_suite, "CUTe", NULL)
