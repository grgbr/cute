#include "cute/cute.h"
#include "cute/expect.h"
#include <assert.h>

/*
 * Override glibc's assertion failure function to mock it with
 * cute_mock_assert().
 */
void  __cute_noreturn
__assert_fail(const char * expression,
              const char * file,
              unsigned int line,
              const char * function)
{
	cute_mock_assert(expression, file, line, function);
}

static void
expect_assert_raise(int parm)
{
	assert(parm != 0);
	assert(parm != 1);
	assert(parm != 2);
}

CUTE_TEST(expect_assert_none_pass_test)
{
	expect_assert_raise(-1);
}

CUTE_TEST(expect_assert_none_fail_test)
{
	cute_expect_assertion(expect_assert_raise(-1));
}

CUTE_TEST(expect_assert_catch0_fail_test)
{
	expect_assert_raise(0);
}

CUTE_TEST(expect_assert_catch0_pass_test)
{
	cute_expect_assertion(expect_assert_raise(0));
}

CUTE_TEST(expect_assert_catch1_fail_test)
{
	expect_assert_raise(1);
}

CUTE_TEST(expect_assert_catch1_pass_test)
{
	cute_expect_assertion(expect_assert_raise(1));
}

CUTE_GROUP(expect_assert_tests) = {
	CUTE_REF(expect_assert_none_pass_test),
	CUTE_REF(expect_assert_none_fail_test),
	CUTE_REF(expect_assert_catch0_fail_test),
	CUTE_REF(expect_assert_catch0_pass_test),
	CUTE_REF(expect_assert_catch1_fail_test),
	CUTE_REF(expect_assert_catch1_pass_test)
};

CUTE_SUITE(expect_assert_suite, expect_assert_tests);

CUTE_MAIN(expect_assert_suite, "CUTe", NULL)
