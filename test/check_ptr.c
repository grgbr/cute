#include "cute/cute.h"
#include "cute/check.h"

CUTE_TEST(check_ptr_equal_pass_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(NULL, equal, NULL);
	cute_check_ptr((void *)0xdeadbeef, equal, (void *)0xdeadbeef);
	cute_check_ptr(chk, equal, (void *)0xdeadbeef);
	cute_check_ptr((void *)0xdeadc0de, equal, ref);
	chk = (void *)0xdeadc0de;
	cute_check_ptr(chk, equal, ref);
}

CUTE_TEST(check_ptr_literal_equal_fail_test)
{
	cute_check_ptr((void *)0xdeadbeef, equal, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_var_equal_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, equal, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_equal_ref_fail_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr((void *)0xdeadbeef, equal, ref);
}

CUTE_TEST(check_ptr_var_equal_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(chk, equal, ref);
}

CUTE_TEST(check_ptr_unequal_pass_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr((void *)0xdeadbeef, unequal, NULL);
	cute_check_ptr(NULL, unequal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, unequal, (void *)0xdeadc0de);
	cute_check_ptr(chk, unequal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, unequal, ref);
	cute_check_ptr(chk, unequal, ref);
}

CUTE_TEST(check_ptr_literal_unequal_fail_test)
{
	cute_check_ptr((void *)0xdeadbeef, unequal, (void *)0xdeadbeef);
}

CUTE_TEST(check_ptr_var_unequal_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, unequal, (void *)0xdeadbeef);
}

CUTE_TEST(check_ptr_unequal_ref_fail_test)
{
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr((void *)0xdeadc0de, unequal, ref);
}

CUTE_TEST(check_ptr_var_unequal_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(chk, unequal, ref);
}

CUTE_TEST(check_ptr_greater_pass_test)
{
	const void * chk = (void *)0xdeadc0de;
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr((void *)0xdeadc0de, greater, NULL);
	cute_check_ptr((void *)0xdeadc0de, greater, (void *)0xdeadbeef);
	cute_check_ptr(chk, greater, (void *)0xdeadbeef);
	cute_check_ptr((void *)0xdeadc0de, greater, ref);
	cute_check_ptr(chk, greater, ref);
}

CUTE_TEST(check_ptr_literal_greater_fail_test)
{
	cute_check_ptr((void *)0xdeadc0de, greater, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_var_greater_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, greater, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_greater_ref_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(NULL, greater, ref);
}

CUTE_TEST(check_ptr_var_greater_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(chk, greater, ref);
}

CUTE_TEST(check_ptr_greater_equal_pass_test)
{
	const void * chk = (void *)0xdeadc0de;
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(NULL, greater_equal, NULL);
	cute_check_ptr((void *)0xdeadc0de, greater_equal, NULL);
	cute_check_ptr((void *)0xdeadc0de, greater_equal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadc0de, greater_equal, (void *)0xdeadbeef);
	cute_check_ptr(chk, greater_equal, (void *)0xdeadbeef);
	cute_check_ptr((void *)0xdeadbeef, greater_equal, ref);
	cute_check_ptr(chk, greater_equal, ref);
}

CUTE_TEST(check_ptr_literal_greater_equal_fail_test)
{
	cute_check_ptr((void *)0xdeadbeef, greater_equal, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_var_greater_equal_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, greater_equal, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_greater_equal_ref_fail_test)
{
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(NULL, greater_equal, ref);
}

CUTE_TEST(check_ptr_var_greater_equal_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(chk, greater_equal, ref);
}

CUTE_TEST(check_ptr_lower_pass_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(NULL, lower, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, lower, (void *)0xdeadc0de);
	cute_check_ptr(chk, lower, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, lower, ref);
	cute_check_ptr(chk, lower, ref);
}

CUTE_TEST(check_ptr_literal_lower_fail_test)
{
	cute_check_ptr((void *)0xdeadc0de, lower, (void *)0xdeadc0de);
}

CUTE_TEST(check_ptr_var_lower_fail_test)
{
	const void * chk = (void *)0xdeadc0de;

	cute_check_ptr(chk, lower, (void *)0xdeadbeef);
}

CUTE_TEST(check_ptr_lower_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, lower, NULL);
}

CUTE_TEST(check_ptr_var_lower_ref_fail_test)
{
	const void * chk = (void *)0xdeadc0de;
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(chk, lower, ref);
}

CUTE_TEST(check_ptr_lower_equal_pass_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * ref = (void *)0xdeadc0de;

	cute_check_ptr(NULL, lower_equal, NULL);
	cute_check_ptr(NULL, lower_equal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, lower_equal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, lower_equal, (void *)0xdeadbeef);
	cute_check_ptr(chk, lower_equal, (void *)0xdeadc0de);
	cute_check_ptr((void *)0xdeadbeef, lower_equal, ref);
	cute_check_ptr(chk, lower_equal, ref);
}

CUTE_TEST(check_ptr_literal_lower_equal_fail_test)
{
	cute_check_ptr((void *)0xdeadc0de, lower_equal, (void *)0xdeadbeef);
}

CUTE_TEST(check_ptr_var_lower_equal_fail_test)
{
	const void * chk = (void *)0xdeadc0de;

	cute_check_ptr(chk, lower_equal, (void *)0xdeadbeef);
}

CUTE_TEST(check_ptr_lower_equal_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;

	cute_check_ptr(chk, lower_equal, NULL);
}

CUTE_TEST(check_ptr_var_lower_equal_ref_fail_test)
{
	const void * chk = (void *)0xdeadc0de;
	const void * ref = (void *)0xdeadbeef;

	cute_check_ptr(chk, lower_equal, ref);
}

CUTE_TEST(check_ptr_in_range_pass_test)
{
	const void *                chk = (void *)0xdeadbeef;
	const void *                min_ref = NULL;
	const void *                max_ref = (void *)0xdeadc0de;
	const struct cute_ptr_range range = CUTE_PTR_RANGE(min_ref, max_ref);

	cute_check_ptr_range(NULL,
	                     in,
	                     CUTE_PTR_RANGE(NULL, (void *)0xdeadc0de));
	cute_check_ptr_range(chk, in, CUTE_PTR_RANGE(NULL, (void *)0xdeadbeef));
	cute_check_ptr_range(NULL, in, CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range(chk, in, CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range(NULL, in, range);
	cute_check_ptr_range(chk, in, range);
}

CUTE_TEST(check_ptr_literal_in_range_fail_test)
{
	cute_check_ptr_range(NULL,
	                     in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0de));
}

CUTE_TEST(check_ptr_var_in_range_fail_test)
{
	const void * chk = (void *)0xdeadc0de;

	cute_check_ptr_range(chk,
	                     in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0da));
}

CUTE_TEST(check_ptr_in_range_ref_fail_test)
{
	const void * min_ref = (void *)0xdeadbeef;
	const void * max_ref = (void *)0xdeadc0de;

	cute_check_ptr_range((void *)0xdeadbeef,
	                     in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range(NULL,
	                     in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_ptr_var_in_range_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeef;
	const void * min_ref = (void *)0xdeadbeef;
	const void * max_ref = (void *)0xdeadc0de;

	cute_check_ptr_range(chk, in, CUTE_PTR_RANGE(min_ref, max_ref));
	chk = (void *)0xdeadbeee;
	cute_check_ptr_range(chk, in, CUTE_PTR_RANGE(min_ref, max_ref));
}


























CUTE_TEST(check_ptr_not_in_range_pass_test)
{
	const void *                chk = (void *)0xdeadbeee;
	const void *                min_ref = (void *)0xdeadbeef;
	const void *                max_ref = (void *)0xdeadc0de;
	const struct cute_ptr_range range = CUTE_PTR_RANGE(min_ref, max_ref);

	cute_check_ptr_range(NULL,
	                     not_in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0de));
	cute_check_ptr_range(chk,
	                     not_in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0de));
	cute_check_ptr_range(NULL,
	                     not_in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range(chk,
	                     not_in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range(NULL, not_in, range);
	cute_check_ptr_range(chk, not_in, range);
}

CUTE_TEST(check_ptr_literal_not_in_range_fail_test)
{
	cute_check_ptr_range((void *)0xdeadc0da,
	                     not_in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0de));
}

CUTE_TEST(check_ptr_var_not_in_range_fail_test)
{
	const void * chk = (void *)0xdeadc0da;

	cute_check_ptr_range(chk,
	                     not_in,
	                     CUTE_PTR_RANGE((void *)0xdeadbeef,
	                                    (void *)0xdeadc0de));
}

CUTE_TEST(check_ptr_not_in_range_ref_fail_test)
{
	const void * min_ref = (void *)0xdeadbeef;
	const void * max_ref = (void *)0xdeadc0de;

	cute_check_ptr_range((void *)0xdeadbeee,
	                     not_in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
	cute_check_ptr_range((void *)0xdeadbeef,
	                     not_in,
	                     CUTE_PTR_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_ptr_var_not_in_range_ref_fail_test)
{
	const void * chk = (void *)0xdeadbeee;
	const void * min_ref = (void *)0xdeadbeef;
	const void * max_ref = (void *)0xdeadc0de;

	cute_check_ptr_range(chk, not_in, CUTE_PTR_RANGE(min_ref, max_ref));
	chk = (void *)0xdeadc0da;
	cute_check_ptr_range(chk, not_in, CUTE_PTR_RANGE(min_ref, max_ref));
}

CUTE_TEST(check_ptr_in_set_pass_test)
{
	const void *              null = NULL;
	const void *              code = (void *)0xdeadc0de;
	const void *              chk = (void *)0xdeadcafe;
	const struct cute_ptr_set set = CUTE_PTR_SET(NULL,
	                                             (void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             (void *)0xdeadc0de,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set(NULL,
	                   in,
	                   CUTE_PTR_SET(NULL,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set(chk,
	                   in,
	                   CUTE_PTR_SET(NULL,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set((void *)0xdeadce11,
	                   in,
	                   CUTE_PTR_SET(NULL,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set(chk,
	                   in,
	                   CUTE_PTR_SET(null,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                code,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set((void *)0xdeadbabe, in, set);
	cute_check_ptr_set(chk, in, set);
}

CUTE_TEST(check_ptr_literal_in_set_fail_test)
{
	cute_check_ptr_set((void *)0xdeadc0da,
	                   in,
	                   CUTE_PTR_SET(NULL,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
}

CUTE_TEST(check_ptr_var_in_set_fail_test)
{
	const void * chk = (void *)0xdeadc0da;

	cute_check_ptr_set(chk,
	                   in,
	                   CUTE_PTR_SET(NULL,
	                                (void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
}

CUTE_TEST(check_ptr_in_set_ref_fail_test)
{
	const void *              babe = (void *)0xdeadbabe;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET(NULL,
	                                             (void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             code,
	                                             babe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set((void *)0xdeadcafe, in, set);
	cute_check_ptr_set((void *)0xdeadc0da, in, set);
}

CUTE_TEST(check_ptr_var_in_set_ref_fail_test)
{
	const void *              chk =  (void *)0xdeadce11;
	const void *              babe = (void *)0xdeadbabe;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET(NULL,
	                                             (void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             code,
	                                             babe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set(chk, in, set);
	chk = (void *)0xdeadbeee;
	cute_check_ptr_set(chk, in, set);
}

CUTE_TEST(check_ptr_not_in_set_pass_test)
{
	const void *              cafe = (void *)0xdeadcafe;
	const void *              code = (void *)0xdeadc0de;
	const void *              chk = (void *)0xdeaddddd;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             (void *)0xdeadc0de,
	                                             (void *)0xdeadbabe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set(NULL,
	                   not_in,
	                   CUTE_PTR_SET((void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set(chk,
	                   not_in,
	                   CUTE_PTR_SET((void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set((void *)0xdeaddddd,
	                   not_in,
	                   CUTE_PTR_SET((void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set(chk,
	                   not_in,
	                   CUTE_PTR_SET(cafe,
	                                (void *)0xdeadbeef,
	                                code,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
	cute_check_ptr_set((void *)0xdeaddddd, not_in, set);
	cute_check_ptr_set(chk, not_in, set);
}

CUTE_TEST(check_ptr_literal_not_in_set_fail_test)
{
	cute_check_ptr_set((void *)0xdeadc0de,
	                   not_in,
	                   CUTE_PTR_SET((void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
}

CUTE_TEST(check_ptr_var_not_in_set_fail_test)
{
	const void * chk = (void *)0xdeadc0de;

	cute_check_ptr_set(chk,
	                   not_in,
	                   CUTE_PTR_SET((void *)0xdeadcafe,
	                                (void *)0xdeadbeef,
	                                (void *)0xdeadc0de,
	                                (void *)0xdeadbabe,
	                                (void *)0xdeadce11));
}

CUTE_TEST(check_ptr_not_in_set_ref_fail_test)
{
	const void *              babe = (void *)0xdeadbabe;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             code,
	                                             babe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set((void *)0xdeaddddd, not_in, set);
	cute_check_ptr_set((void *)0xdeadce11, not_in, set);
}

CUTE_TEST(check_ptr_var_not_in_set_ref_fail_test)
{
	const void *              chk =  (void *)0xdeaddddd;
	const void *              babe = (void *)0xdeadbabe;
	const void *              code = (void *)0xdeadc0de;
	const struct cute_ptr_set set = CUTE_PTR_SET((void *)0xdeadcafe,
	                                             (void *)0xdeadbeef,
	                                             code,
	                                             babe,
	                                             (void *)0xdeadce11);

	cute_check_ptr_set(chk, not_in, set);
	chk = (void *)0xdeadbeef;
	cute_check_ptr_set(chk, not_in, set);
}

CUTE_GROUP(check_ptr_tests) = {
	CUTE_REF(check_ptr_equal_pass_test),
	CUTE_REF(check_ptr_literal_equal_fail_test),
	CUTE_REF(check_ptr_var_equal_fail_test),
	CUTE_REF(check_ptr_equal_ref_fail_test),
	CUTE_REF(check_ptr_var_equal_ref_fail_test),

	CUTE_REF(check_ptr_unequal_pass_test),
	CUTE_REF(check_ptr_literal_unequal_fail_test),
	CUTE_REF(check_ptr_var_unequal_fail_test),
	CUTE_REF(check_ptr_unequal_ref_fail_test),
	CUTE_REF(check_ptr_var_unequal_ref_fail_test),

	CUTE_REF(check_ptr_greater_pass_test),
	CUTE_REF(check_ptr_literal_greater_fail_test),
	CUTE_REF(check_ptr_var_greater_fail_test),
	CUTE_REF(check_ptr_greater_ref_fail_test),
	CUTE_REF(check_ptr_var_greater_ref_fail_test),

	CUTE_REF(check_ptr_greater_equal_pass_test),
	CUTE_REF(check_ptr_literal_greater_equal_fail_test),
	CUTE_REF(check_ptr_var_greater_equal_fail_test),
	CUTE_REF(check_ptr_greater_equal_ref_fail_test),
	CUTE_REF(check_ptr_var_greater_equal_ref_fail_test),

	CUTE_REF(check_ptr_lower_pass_test),
	CUTE_REF(check_ptr_literal_lower_fail_test),
	CUTE_REF(check_ptr_var_lower_fail_test),
	CUTE_REF(check_ptr_lower_ref_fail_test),
	CUTE_REF(check_ptr_var_lower_ref_fail_test),

	CUTE_REF(check_ptr_lower_equal_pass_test),
	CUTE_REF(check_ptr_literal_lower_equal_fail_test),
	CUTE_REF(check_ptr_var_lower_equal_fail_test),
	CUTE_REF(check_ptr_lower_equal_ref_fail_test),
	CUTE_REF(check_ptr_var_lower_equal_ref_fail_test),

	CUTE_REF(check_ptr_in_range_pass_test),
	CUTE_REF(check_ptr_literal_in_range_fail_test),
	CUTE_REF(check_ptr_var_in_range_fail_test),
	CUTE_REF(check_ptr_in_range_ref_fail_test),
	CUTE_REF(check_ptr_var_in_range_ref_fail_test),

	CUTE_REF(check_ptr_not_in_range_pass_test),
	CUTE_REF(check_ptr_literal_not_in_range_fail_test),
	CUTE_REF(check_ptr_var_not_in_range_fail_test),
	CUTE_REF(check_ptr_not_in_range_ref_fail_test),
	CUTE_REF(check_ptr_var_not_in_range_ref_fail_test),

	CUTE_REF(check_ptr_in_set_pass_test),
	CUTE_REF(check_ptr_literal_in_set_fail_test),
	CUTE_REF(check_ptr_var_in_set_fail_test),
	CUTE_REF(check_ptr_in_set_ref_fail_test),
	CUTE_REF(check_ptr_var_in_set_ref_fail_test),

	CUTE_REF(check_ptr_not_in_set_pass_test),
	CUTE_REF(check_ptr_literal_not_in_set_fail_test),
	CUTE_REF(check_ptr_var_not_in_set_fail_test),
	CUTE_REF(check_ptr_not_in_set_ref_fail_test),
	CUTE_REF(check_ptr_var_not_in_set_ref_fail_test)
};

CUTE_SUITE(check_ptr_suite, check_ptr_tests);

CUTE_MAIN(check_ptr_suite, "CUTe", NULL)
