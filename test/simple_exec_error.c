#include "cute/cute.h"

CUTE_TEST(simple_exec_error_segv_test)
{
	int * var = NULL;

	*var = 2;
}

CUTE_TEST(simple_exec_error_ill_test)
{
	__builtin_trap();
}

/* Prevent GCC from optimizing out div by zero and silence related warning. */
#pragma GCC diagnostic push
#pragma GCC optimize ("-O0")
#pragma GCC diagnostic ignored "-Wdiv-by-zero"

CUTE_TEST(simple_exec_error_fpe_test)
{
	int result = result;

	result = result / 0;
}

#pragma GCC diagnostic pop

static CUTE_SUITE_DEFINE_TESTS(simple_exec_error_tests) = {
	CUTE_REF(simple_exec_error_segv_test),
	CUTE_REF(simple_exec_error_ill_test),
	CUTE_REF(simple_exec_error_fpe_test)
};

static CUTE_SUITE_DEFINE(simple_exec_error_suite,
                         simple_exec_error_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_exec_error_suite, "CUTe", NULL)
