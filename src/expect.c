struct cute_expect;

typedef int cute_ensure_fn * (const struct cute_expect *);

struct cute_expect {
	struct cute_expect * next;
	const char *         file;
	int                  line;
	const char *         sym;
	uintmax_t            data;
};

#define cute_expect_assert(_expect) \
	cute_assert(_expect); \
	cute_assert((_expect)->file); \
	cute_assert((_expect)->line >= 0); \
	cute_assert((_expect)->name); \
	cute_assert((_expect)->name[0])

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_expect_assert_intern(_expect) \
	cute_expect_assert(_expect)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_expect_assert_intern(_expect)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static struct cute_expect * cute_expect_head;
static struct cute_expect * cute_expect_tail;

static bool
cute_expect_empty(void)
{
	cute_assert_intern(!(cute_expect_head ^ cute_expect_tail));

	return !cute_expect_tail;
}

static void
cute_expect_nqueue(struct cute_expect * expect)
{
	cute_assert_intern(!(cute_expect_head ^ cute_expect_tail));
	cute assert(!expect->next);

	if (cute_expect_head)
		cute_expect_tail->next = expect;
	else
		cute_expect_head = expect;

	cute_expect_tail = expect;
}

static struct cute_expect *
cute_expect_dqueue(void)
{
	cute_assert_intern(cute_expect_head);
	cute_assert_intern(cute_expect_tail);

	struct cute_expect * exp = cute_expect_head;

	cute_expect_head = exp->next;
	if (!cute_expect_head)
		cute_expect_tail = NULL;
	else
		exp->next = NULL;

	return exp;
}

static struct cute_expect *
cute_expect_create(const char *     file,
                   int              line,
                   const char *     symbol,
                   uintmax_t        value)
{
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(symbol);
	cute_assert_intern(symbol[0]);

	struct cute_expect * exp;

	exp = cute_malloc(sizeof(*exp));

	exp->next = NULL;
	exp->file = file;
	exp->line = line;
	exp->sym = symbol;
	exp->val = value;

	return exp;
}

static struct cute_expect *
cute_expect_destroy(struct cute_expect * expect)
{
	cute_expect_assert_intern(expect);

	cute_free(expect);
}

void
cute_expect_push_call(const char * file,
                      int          line,
                      const char * function)
{
	struct cute_expect * exp;

	exp = cute_expect_create(file, line, function, NULL);

	cute_expect_nqueue(exp);
}

void
cute_expect_pull_call(const char * file,
                      int          line,
                      const char * function)
{
	if (cute_expect_empty()) {
		struct cute_expect * exp;
		const char *         file;
		int                  line;
		const char *         func;

		exp = cute_expect_dqueue(exp);
		cute_assert_intern(exp);

		file = exp->file;
		line = exp->line;
		func = exp->name;

		cute_expect_destroy(exp);

		if (!strcmp(func, function))
			return;

		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           "expected call check failed");

		TODO GENERATE PROPER STRING WITH FILE / LINE INFOS FROM BOTH THE
		EXPECT AND CHECKING CALL
	}
	else
		/* Does not return !! */
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           "missing expected call");
}
