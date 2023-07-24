#include "cute/cute.h"
#include "cute/check.h"
#include <string.h>

static const char         check_mem_str[] = "reference string";
static const unsigned int check_mem_uint[] = {
	0x00000000, 0xffffffff, 0x0000ffff, 0xffff0000,
	0x00ff00ff, 0xff00ff00, 0x0f0f0f0f, 0xf0f0f0f0,
	0x55555555, 0xaaaaaaaa, 0x5555aaaa, 0xaaaa5555,
	0x55aa55aa, 0xaa55aa55, 0x5a5a5a5a, 0xa5a5a5a5
};

CUTE_TEST(check_mem_equal_pass_test)
{
	unsigned char chk[sizeof(check_mem_uint)];

	memcpy(chk, check_mem_uint, sizeof(check_mem_uint));
	cute_check_mem(chk,
	               equal,
	               check_mem_uint,
	               sizeof(check_mem_uint));

	cute_check_mem("reference string",
	               equal,
	               check_mem_str,
	               sizeof(check_mem_str));
}

CUTE_TEST(check_mem_null_equal_fail_test)
{
	cute_check_mem(NULL,
	               equal,
	               check_mem_str,
	               sizeof(check_mem_str));
}

CUTE_TEST(check_mem_literal_equal_fail_test)
{
	cute_check_mem("test string",
	               equal,
	               check_mem_str,
	               sizeof(check_mem_str));
}

CUTE_TEST(check_mem_var_equal_fail_test)
{
	cute_check_mem(check_mem_str,
	               equal,
	               check_mem_uint,
	               sizeof(check_mem_uint));
}

CUTE_TEST(check_mem_unequal_pass_test)
{
	char chk[] = { 'a', 'b' };

	cute_check_mem(NULL,
	               unequal,
	               check_mem_str,
	               sizeof(check_mem_str));
	cute_check_mem("test string",
	               unequal,
	               check_mem_str,
	               sizeof(check_mem_str));
	cute_check_mem(chk,
	               unequal,
	               check_mem_uint,
	               sizeof(check_mem_uint));
}

CUTE_TEST(check_mem_literal_unequal_fail_test)
{
	cute_check_mem("reference string",
	               unequal,
	               check_mem_str,
	               sizeof(check_mem_str));
}

CUTE_TEST(check_mem_var_unequal_fail_test)
{
	unsigned char chk[sizeof(check_mem_uint)];

	memcpy(chk, check_mem_uint, sizeof(check_mem_uint));
	cute_check_mem(chk,
	               unequal,
	               check_mem_uint,
	               sizeof(check_mem_uint));
}

CUTE_GROUP(check_mem_tests) = {
	CUTE_REF(check_mem_equal_pass_test),
	CUTE_REF(check_mem_null_equal_fail_test),
	CUTE_REF(check_mem_literal_equal_fail_test),
	CUTE_REF(check_mem_var_equal_fail_test),

	CUTE_REF(check_mem_unequal_pass_test),
	CUTE_REF(check_mem_literal_unequal_fail_test),
	CUTE_REF(check_mem_var_unequal_fail_test),
};

CUTE_SUITE(check_mem_suite, check_mem_tests);

CUTE_MAIN(check_mem_suite, "CUTe", NULL)
