#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#define cute_is_schar(_val) \
	__builtin_types_compatible_p(typeof(_val), signed char)

#define cute_is_sshort(_val) \
	__builtin_types_compatible_p(typeof(_val), signed short)

#define cute_is_sint(_val) \
	__builtin_types_compatible_p(typeof(_val), signed int)

#define cute_is_slong(_val) \
	__builtin_types_compatible_p(typeof(_val), signed long)

#define cute_is_sllong(_val) \
	__builtin_types_compatible_p(typeof(_val), signed long long)

#define cute_is_sign(_val) \
	(cute_is_schar(_val) || \
	 cute_is_sshort(_val) || \
	 cute_is_sint(_val) || \
	 cute_is_slong(_val) || \
	 cute_is_sllong(_val))

#define cute_is_uchar(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned char)

#define cute_is_ushort(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned short)

#define cute_is_uint(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned int)

#define cute_is_ulong(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned long)

#define cute_is_ullong(_val) \
	__builtin_types_compatible_p(typeof(_val), unsigned long long)

#define cute_is_unsign(_val) \
	(cute_is_uchar(_val) || \
	 cute_is_ushort(_val) || \
	 cute_is_uint(_val) || \
	 cute_is_ulong(_val) || \
	 cute_is_ullong(_val))

#define cute_is_float(_val) \
	__builtin_types_compatible_p(typeof(_val), float)

#define cute_is_double(_val) \
	__builtin_types_compatible_p(typeof(_val), double)

#define cute_is_ldouble(_val) \
	__builtin_types_compatible_p(typeof(_val), long double)

#define cute_is_flt(_val) \
	(cute_is_float(_val) || \
	 cute_is_double(_val) || \
	 cute_is_ldouble(_val))

#define cute_is_charp(_val) \
	__builtin_types_compatible_p(typeof(_val), char *)

#define cute_is_int8p(_val) \
	__builtin_types_compatible_p(typeof(_val), int8_t *)

#define cute_is_str(_val) \
	(cute_is_charp(_val) || cute_is_int8p(_val))

#define cute_is_ptr(_val) \
	__builtin_types_compatible_p(typeof(_val), void *)

#define cute_is_type_supported(_val) \
	(cute_is_sint(_val) || \
	 cute_is_uint(_val) || \
	 cute_is_flt(_val) || \
	 cute_is_str(_val) || \
	 cute_is_ptr(_val))

union cute_value {
	intmax_t    sint;
	uintmax_t   uint;
	long double flt;
	char *      str;
	void *      ptr;
};

#define cute_value_type_init(_type, _val) \
	((union cute_value){ ._type = _val })

#define cute_choose(_expr, _true_expr, _false_expr) \
	__builtin_choose_expr(_expr, _true_expr, _false_expr)

#define _compile_eval(_expr, _msg) \
	(!!sizeof(struct { static_assert(_expr, _msg); int _dummy; }))

#define compile_eval(_expr, _stmt, _msg) \
	(_compile_eval(_expr, "compile_eval(" # _expr "): " _msg) ? (_stmt) : \
	                                                            (_stmt))

#if 0
#define _cute_value_init(_val) \
	cute_choose(cute_is_sign(_val), \
		cute_value_type_init(sint, _val), \
		cute_choose(cute_is_unsign(_val), \
			cute_value_type_init(uint, _val), \
			cute_choose(cute_is_flt(_val), \
				cute_value_type_init(flt, _val), \
				cute_choose(cute_is_str(_val), \
					cute_value_type_init(str, _val), \
					cute_value_type_init(ptr, _val)))))
#endif

#define _cute_value_init(_val) \
	cute_choose(cute_is_sign(_val), \
		cute_value_type_init(sint, _val), \
		cute_choose(cute_is_unsign(_val), \
			cute_value_type_init(uint, _val), \
			cute_choose(cute_is_flt(_val), \
				cute_value_type_init(flt, _val), \
				cute_value_type_init(ptr, _val))))

#define cute_value_init(_val) \
	compile_eval(cute_is_type_supported(_val), \
	             _cute_value_init(_val), \
	             "Unsupported type")

int main(void)
{
	int              sint = -32;
	union cute_value sint_val = cute_value_init(sint);

	//unsigned short   sshrt = 16;
	//union cute_value sshrt_val = cute_value_init(sshrt);

	printf("sint: %d --> %" PRIdMAX "\n", sint, sint_val.sint);
	//printf("sshrt: %hd --> %" PRIdMAX "\n", sshrt, sshrt_val.sint);
}

#if 0
struct cute_term {
	const char *     expr;
	union cute_value value;
};

#define cute_term_init(_type, _val) \
	((struct cute_term){ .expr = # _val, .value._type = _val })

struct cute_sint {
	const char * expr;
	intmax_t     val;
};

#define __CUTE_SINT_INIT(_val) \
	((const struct cute_sint) { .expr = # _val, .val  = _val })

#define greater_than(_val) \
	greater, &__CUTE_SINT_INIT(_val)

void
cute_ensure_sint_greater(const char *             file,
                         int                      line,
                         const char *             function,
                         const struct cute_sint * check,
                         const struct cute_sint * expect)
{
	unsigned int i;

	printf("%s\n", expect->expr);
	printf("%" PRIdMAX "\n", expect->val);
}


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
