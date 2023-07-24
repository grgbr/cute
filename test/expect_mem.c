#include "cute/cute.h"
#include "cute/expect.h"
#include <string.h>

static const char         expect_mem_str[] = "reference string";
static const unsigned int expect_mem_uint[] = {
	0x00000000, 0xffffffff, 0x0000ffff, 0xffff0000,
	0x00ff00ff, 0xff00ff00, 0x0f0f0f0f, 0xf0f0f0f0,
	0x55555555, 0xaaaaaaaa, 0x5555aaaa, 0xaaaa5555,
	0x55aa55aa, 0xaa55aa55, 0x5a5a5a5a, 0xa5a5a5a5
};

static void
expect_mem_value_callee(const void * ptr)
{
	cute_mock_mem_parm(ptr);
}

static void
expect_mem_value_caller(const void * ptr)
{
	expect_mem_value_callee(ptr);
}

CUTE_TEST(expect_mem_caller_fail_test)
{
	const char * str = "test string";

	cute_expect_mem_parm(nonexistent_callee, mem, equal, str, sizeof(str));
	expect_mem_value_caller(NULL);
}

CUTE_TEST(expect_mem_parm_fail_test)
{
	const char * str = "test string";

	cute_expect_mem_parm(expect_mem_value_callee,
	                     nonexistent_parm,
	                     equal,
	                     str,
	                     sizeof(str));
	expect_mem_value_caller(NULL);
}

CUTE_TEST(expect_mem_equal_pass_test)
{
	unsigned char chk[sizeof(expect_mem_uint)];

	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     equal,
	                     expect_mem_uint,
	                     sizeof(expect_mem_uint));

	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     equal,
	                     expect_mem_str,
	                     sizeof(expect_mem_str));

	memcpy(chk, expect_mem_uint, sizeof(expect_mem_uint));
	expect_mem_value_caller(chk);
	expect_mem_value_caller("reference string");
}

CUTE_TEST(expect_mem_null_equal_fail_test)
{
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     equal,
	                     expect_mem_str,
	                     sizeof(expect_mem_str));
	expect_mem_value_caller(NULL);
}

CUTE_TEST(expect_mem_literal_equal_fail_test)
{
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     equal,
	                     expect_mem_uint,
	                     sizeof(expect_mem_uint));
	expect_mem_value_caller("test string");
}

CUTE_TEST(expect_mem_var_equal_fail_test)
{
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     equal,
	                     expect_mem_uint,
	                     sizeof(expect_mem_uint));
	expect_mem_value_caller(expect_mem_str);
}

CUTE_TEST(expect_mem_unequal_pass_test)
{
	unsigned char chk[sizeof(expect_mem_uint)];

	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     unequal,
	                     expect_mem_uint,
	                     sizeof(expect_mem_uint));
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     unequal,
	                     expect_mem_str,
	                     sizeof(expect_mem_str));
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     unequal,
	                     expect_mem_str,
	                     sizeof(expect_mem_str));

	expect_mem_value_caller(NULL);
	expect_mem_value_caller("test string");
	memcpy(chk, expect_mem_uint, sizeof(expect_mem_uint));
	expect_mem_value_caller(chk);
}

CUTE_TEST(expect_mem_literal_unequal_fail_test)
{
	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     unequal,
	                     expect_mem_str,
	                     sizeof(expect_mem_str));
	expect_mem_value_caller("reference string");
}

CUTE_TEST(expect_mem_var_unequal_fail_test)
{
	unsigned char chk[sizeof(expect_mem_uint)];

	cute_expect_mem_parm(expect_mem_value_callee,
	                     ptr,
	                     unequal,
	                     expect_mem_uint,
	                     sizeof(expect_mem_uint));

	memcpy(chk, expect_mem_uint, sizeof(expect_mem_uint));
	expect_mem_value_caller(chk);
}

CUTE_GROUP(expect_mem_tests) = {
	CUTE_REF(expect_mem_caller_fail_test),
	CUTE_REF(expect_mem_parm_fail_test),

	CUTE_REF(expect_mem_equal_pass_test),
	CUTE_REF(expect_mem_null_equal_fail_test),
	CUTE_REF(expect_mem_literal_equal_fail_test),
	CUTE_REF(expect_mem_var_equal_fail_test),

	CUTE_REF(expect_mem_unequal_pass_test),
	CUTE_REF(expect_mem_literal_unequal_fail_test),
	CUTE_REF(expect_mem_var_unequal_fail_test),
};

CUTE_SUITE(expect_mem_suite, expect_mem_tests);

CUTE_MAIN(expect_mem_suite, "CUTe", NULL)
