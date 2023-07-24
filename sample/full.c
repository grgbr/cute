#include <cute/cute.h>
#include <cute/check.h>
#include <stdlib.h>
#include <unistd.h>

static void
pass(void)
{
	usleep(250000);
	cute_check_assert(0 == 0);
}

static void
skip(void)
{
	usleep(250000);
	cute_skip(NULL);
}

static void
fail(void)
{
	usleep(250000);
	cute_check_assert(0 == 0);
	cute_check_assert(0 == 1);
	cute_check_assert(1 == 1);
}

static void
request_fail(void)
{
	usleep(250000);
	cute_fail("example of an explicit fail request");
}

static CUTE_TEST_DEFINE(pass_1,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(pass_2,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(pass_3,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(pass_4,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(pass_5,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(skip_2,
                        skip,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(skip_3,
                        skip,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(skip_5,
                        skip,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(fail_3,
                        fail,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(root_pass,
                        pass,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(root_skip,
                        skip,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_TEST_DEFINE(root_fail,
                        request_fail,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static CUTE_SUITE_DEFINE_TESTS(pass_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(pass_2),
	CUTE_REF(pass_3),
	CUTE_REF(pass_4),
	CUTE_REF(pass_5)
};

static CUTE_SUITE_DEFINE(pass_suite,
                         pass_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

static CUTE_SUITE_DEFINE_TESTS(pass_skip_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(pass_2),
	CUTE_REF(skip_3),
	CUTE_REF(pass_4),
	CUTE_REF(skip_5)
};

static CUTE_SUITE_DEFINE(pass_skip_suite,
                         pass_skip_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

static CUTE_SUITE_DEFINE_TESTS(fail_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(skip_2),
	CUTE_REF(fail_3),
	CUTE_REF(pass_4)
};

static CUTE_SUITE_DEFINE(fail_suite,
                         fail_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

static CUTE_SUITE_DEFINE_TESTS(root_suite_tests) = {
	CUTE_REF(pass_suite),
	CUTE_REF(pass_skip_suite),
	CUTE_REF(fail_suite),
	CUTE_REF(root_pass),
	CUTE_REF(root_skip),
	CUTE_REF(root_fail)
};

static CUTE_SUITE_DEFINE(root_suite,
                         root_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(root_suite, "CUTe full sample", CUTE_VERSION_STRING)
