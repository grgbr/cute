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

static CUTE_TEST_DEFN(pass_1,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(pass_2,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(pass_3,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(pass_4,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(pass_5,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(skip_2,
                      skip,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(skip_3,
                      skip,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(skip_5,
                      skip,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(fail_3,
                      fail,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(root_pass,
                      pass,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(root_skip,
                      skip,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

static CUTE_TEST_DEFN(root_fail,
                      request_fail,
                      CUTE_NULL_SETUP,
                      CUTE_NULL_TEARDOWN,
                      CUTE_NONE_TMOUT);

CUTE_GROUP(pass_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(pass_2),
	CUTE_REF(pass_3),
	CUTE_REF(pass_4),
	CUTE_REF(pass_5)
};

static CUTE_SUITE_DEFN(pass_suite,
                       pass_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_GROUP(pass_skip_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(pass_2),
	CUTE_REF(skip_3),
	CUTE_REF(pass_4),
	CUTE_REF(skip_5)
};

static CUTE_SUITE_DEFN(pass_skip_suite,
                       pass_skip_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_GROUP(fail_suite_tests) = {
	CUTE_REF(pass_1),
	CUTE_REF(skip_2),
	CUTE_REF(fail_3),
	CUTE_REF(pass_4)
};

static CUTE_SUITE_DEFN(fail_suite,
                       fail_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_GROUP(root_suite_tests) = {
	CUTE_REF(pass_suite),
	CUTE_REF(pass_skip_suite),
	CUTE_REF(fail_suite),
	CUTE_REF(root_pass),
	CUTE_REF(root_skip),
	CUTE_REF(root_fail)
};

static CUTE_SUITE_DEFN(root_suite,
                       root_suite_tests,
                       CUTE_NULL_SETUP,
                       CUTE_NULL_TEARDOWN,
                       CUTE_NONE_TMOUT);

CUTE_MAIN(root_suite, "CUTe full sample", CUTE_VERSION_STRING)
