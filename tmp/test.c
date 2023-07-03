#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

void
cute_check_sint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect)
{
	printf("check: %s: %" PRIdMAX " | ", check->expr, check->value);
	printf("expect: %s: %" PRIdMAX "\n", expect->expr, expect->value);
}

void
cute_check_sint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_sint *       check,
                         const struct cute_sint_range * expect)
{
	printf("check: %s: %" PRIdMAX " | ", check->expr, check->value);
	printf("expect: %s: from %" PRIdMAX " to %" PRIdMAX "\n",
	       expect->expr,
	       expect->min,
	       expect->max);
}

void
cute_check_sint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_sint *     check,
                       const struct cute_sint_set * expect)
{
	unsigned int i;

	printf("check: %s: %" PRIdMAX " | ", check->expr, check->value);
	printf("expect: %s:\n", expect->expr);

	for (i = 0; i < expect->count;i++)
		printf("    %" PRIdMAX "\n", expect->items[i]);
}

void
cute_check_float_greater(const char *              file,
                         int                       line,
                         const char *              function,
                         const struct cute_float * check,
                         const struct cute_float * expect)
{
	printf("check: %s: %Lf | ", check->expr, check->value);
	printf("expect: %s: %Lf \n", expect->expr, expect->value);
}

void
cute_check_str_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_str * check,
                     const struct cute_str * expect)
{
	printf("check: %s: \"%s\" | ", check->expr, check->value);
	printf("expect: %s: \"%s\" \n", expect->expr, expect->value);
}

void
cute_check_str_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_str *     check,
                      const struct cute_str_set * expect)
{
	unsigned int i;

	printf("check: %s: \"%s\" | ", check->expr, check->value);
	printf("expect: %s:\n", expect->expr);

	for (i = 0; i < expect->count;i++)
		printf("    \"%s\"\n", expect->items[i]);
}

void
cute_check_mem_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_ptr  * check,
                     const struct cute_mem  * expect)
{
	printf("check: %s: %p | ", check->expr, check->value);
	printf("expect: %s: @%p/%zu\n",
	       kxpect->expr, expect->value, expect->size);
}

void
cute_check_ptr_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_ptr  * check,
                     const struct cute_ptr  * expect)
{
	printf("check: %s: %p | ", check->expr, check->value);
	printf("expect: %s: %p | ", expect->expr, expect->value);
}

struct test {
	struct cute_sint_set set;
};

void build(struct test * t)
{
	t->set = CUTE_SINT_SET(-1, -2, -3);
}

int main(void)
{
	float                        flt_val = 2.5;
	float                        flt_ref = 7.003222;
	int                          val = 2;
	int                          ref = 3;
	int                          min = -1;
	int                          max = 1;
	const struct cute_sint_range range = CUTE_SINT_RANGE(min, 6);
	const struct cute_sint_set   set = CUTE_SINT_SET(ref, -1, -2, -3);

	const char * str_val = "value";
	const char * str_ref = "reference";

	struct test t;

	build(&t);
	cute_check_sint_set(2, in, t.set);

	cute_check_mem("abo", equal, "tot", 4);
	cute_check_ptr(&val, equal, &flt_ref);

	cute_check_str("a", equal, "b");
	cute_check_str(str_val, equal, "b");
	cute_check_str("a", equal, str_ref);
	cute_check_str_set("a", in, CUTE_STR_SET(str_val, "toto", str_ref));

	cute_check_sint(2, greater, 3);
	cute_check_sint(val, greater, -1);
	cute_check_sint(2, greater, ref);
	cute_check_sint(val, greater, ref);

	cute_check_float(2, greater, 3.5);
	cute_check_float(flt_val, greater, flt_ref);

	cute_check_sint_range(2, in, CUTE_SINT_RANGE(3, 5));
	cute_check_sint_range(val, in, CUTE_SINT_RANGE(3, 5));
	cute_check_sint_range(2, in, range);
	cute_check_sint_range(val, in, range);
	cute_check_sint_range(2, in, CUTE_SINT_RANGE(min, max));
	cute_check_sint_range(val, in, CUTE_SINT_RANGE(min, max));

	cute_check_sint_set(2, in, CUTE_SINT_SET());
	cute_check_sint_set(2, in, CUTE_SINT_SET(-2, 3, 0, 7));
	cute_check_sint_set(val, in, CUTE_SINT_SET(-5, 200, 0));
	cute_check_sint_set(val, in, CUTE_SINT_SET(-5, ref, 0));
	cute_check_sint_set(2, in, set);
	cute_check_sint_set(val, in, set);
}


































#if 0
int main(void)
{
	int val = 2;
	int ref = 3;
	
	const union cute_term_sint sint_scal = cute_scal(sint, -250);
	const union cute_term_sint sint_range = cute_range(sint, -20, 9);
	const union cute_term_sint sint_set = cute_set(sint, 0, 5000, -20, 9);

	cute_check_sint(val, greater,  sint_scal);
	cute_check_sint(val, greater,  cute_scal(sint, ref));
	cute_check_sint(5,   greater,  cute_scal(sint, 10));
	cute_check_sint(2,   in_range, cute_range(sint, 3, 5));
	cute_check_sint(val, in_range, cute_range(sint, ref, 20));
	cute_check_sint(val, in_range, sint_range);
	cute_check_sint(9,   in_set,   cute_set(sint, 0, -2, val, ref));
	cute_check_sint(-8,  in_set,   sint_set);
}




#define cute_choose(_expr, _true_expr, _false_expr) \
	__builtin_choose_expr(_expr, _true_expr, _false_expr)

#define cute_value_is_schar(_val) \
	__builtin_types_compatible_p(typeof(_val), signed char)

#define cute_value_is_sshort(_val) \
	__builtin_types_compatible_p(typeof(_val), signed short)

#define cute_value_is_signed(_val) \
	__builtin_types_compatible_p(typeof(_val), signed int)

#define cute_value_is_slong(_val) \
	__builtin_types_compatible_p(typeof(_val), signed long)

#define cute_value_is_sllong(_val) \
	__builtin_types_compatible_p(typeof(_val), signed long long)

#define cute_value_is_sint(_val) \
	(cute_value_is_schar(_val) || \
	 cute_value_is_sshort(_val) || \
	 cute_value_is_signed(_val) || \
	 cute_value_is_slong(_val) || \
	 cute_value_is_sllong(_val))

#define cute_scal_type(_type) \
	cute_typeof_member(struct cute_scal_ ## _type, value)

#define cute_scal(_type, _val) \
	((const union cute_term_ ## _type) \
	 { .scal = cute_scal_init(_type, _val) })

#define cute_range_str(_min, _max) \
	"{" # _min " ... " # _max "}"

#define cute_range_type(_type) \
	cute_typeof_member(struct cute_range_ ## _type, min)

#define cute_range(_type, _min, _max) \
	((const union cute_term_ ## _type) { \
		.range = { \
			.expr = cute_range_str(_min, _max), \
			.min  = (cute_range_type(_type))(_min), \
			.max  = (cute_range_type(_type))(_max) \
		} \
	 })

#define cute_set_str(...) \
	"{" # __VA_ARGS__ "}"

#define cute_set_nr(_type, ...) \
	(sizeof((cute_set_type(_type) []){ __VA_ARGS__ }) / \
	 sizeof(cute_set_type(_type)))

#define cute_set(_type, ...) \
	((const union cute_term_ ## _type) { \
		.set = { \
			.expr  = cute_set_str(__VA_ARGS__), \
			.count = cute_set_nr(_type, __VA_ARGS__), \
			.items = (const cute_set_type(_type) []) \
			         { __VA_ARGS__ } \
		} \
	 })

// FIXME: fix me then use me from cute_check_sint() with _ref arg
#define cute_scal_build(_type, _val) \
	cute_choose(cute_value_is_ ## _type(_val), \
	            cute_scal(_type, _val), \
	            _val)

#define _compile_eval(_expr, _msg) \
	(!!sizeof(struct { static_assert(_expr, _msg); int _dummy; }))

#define compile_eval(_expr, _stmt, _msg) \
	(_compile_eval(_expr, "compile_eval(" # _expr "): " _msg) ? (_stmt) : \
	                                                            (_stmt))

#define cute_value_is_uchar(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned char)

#define cute_value_is_ushort(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned short)

#define cute_value_is_uint(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned int)

#define cute_value_is_ulong(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned long)

#define cute_value_is_ullong(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned long long)

#define cute_value_is_unsign(_val) \
	(cute_value_is_uchar(_val) || \
	 cute_value_is_ushort(_val) || \
	 cute_value_is_uint(_val) || \
	 cute_value_is_ulong(_val) || \
	 cute_value_is_ullong(_val))

#define cute_value_is_float(_val) \
	__builtin_types_compatible_p(typeof(_val), float)

#define cute_value_is_double(_val) \
	__builtin_types_compatible_p(typeof(_val), double)

#define cute_value_is_ldouble(_val) \
	__builtin_types_compatible_p(typeof(_val), long double)

#define cute_value_is_flt(_val) \
	(cute_value_is_float(_val) || \
	 cute_value_is_double(_val) || \
	 cute_value_is_ldouble(_val))

#define cute_value_is_num(_val) \
	(cute_value_is_sign(_val) || \
	 cute_value_is_unsign(_val) || \
	 cute_value_is_flt(_val))

#define cute_value_is_charp(_val) \
	__builtin_types_compatible_p(typeof(_val), char *)

#define cute_value_is_str(_val) \
	cute_value_is_charp(_val)

#define cute_value_is_ptr(_val) \
	__builtin_types_compatible_p(typeof(_val), void *)

#define cute_value_is_supported(_val) \
	(cute_value_is_num(_val) || \
	 cute_value_is_str(_val) || \
	 cute_value_is_ptr(_val))

union cute_value {
	intmax_t     sint;
	uintmax_t    uint;
	long double  flt;
	const char * str;
	const void * ptr;
};

#define cute_value_init_sint(_val) \
	((union cute_value) { .sint = _val })

#define cute_value_init_uint(_val) \
	((union cute_value) { .uint = _val })

#define cute_value_init_flt(_val) \
	((union cute_value) { .flt = _val })

#define cute_value_init_num(_val) \
	cute_choose(cute_value_is_sign(_val), \
	            cute_value_init_sint(_val), \
	            cute_choose(cute_value_is_unsign(_val), \
	                        cute_value_init_uint(_val, \
	                        cute_value_init_flt(_val)))

#define cute_value_init_str(_val) \
	((union cute_value) { .str = _val })

#define cute_value_init_ptr(_val) \
	((union cute_value) { .ptr = _val })

struct cute_term {
	const char *     expr;
	union cute_value value;
};

#define cute_term_init_num(_val) \
	((struct cute_term) { \
		.expr  = # _val, \
		.value = cute_value_init_num(_val) \
	 })

#define cute_term_init_str(_val) \
	((struct cute_term) { \
		.expr  = # _val, \
		.value = cute_value_init_str(_val) \
	 })

#define cute_term_init_ptr(_val) \
	((struct cute_term) { \
		.expr  = # _val, \
		.value = cute_value_init_ptr(_val) \
	 })

#define _cute_check_num(_val, _op, _ref) \
	cute_ensure_ ## _op(__FILE__, \
	                    __LINE__, \
	                    __func__, \
	                    &cute_term_init_num(_val), \
	                    &(_ref))

#define cute_check_num(...) \
	_cute_check_num(__VA_ARGS__)

#define equal(_ref) \
	equal, cute_term_init_num(_ref)

#define unequal(_ref) \
	unequal, cute_term_init_num(_ref)

#define greater_than(_ref) \
	greater_than, cute_term_init_num(_ref)

#define greater_than_or_equal(_ref) \
	greater_than_or_equal, cute_term_init_num(_ref)

#define lower_than(_ref) \
	lower_than, cute_term_init_num(_ref)

#define lower_than_or_equal(_ref) \
	lower_than_or_equal, cute_term_init_num(_ref)

void
cute_ensure_greater_than(const char *             file,
                         int                      line,
                         const char *             function,
                         const struct cute_term * check,
                         const struct cute_term * expect)
{
	printf("%s\n", expect->expr);
	printf("%" PRIdMAX "\n", expect->val);
}


int main(void)
{
	int              sint = -32;
	union cute_value sint_val = cute_value_init_num(sint);

	unsigned int     uint = -32;
	union cute_value uint_val = cute_value_init_num(uint);

	unsigned short   sshrt = 16;
	union cute_value sshrt_val = cute_value_init_num(sshrt);

	float            sflt = -2.5;
	union cute_value sflt_val = cute_value_init_num(sflt);

	void * ptr = NULL;
	union cute_value ptr_val = cute_value_init_ptr(ptr);

	struct { int a; int b; } sptr = { 0, 10 };
	union cute_value sptr_val = cute_value_init_ptr(&sptr);

	uint64_t u64 = UINT64_MAX;
	union cute_value u64_val = cute_value_init_num(u64);

	char arr[] = "bob\0";
	union cute_value arr_val = cute_value_init_str(arr);

	const char * str = "bob\0";
	union cute_value str_val = cute_value_init_str(str);

	printf("sint: %d --> %" PRIdMAX "\n", sint, sint_val.sint);
	printf("uint: %d --> %" PRIuMAX "\n", uint, uint_val.uint);
	//printf("sshrt: %hd --> %" PRIdMAX "\n", sshrt, sshrt_val.sint);
	//printf("ptr: %p --> %p\n", ptr, ptr_val.ptr);
	//printf("sptr: %p --> %p\n", sptr, sptr_val.ptr);
	//printf("u64: %lu --> %" PRIxMAX "\n", u64, u64_val.uint);

	//sint = ptr;
	//ptr = sint;
}

#if 0
struct cute_sint {
	const char * expr;
	intmax_t     val;
};

#define __CUTE_SINT_INIT(_val) \
	((const struct cute_sint) { .expr = # _val, .val  = _val })

#define greater_than(_val) \
	greater, &__CUTE_SINT_INIT(_val)


#define cute_ensure_sint(...) \
	_cute_ensure_sint(__VA_ARGS__)

#define _cute_ensure_sint(_a, _op, _b) \
	cute_ensure_sint_ ## _op(__FILE__, \
	                         __LINE__, \
	                         __func__, \
	                         &__CUTE_SINT_INIT(_a), \
	                         _b)

struct cute_sint_range {
	const char * expr;
	intmax_t     min;
	intmax_t     max;
};

#define __CUTE_RANGE_STRING(_min, _max) \
	"{" # _min " ... " # _max "}"

#define __CUTE_SINT_INIT_RANGE(_min, _max) \
	((const struct cute_sint_range) \
	 { \
		.expr = __CUTE_RANGE_STRING(_min, _max), \
		.min  = _min, \
		.max  = _max \
	 })

#define in_range(_min, _max) \
	in_range, &__CUTE_SINT_INIT_RANGE(_min, _max)

void
cute_ensure_sint_in_range(const char *                   file,
                          int                            line,
                          const char *                   function,
                          const struct cute_sint *       check,
                          const struct cute_sint_range * expect)
{
	printf("%s\n", expect->expr);
	printf("%" PRIdMAX "\n", expect->min);
	printf("%" PRIdMAX "\n", expect->max);

}

struct cute_sint_set {
	const char *     expr;
	unsigned int     count;
	const intmax_t * items;
};

#define __CUTE_SET_STRING(...) \
	"{" # __VA_ARGS__ "}"

#define __CUTE_SINT_INIT_SET(...) \
	((const struct cute_sint_set) \
	 { \
		.expr  = __CUTE_SET_STRING(__VA_ARGS__), \
		.count = sizeof((intmax_t []){ __VA_ARGS__ }) / \
		         sizeof(intmax_t), \
		.items = (const intmax_t []){ __VA_ARGS__ } \
	 })

#define in_set(...) \
	in_set, &__CUTE_SINT_INIT_SET(__VA_ARGS__)

void
cute_ensure_sint_in_set(const char *                 file,
                        int                          line,
                        const char *                 function,
                        const struct cute_sint *     check,
                        const struct cute_sint_set * expect)
{
	unsigned int i;

	printf("%s\n", expect->expr);
	for (i = 0; i < expect->count;i++)
		printf("%" PRIdMAX "\n", expect->items[i]);

}

int main(void)
{
	intmax_t a = 2;
	intmax_t mymax = 10;
	intmax_t b = -1;
	unsigned short c = 16;

	cute_ensure_sint(a, greater_than(mymax));

	cute_ensure_sint(a, in_range(0, mymax));

	cute_ensure_sint(a, in_set(0, mymax, b, 6, c));
}
#endif
#endif
