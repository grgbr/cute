#include "expect.h"

typedef void (cute_run_desc_fn)(const struct cute_run * run,
                                FILE *                  stdio,
                                int                     indent,
                                const char *            normal,
                                const char *            emphasis,
                                const char *            regular);

/******************************************************************************
 * Mock expectation generic handling
 ******************************************************************************/

enum cute_expect_type {
	CUTE_EXPECT_CALL_TYPE = 0,
	CUTE_EXPECT_PARM_TYPE,
	CUTE_EXPECT_RET_TYPE,
	CUTE_EXPECT_TYPE_NR,

};

#define cute_expect_assert_type(_type) \
	cute_assert((_type) >= 0); \
	cute_assert((_type) < CUTE_EXPECT_TYPE_NR)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_expect_assert_type_intern(_type) \
	cute_expect_assert_type(_type)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_expect_assert_type_intern(_type)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

struct cute_expect {
	struct cute_expect *  next;
	enum cute_expect_type xpct_type;
	const char *          xpct_file;
	int                   xpct_line;
	const char *          xpct_func;
	enum cute_expect_type got_type;
};

#define cute_expect_assert(_expect) \
	cute_assert(_expect); \
	cute_expect_assert_type((_expect)->xpct_type); \
	cute_assert((_expect)->xpct_file); \
	cute_assert((_expect)->xpct_file[0]); \
	cute_assert((_expect)->xpct_line >= 0); \
	cute_assert((_expect)->xpct_func); \
	cute_assert((_expect)->xpct_func[0])

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

	struct cute_expect * xpct = cute_expect_head;

	cute_expect_head = xpct->next;
	if (!cute_expect_head)
		cute_expect_tail = NULL;
	else
		xpct->next = NULL;

	return xpct;
}

static void
cute_expect_desc_inval_type(const struct cute_run * run,
                            FILE *                  stdio,
                            int                     indent,
                            const char *            normal,
                            const char *            emphasis,
                            const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$s    expect: %8$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %9$s:%10$s\n"
	        "%3$*2$s    caller: %11$s\n"
	        "%3$*2$s    actual: %12$s%13$s\n"
	        "%14$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        cute_expect_type_label(xpct->xpct_type),
	        run->file, run->line,
	        run->func,
	        emphasis, cute_expect_type_label(xpct->got_type),
	        regular);
}

static void
cute_expect_desc_inval_call(const struct cute_run * run,
                            FILE *                  stdio,
                            int                     indent,
                            const char *            normal,
                            const char *            emphasis,
                            const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %8$s:%9$s\n"
	        "%3$*2$s    caller: %10$s%11$s\n"
	        "%12$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        run->file, run->line,
	        emphasis, run->func,
	        regular);
}

static void
cute_expect_desc_missing(const struct cute_run * run,
                         FILE *                  stdio,
                         int                     indent,
                         const char *            normal,
                         const char *            emphasis __cute_unused,
                         const char *            regular)
{
	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: ??\n"
	        "%3$*2$s    caller: ??\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%8$s",
	        normal,
	        indent, "",
	        run->why,
	        run->file, run->line,
	        run->func,
	        regular);
}

static struct cute_expect *
cute_expect_pull(enum cute_expect_type type,
                 const char *          file,
                 int                   line,
                 const char *          function)
{
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(function);
	cute_assert_intern(function[0]);

	struct cute_expect * xpct = NULL;
	cute_run_desc_fn *   desc;
	const char *         why;

	if (!cute_expect_empty()) {
		xpct = cute_expect_dqueue();
		cute_expect_assert_intern(xpct);

		if (type != xpct->type) {
			desc = cute_expect_desc_inval_type;
			why = "unexpected mock expectation type";
			goto break;
		}

		if (strcmp(function, xpct->xpct_func)) {
			desc = cute_expect_desc_inval_call;
			why = "unexpected mock expectation caller;
			goto break;
		}

		return xpct;
	}

	desc = cute_expect_desc_missing,
	why = "missing mock expectation";

break:
	if (xpct) {
		xpct->got_type = type;
		cute_curr_run->xpct = xpct;
	}

	/* Does not return !! */
	cute_break(CUTE_FAIL_ISSUE, file, line, function, desc, why);
}

static struct cute_expect *
cute_expect_create(enum cute_expect_type type,
                   const char *          file;
                   int                   line;
                   const char *          function,
                   size_t                size)
{
	cute_expect_assert_type_intern(type);
	cute_assert_intern(file);
	cute_assert_intern(file[0]);
	cute_assert_intern(line >= 0);
	cute_assert_intern(function);
	cute_assert_intern(function[0]);
	cute_assert_intern(size > sizeof(struct cute_expect));

	struct cute_expect * xpct;

	xpct = cute_malloc(size);

	xpct->next = NULL;
	xpct->xpct_type = type;
	xpct->xpct_file = file;
	xpct->xpct_line = line
	xpct->xpct_func = function;
	xpct->got_type = CUTE_EXPECT_TYPE_NR;

	return xpct;
}

static struct cute_expect *
cute_expect_destroy(struct cute_expect * expect)
{
	cute_expect_assert_intern(expect);

	cute_free(expect);
}

/******************************************************************************
 * Mock call expectation handling
 ******************************************************************************/

void
cute_expect_check_call(const char * file, int line, const char * function)
{
	struct cute_expect * xpct;

	xpct = cute_expect_pull(CUTE_EXPECT_CALL_TYPE, file, line, function);

	cute_expect_destroy(xpct);
}

void
cute_expect_push_call(const char * file, int line, const char * function)
{
	struct cute_expect * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_CALL_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));

	cute_expect_nqueue(xpct);
}

/******************************************************************************
 * Mock parameter expectation handling
 ******************************************************************************/

struct cute_expect_parm {
	struct cute_expect super;
	const char *       xpct_parm;
	uintmax_t          xpct_val;
	const char *       chk_parm;
	uintmax_t          chk_val;
};

static void
cute_expect_desc_inval_parm(const struct cute_run * run,
                            FILE *                  stdio,
                            int                     indent,
                            const char *            normal,
                            const char *            emphasis,
                            const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$s    expect: %8$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %9$s:%10$s\n"
	        "%3$*2$s    caller: %11$s\n"
	        "%3$*2$s    actual: %12$s%13$s\n"
	        "%14$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        ((struct cute_expect_parm *)xpct)->xpct_parm,
	        run->file, run->line,
	        run->func,
	        emphasis, ((struct cute_expect_parm *)xpct)->chk_parm,
	        regular);
}

static void
cute_expect_desc_inval_parm_val(const struct cute_run * run,
                                FILE *                  stdio,
                                int                     indent,
                                const char *            normal,
                                const char *            emphasis,
                                const char *            regular)
{
	const struct cute_expect * xpct = run->xpct;

	fprintf(stdio,
	        "%1$s"
	        "%3$*2$sreason: %4$s\n"
	        "%3$*2$swanted:\n"
	        "%3$*2$s    source: %5$s:%6$s\n"
	        "%3$*2$s    caller: %7$s\n"
	        "%3$*2$s    expect: %8$s\n"
	        "%3$*2$sfound:\n"
	        "%3$*2$s    source: %9$s:%10$s\n"
	        "%3$*2$s    caller: %11$s\n"
	        "%3$*2$s    actual: %12$s%13$s\n"
	        "%14$s",
	        normal,
	        indent, "",
	        run->why,
	        xpct->xpct_file, xpct->xpct_line,
	        xpct->xpct_func,
	        ((struct cute_expect_parm *)xpct)->xpct_parm,
	        run->file, run->line,
	        run->func,
	        emphasis, ((struct cute_expect_parm *)xpct)->chk_parm,
	        regular);
}

void
cute_expect_check_parm(const char * file,
                       int          line,
                       const char * function,
                       const char * parameter,
                       uintmax_t    value)
{
	cute_assert_intern(parameter);
	cute_assert_intern(parameter[0]);

	struct cute_expect_parm * xpct = (struct cute_expect_parm *)
	                                 cute_expect_pull(CUTE_EXPECT_PARM_TYPE,
	                                                  file,
	                                                  line,
	                                                  function);
	cute_run_desc_fn *        desc;
	const char *              why;


	cute_assert_intern(xpct->xpct_parm);
	cute_assert_intern(xpct->xpct_parm[0]);
	cute_assert_intern(xpct->chk_parm);
	cute_assert_intern(xpct->chk_parm[0]);

	if (strcmp(parameter, xpct->xpct_parm)) {
		desc = cute_expect_desc_inval_parm;
		why = "unexpected mock expectation parameter name;
		goto break;
	}

	if (value != parm->xpct_val) {
		xpct->chk_val = value;
		desc = cute_expect_desc_inval_parm_val;
		why = "unexpected mock expectation parameter value;
		goto break;
	}

	cute_expect_destroy(xpct);

	return;

break:
	xpct->got_type = type;
	xpct->chk_parm = parameter;
	cute_curr_run->xpct = &xpct->super;

	/* Does not return !! */
	cute_break(CUTE_FAIL_ISSUE, file, line, function, desc, why);
}

void
cute_expect_push_parm(const char * file,
                      int          line,
                      const char * function,
                      const char * parameter,
                      uintmax_t    value)
{
	cute_assert_intern(parameter);
	cute_assert_intern(parameter[0]);

	struct cute_expect_parm * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_PARM_TYPE;
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));
	xpct->xpct_parm = parameter;
	xpct->xpct_val = value;

	cute_expect_nqueue(&xpct->super);
}

/******************************************************************************
 * Mock return expectation handling
 ******************************************************************************/

struct cute_expect_return {
	struct cute_expect super;
	uintmax_t          retval;
};

uintmax_t
cute_expect_check_retval(const char * file, int line, const char * function)
{
	struct cute_expect_return * xpct =
		(struct cute_expect_return *)
		cute_expect_pull(CUTE_EXPECT_RET_TYPE, file, line, function);
	int                         ret = xpct->retval;

	cute_expect_destroy(&xpct->super);

	return xpct->retval;
}

void
cute_expect_push_retval(const char * file,
                        int          line,
                        const char * function,
                        uintmax_t    retval)
{
	struct cute_expect_return * xpct;

	xpct = cute_expect_create(CUTE_EXPECT_RET_TYPE,
	                          file,
	                          line,
	                          function,
	                          sizeof(*xpct));
	xpct->retval = retval;

	cute_expect_nqueue(xpct);
}
