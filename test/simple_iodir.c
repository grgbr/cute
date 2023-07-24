#include "cute/cute.h"
#include "cute/check.h"
#include <stdio.h>

CUTE_TEST(simple_iodir_stdout_pass_test)
{
	printf("a simple stdout output.\n");
}

CUTE_TEST(simple_iodir_stdout_fail_test)
{
	printf("a simple stdout output.\n");
	cute_fail("stdout collection test");
}

CUTE_TEST(simple_iodir_stderr_pass_test)
{
	printf("a simple stderr output.\n");
}

CUTE_TEST(simple_iodir_stderr_fail_test)
{
	fprintf(stderr, "a simple stderr output.\n");
	cute_fail("stderr collection test");
}

CUTE_TEST(simple_iodir_stdio_pass_test)
{
	printf("a simple stderr output.\n");
	fprintf(stderr, "a simple stderr output.\n");
}

CUTE_TEST(simple_iodir_stdio_fail_test)
{
	printf("a simple stdout output.\n");
	fprintf(stderr, "a simple stderr output.\n");
	cute_fail("stdio collection test");
}

CUTE_TEST(simple_iodir_blank_line_pass_test)
{
	printf("\n");
	fprintf(stderr, "\n");
}

CUTE_TEST(simple_iodir_blank_line_fail_test)
{
	printf("\n");
	fprintf(stderr, "\n");
	cute_fail("blank line stdio collection test");
}

CUTE_TEST(simple_iodir_multi_blank_line_pass_test)
{
	printf("\n\n");
	fprintf(stderr, "\n\n");
}

CUTE_TEST(simple_iodir_multi_blank_line_fail_test)
{
	printf("\n\n");
	fprintf(stderr, "\n\n");
	cute_fail("multi blank line stdio collection test");
}

CUTE_TEST(simple_iodir_multi_line_pass_test)
{
	printf("\n\nsimple multiline\n\nstdout\noutput\n\n");
	fprintf(stderr, "\n\nsimple multiline\n\nstderr\noutput\n\n");
}

CUTE_TEST(simple_iodir_multi_line_fail_test)
{
	printf("\n\nsimple multiline\n\nstdout\noutput\n\n");
	fprintf(stderr, "\n\nsimple multiline\n\nstderr\noutput\n\n");
	cute_fail("empty line stdio collection test");
}

static CUTE_SUITE_DEFINE_TESTS(simple_iodir_tests) = {
	CUTE_REF(simple_iodir_stdout_pass_test),
	CUTE_REF(simple_iodir_stdout_fail_test),
	CUTE_REF(simple_iodir_stderr_pass_test),
	CUTE_REF(simple_iodir_stderr_fail_test),
	CUTE_REF(simple_iodir_stdio_pass_test),
	CUTE_REF(simple_iodir_stdio_fail_test),
	CUTE_REF(simple_iodir_blank_line_pass_test),
	CUTE_REF(simple_iodir_blank_line_fail_test),
	CUTE_REF(simple_iodir_multi_blank_line_pass_test),
	CUTE_REF(simple_iodir_multi_blank_line_fail_test),
	CUTE_REF(simple_iodir_multi_line_pass_test),
	CUTE_REF(simple_iodir_multi_line_fail_test)
};

static CUTE_SUITE_DEFINE(simple_iodir_suite,
                         simple_iodir_tests,
                         CUTE_NULL_SETUP,
                         CUTE_NULL_TEARDOWN,
                         CUTE_FOREVER_TMOUT);

CUTE_MAIN(simple_iodir_suite, "CUTe", NULL)
