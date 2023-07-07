#include <cute/expect.h>
#include <stdlib.h>

typedef void (callee_fn)(void);

static void
mocked_callee(void)
{
	cute_mock_call();
}

static void
caller(callee_fn * callee)
{
	callee();
}

CUTE_TEST(mock_call_pass_test)
{
	cute_expect_call(mocked_callee);
	caller(mocked_callee);
}

CUTE_TEST(mock_call_inval_fail_test)
{
	cute_expect_call(nonexistent_callee);
	caller(mocked_callee);
}

CUTE_TEST(mock_call_xcess_fail_test)
{
	cute_expect_call(mocked_callee);
	cute_expect_call(mocked_callee);
	caller(mocked_callee);
}

static CUTE_SUITE_DEFINE_TESTS(mock_suite_tests) = {
	CUTE_REF(mock_call_pass_test),
	CUTE_REF(mock_call_inval_fail_test),
	CUTE_REF(mock_call_xcess_fail_test),
};

static CUTE_SUITE_DEFINE(mock_suite,
                         mock_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(mock_suite, "CUTe mocking sample", CUTE_VERSION_STRING)
