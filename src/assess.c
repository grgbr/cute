/*
int / float:
    ==  equals
    !=  unequals
    >   is_greater_than
    >=  greater_or_equal
    <   lower
    <=  lower_or_equal
    in_set
    not_in_set
    in_range
    not_in_range

ptr:
    ==  equals
    !=  unequals
    >   is_greater_than
    >=  greater_or_equal
    <   lower
    <=  lower_or_equal
    in_range
    not_in_range

string:
    starts_with
    ends_with
    contains
    equals
    unequals
    longer
    shorter
    greater
    greater_or_equal
    lower
    lower_or_equal

memory:
    equals
    unequals
*/

enum cute_assess_oper {
    CUTE_ASSESS_EQUAL_OPER = 0,
    CUTE_ASSESS_UNEQUAL_OPER,
    CUTE_ASSESS_GREATER_OPER,
    CUTE_ASSESS_GREATER_EQUAL_OPER,
    CUTE_ASSESS_LOWER_OPER,
    CUTE_ASSESS_LOWER_EQUAL_OPER,
    CUTE_ASSESS_INRANGE_OPER,
    CUTE_ASSESS_NOT_INRANGE_OPER,
    CUTE_ASSESS_INSET_OPER,
    CUTE_ASSESS_NOT_INSET_OPER,
    CUTE_ASSESS_START_WITH_OPER,
    CUTE_ASSESS_END_WITH_OPER,
    CUTE_ASSESS_CONTAINS_OPER,
    CUTE_ASSESS_OPER_NR,
};

#define cute_assess_assert_oper(_oper) \
	cute_assert((_oper) >= 0); \
	cute_assert((_oper) < CUTE_ASSESS_OPER_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_oper_intern(_oper) \
	cute_assess_assert_oper(_oper)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_oper_intern(_oper)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

union cute_assess_member {
	intmax_t    i;
	uintmax_t   u;
	long double f;
	void *      p;
	char *      s;
};

struct cute_assess_range {
	union cute_assess_member min;
	union cute_assess_member max;
};

struct cute_assess_set {
	unsigned int               count;
	union cute_assess_member * items;
};

#define cute_assess_assert_set(_set) \
	cute_assert(_set); \
	cute_assert((_set)->count); \
	cute_assert((_set)->items)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_set_intern(_set) \
	cute_assess_assert_set(_set)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_set_intern(_set)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

enum cute_assess_desc {
	CUTE_ASSESS_EXPECT_DESC = 0,
	CUTE_ASSESS_FOUND_DESC,
	CUTE_ASSESS_DESC_NR
};

#define cute_assess_assert_desc(_desc) \
	cute_assert((_desc) >= 0); \
	cute_assert((_desc) < CUTE_ASSESS_DESC_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_desc_intern(_desc) \
	cute_assess_assert_desc(_desc)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_desc_intern(_desc)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

typedef bool (cute_assess_cmp_fn)(const struct cute_assess * assess,
                                  union cute_assess_member   value);

typedef void (cute_assess_desc_fn)(const struct cute_assess * assess,
                                   enum cute_assess_desc      desc,
                                   FILE *                     stdio);

struct cute_assess_ops {
	cute_assess_cmp_fn *  cmp;
	cute_assess_desc_fn * desc;
};

#define cute_assess_assert_ops(_ops) \
	cute_assert(_ops); \
	cute_assert((_ops)->cmp); \
	cute_assert((_ops)->desc)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_ops_intern(_ops) \
	cute_assess_assert_ops(_ops)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_ops_intern(_ops)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

struct cute_assess {
	const struct cute_assess_ops * ops;
	enum cute_oper                 oper;
	const char *                   chk_expr;
	const char *                   xpct_expr;
	union {
		union cute_assess_member member;
		struct cute_assess_range range;
		struct cute_assess_set   set;
	}                              xpct_val;
	union cute_assess_member       chk_val;
};

#define cute_assess_assert(_assess) \
	cute_assert(_assess); \
	cute_assess_assert_ops((_assess)->ops); \
	cute_assess_assert_oper((_assess)->oper); \
	cute_assess((_assess)->chk_expr); \
	cute_assess((_assess)->chk_expr[0]); \
	cute_assess((_assess)->xpct_expr); \
	cute_assess((_assess)->xpct_expr[0])

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assess_assert_intern(_assess) \
	cute_assess_assert(_assess)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assess_assert_intern(_assess)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static const char *
cute_assess_oper_label(enum cute_assess_oper oper)
{
	cute_assess_assert_oper_intern(oper);

	static const char * labels[] = {
		[CUTE_ASSESS_EQUAL_OPER]         = "==",
		[CUTE_ASSESS_UNEQUAL_OPER]       = "!=",
		[CUTE_ASSESS_GREATER_OPER]       = ">",
		[CUTE_ASSESS_GREATER_EQUAL_OPER] = ">=",
		[CUTE_ASSESS_LOWER_OPER] =       = "<",
		[CUTE_ASSESS_LOWER_EQUAL_OPER]   = "<=",
		[CUTE_ASSESS_INRANGE_OPER]       = "in range",
		[CUTE_ASSESS_NOT_INRANGE_OPER]   = "not in range",
		[CUTE_ASSESS_INSET_OPER]         = "in set",
		[CUTE_ASSESS_NOT_INSET_OPER]     = "not in set",
		[CUTE_ASSESS_START_WITH_OPER]    = "starts with",
		[CUTE_ASSESS_END_WITH_OPER]      = "ends with"
		[CUTE_ASSESS_CONTAINS_OPER]      = "contains"
	};

	return labels[oper];
}

void
cute_assess_check(struct cute_assess * assess, union cute_assess_member value)
{
	cute_assess_assert(assess);

	if (assess->ops->cmp(assess, value))
		return;

	assess->chk_val.member = value;

	//cute_break()
}

void
cute_assess_desc(const struct cute_assess * assess,
                 enum cute_assess_desc      desc,
                 FILE *                     stdio)
{
	cute_assess_assert(assess);
	cute_assess_assert_desc(desc);
	cute_assert(stdio);

	assess->ops->desc(assess, desc, stdio);
}

static bool
cute_assess_cmp_int_member(const struct cute_assess * assess,
                           union cute_assess_member   value)
{
	cute_assess_assert_intern(assess);

	switch (assess->oper) {
	case CUTE_ASSESS_EQUAL_OPER:
		return value.i == assess->xpct_val.member.i;

	case CUTE_ASSESS_UNEQUAL_OPER:
		return value.i != assess->xpct_val.member.i;

	case CUTE_ASSESS_GREATER_OPER:
		return value.i > assess->xpct_val.member.i;

	case CUTE_ASSESS_GREATER_EQUAL_OPER:
		return value.i >= assess->xpct_val.member.i;

	case CUTE_ASSESS_LOWER_OPER:
		return value.i < assess->xpct_val.member.i;

	case CUTE_ASSESS_LOWER_EQUAL_OPER:
		return value.i <= assess->xpct_val.member.i;

	default:
		__cute_unreachable();
	}
}

static void
cute_assess_desc_int_chk(const struct cute_assess * assess,
                         FILE *                     stdio)
{
	cute_assess_assert_intern(assess);
	cute_assert_intern(stdio);

	fprintf(stdio,
	        "%s equals [%PRIdMAX]",
	        assess->chk_expr,
	        assess->chk_val.member.i);
}

static void
cute_assess_desc_int_member(const struct cute_assess * assess,
                            enum cute_assess_desc      desc,
                            FILE *                     stdio)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_desc_intern(desc);
	cute_assert_intern(stdio);

	const char * oper;

	oper = cute_assess_oper_label(assess->oper);

	switch (desc) {
	case CUTE_ASSESS_EXPECT_DESC:
		fprintf(stdio,
		        "%s %s %s [%PRIdMAX]",
		        assess->chk_expr,
		        oper,
		        assess->xpct_expr,
		        assess->xpct_val.member.i);
		break;

	case CUTE_ASSESS_FOUND_DESC:
		cute_assess_desc_int_chk(assess, stdio);
		break;

	default:
		__cute_unreachable();
	}
}

const struct cute_assess_ops cute_assess_int_ops = {
	.cmp  = cute_assess_cmp_int_member,
	.desc = cute_assess_desc_int_member
};

void
cute_assess_build_int_member(struct cute_assess * assess,
                             enum cute_oper       oper,
                             const char *         chk_expr,
                             const char *         xpct_expr,
                             intmax_t             xpct_val)
{
	assess->ops = &cute_assess_int_ops;
	assess->oper = oper;
	assess->chk_expr = chk_expr;
	assess->xpct_expr = xpct_expr;
	assess->xpct_val.member.i = xpct_val;
}

void
cute_assess_int_member(struct cute_assess * assess,
                       const char *         chk_expr,
                       intmax_t             chk_val,
                       enum cute_oper       oper,
                       const char *         xpct_expr,
                       intmax_t             xpct_val)
{
	union cute_assess_member val = { .i = chk_val };

	cute_assess_build_int_member(assess,
	                             oper,
	                             chk_expr,
	                             xpct_expr,
	                             xpct_val);

	cute_assess_check(assess, val);
}

#define cute_ensure_int_member(_a, _op, _b) \
	cute_assess_int_member(&cute_curr_run->assess, \
	                       # _chk_expr, \
	                       _a, \
	                       _oper, \
	                       # _xpct_expr, \
	                       _b)

cute_ensure_signed(a, greater_than, 2)






#if 0
static bool
cute_assess_cmp_int_range(const struct cute_assess * assess,
                          union cute_assess_member   value)
{
	cute_assess_assert_intern(assess);

	switch (assess->oper) {
	case CUTE_ASSESS_INRANGE_OPER:
		return (value.i >= assess->xpct_val.range.min) &&
		       (value.i <= assess->xpct_val.range.max);

	case CUTE_ASSESS_NOT_INRANGE_OPER:
		return (value.i < assess->xpct_val.range.min) &&
		       (value.i > assess->xpct_val.range.max);

	default:
		__cute_unreachable();
	}
}

static bool
cute_assess_cmp_int_set(const struct cute_assess * assess,
                        union cute_assess_member   value)
{
	cute_assess_assert_intern(assess);
	cute_assess_assert_set_intern(&assess->xpct_val.set);

	unsigned int c;

	switch (assess->oper) {
	case CUTE_ASSESS_INSET_OPER:
		for (c = 0; c < assess->xpct_val.set.count; c++)
			if (value == assess->xpct_val.set.items[c].i)
				return true;

		return false;

	case CUTE_ASSESS_NOT_INSET_OPER:
		for (c = 0; c < assess->xpct_val.set.count; c++)
			if (value == assess->xpct_val.set.items[c].i)
				return false;

		return true;

	default:
		__cute_unreachable();
	}
}









void
cute_check_ptr(const void * a, enum cute_oper oper, const void * b)

void
cute_check_str(const char * a, enum cute_oper oper, const char * b)

void
cute_check_mem(const void * a, enum cute_oper oper, const void * b, size_t size)
#endif
