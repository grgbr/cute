#include <cute/cute.h>
#include <stdlib.h>

static void
test_skipped(void)
{
	cute_skip();
}

static CUTE_TEST_DEFINE(skipped,
                        test_skipped,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static void
test_failed(void)
{
	cute_fail();
}

static CUTE_TEST_DEFINE(failed,
                        test_failed,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static void
test_failed_ensure(void)
{
	cute_ensure(0 == 0);
	cute_ensure(0 == 1);
	cute_ensure(1 == 1);
}

static CUTE_TEST_DEFINE(failed_ensure,
                        test_failed_ensure,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);

static void
test_passed(void)
{
	cute_ensure(0 == 0);
}

static CUTE_TEST_DEFINE(passed,
                        test_passed,
                        CUTE_NULL_SETUP,
                        CUTE_NULL_TEARDOWN,
                        CUTE_FOREVER_TMOUT);


static CUTE_SUITE_DEFINE_TESTS(hello_suite_tests) = {
	CUTE_REF(skipped),
	CUTE_REF(failed),
	CUTE_REF(failed_ensure),
	CUTE_REF(passed)
};

static CUTE_SUITE_DEFINE(hello_suite,
                         hello_suite_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(hello_suite, "CUTe hello sample")
