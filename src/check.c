#include "cute/check.h"
#include "run.h"
#include "assess.h"

void
__cute_check_assert(bool         fail,
                    const char * file,
                    int          line,
                    const char * function,
                    const char * expr)
{
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(expr);
	cute_assert(expr[0]);

	if (!fail)
		return;

	cute_assess_build_expr(&cute_curr_run->assess, NULL, expr);

	cute_break(CUTE_FAIL_ISSUE,
	           file,
	           line,
	           function,
	           "assertion check failed");
}

/******************************************************************************
 * Signed integer handling
 ******************************************************************************/

typedef bool (cute_assess_sint_fn)(struct cute_assess *     assess,
                                   const struct cute_sint * check,
                                   const struct cute_sint * expect);

static void
cute_check_sint_value_assess(cute_assess_sint_fn *    assess,
                             const char *             file,
                             int                      line,
                             const char *             function,
                             const struct cute_sint * check,
                             const struct cute_sint * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer value check failed");
}

#define CUTE_CHECK_DEFINE_SINT(_name, _assess) \
	void \
	_name(const char *             file, \
	      int                      line, \
	      const char *             function, \
	      const struct cute_sint * check, \
	      const struct cute_sint * expect) \
	{ \
		cute_check_sint_value_assess(_assess, \
		                             file, \
		                             line, \
		                             function, \
		                             check, \
		                             expect); \
	}

CUTE_CHECK_DEFINE_SINT(cute_check_sint_equal,
                       cute_assess_sint_equal)
CUTE_CHECK_DEFINE_SINT(cute_check_sint_unequal,
                       cute_assess_sint_unequal)
CUTE_CHECK_DEFINE_SINT(cute_check_sint_greater,
                       cute_assess_sint_greater)
CUTE_CHECK_DEFINE_SINT(cute_check_sint_greater_equal,
                       cute_assess_sint_greater_equal)
CUTE_CHECK_DEFINE_SINT(cute_check_sint_lower,
                       cute_assess_sint_lower)
CUTE_CHECK_DEFINE_SINT(cute_check_sint_lower_equal,
                       cute_assess_sint_lower_equal)

typedef bool (cute_assess_sint_range_fn)(struct cute_assess *           assess,
                                         const struct cute_sint *       check,
                                         const struct cute_sint_range * expect);

static void
cute_check_sint_range_assess(cute_assess_sint_range_fn *    assess,
                             const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_sint *       check,
                             const struct cute_sint_range * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer range check failed");
}

#define CUTE_CHECK_DEFINE_SINT_RANGE(_name, _assess) \
	void \
	_name(const char *                   file, \
	      int                            line, \
	      const char *                   function, \
	      const struct cute_sint *       check, \
	      const struct cute_sint_range * expect) \
	{ \
		cute_check_sint_range_assess(_assess, \
		                             file, \
		                             line, \
		                             function, \
		                             check, \
		                             expect); \
	}

CUTE_CHECK_DEFINE_SINT_RANGE(cute_check_sint_in_range,
                             cute_assess_sint_in_range)
CUTE_CHECK_DEFINE_SINT_RANGE(cute_check_sint_not_in_range,
                             cute_assess_sint_not_in_range)

typedef bool (cute_assess_sint_set_fn)(struct cute_assess *         assess,
                                       const struct cute_sint *     check,
                                       const struct cute_sint_set * expect);

static void
cute_check_sint_set_assess(cute_assess_sint_set_fn *    assess,
                           const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_sint *     check,
                           const struct cute_sint_set * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "signed integer set check failed");
}

#define CUTE_CHECK_DEFINE_SINT_SET(_name, _assess) \
	void \
	_name(const char *                 file, \
	      int                          line, \
	      const char *                 function, \
	      const struct cute_sint *     check, \
	      const struct cute_sint_set * expect) \
	{ \
		cute_check_sint_set_assess(_assess, \
		                           file, \
		                           line, \
		                           function, \
		                           check, \
		                           expect); \
	}

CUTE_CHECK_DEFINE_SINT_SET(cute_check_sint_in_set,
                           cute_assess_sint_in_set)
CUTE_CHECK_DEFINE_SINT_SET(cute_check_sint_not_in_set,
                           cute_assess_sint_not_in_set)

/******************************************************************************
 * Unsigned integer handling
 ******************************************************************************/

typedef bool (cute_assess_uint_fn)(struct cute_assess *     assess,
                                   const struct cute_uint * check,
                                   const struct cute_uint * expect);

static void
cute_check_uint_value_assess(cute_assess_uint_fn *    assess,
                             const char *             file,
                             int                      line,
                             const char *             function,
                             const struct cute_uint * check,
                             const struct cute_uint * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer value check failed");
}

#define CUTE_CHECK_DEFINE_UINT(_name, _assess) \
	void \
	_name(const char *             file, \
	      int                      line, \
	      const char *             function, \
	      const struct cute_uint * check, \
	      const struct cute_uint * expect) \
	{ \
		cute_check_uint_value_assess(_assess, \
		                             file, \
		                             line, \
		                             function, \
		                             check, \
		                             expect); \
	}

CUTE_CHECK_DEFINE_UINT(cute_check_uint_equal,
                       cute_assess_uint_equal)
CUTE_CHECK_DEFINE_UINT(cute_check_uint_unequal,
                       cute_assess_uint_unequal)
CUTE_CHECK_DEFINE_UINT(cute_check_uint_greater,
                       cute_assess_uint_greater)
CUTE_CHECK_DEFINE_UINT(cute_check_uint_greater_equal,
                       cute_assess_uint_greater_equal)
CUTE_CHECK_DEFINE_UINT(cute_check_uint_lower,
                       cute_assess_uint_lower)
CUTE_CHECK_DEFINE_UINT(cute_check_uint_lower_equal,
                       cute_assess_uint_lower_equal)

typedef bool (cute_assess_uint_range_fn)(struct cute_assess *           assess,
                                         const struct cute_uint *       check,
                                         const struct cute_uint_range * expect);

static void
cute_check_uint_range_assess(cute_assess_uint_range_fn *    assess,
                             const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_uint *       check,
                             const struct cute_uint_range * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer range check failed");
}

#define CUTE_CHECK_DEFINE_UINT_RANGE(_name, _assess) \
	void \
	_name(const char *                   file, \
	      int                            line, \
	      const char *                   function, \
	      const struct cute_uint *       check, \
	      const struct cute_uint_range * expect) \
	{ \
		cute_check_uint_range_assess(_assess, \
		                             file, \
		                             line, \
		                             function, \
		                             check, \
		                             expect); \
	}

CUTE_CHECK_DEFINE_UINT_RANGE(cute_check_uint_in_range,
                             cute_assess_uint_in_range)
CUTE_CHECK_DEFINE_UINT_RANGE(cute_check_uint_not_in_range,
                             cute_assess_uint_not_in_range)

typedef bool (cute_assess_uint_set_fn)(struct cute_assess *         assess,
                                       const struct cute_uint *     check,
                                       const struct cute_uint_set * expect);

static void
cute_check_uint_set_assess(cute_assess_uint_set_fn *    assess,
                           const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_uint *     check,
                           const struct cute_uint_set * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "unsigned integer set check failed");
}

#define CUTE_CHECK_DEFINE_UINT_SET(_name, _assess) \
	void \
	_name(const char *                 file, \
	      int                          line, \
	      const char *                 function, \
	      const struct cute_uint *     check, \
	      const struct cute_uint_set * expect) \
	{ \
		cute_check_uint_set_assess(_assess, \
		                           file, \
		                           line, \
		                           function, \
		                           check, \
		                           expect); \
	}

CUTE_CHECK_DEFINE_UINT_SET(cute_check_uint_in_set,
                           cute_assess_uint_in_set)
CUTE_CHECK_DEFINE_UINT_SET(cute_check_uint_not_in_set,
                           cute_assess_uint_not_in_set)

/******************************************************************************
 * Floating point number handling
 ******************************************************************************/

typedef bool (cute_assess_flt_fn)(struct cute_assess *    assess,
                                  const struct cute_flt * check,
                                  const struct cute_flt * expect);

static void
cute_check_flt_value_assess(cute_assess_flt_fn *    assess,
                            const char *            file,
                            int                     line,
                            const char *            function,
                            const struct cute_flt * check,
                            const struct cute_flt * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point value check failed");
}

#define CUTE_CHECK_DEFINE_FLT(_name, _assess) \
	void \
	_name(const char *            file, \
	      int                     line, \
	      const char *            function, \
	      const struct cute_flt * check, \
	      const struct cute_flt * expect) \
	{ \
		cute_check_flt_value_assess(_assess, \
		                            file, \
		                            line, \
		                            function, \
		                            check, \
		                            expect); \
	}

CUTE_CHECK_DEFINE_FLT(cute_check_flt_equal,
                      cute_assess_flt_equal)
CUTE_CHECK_DEFINE_FLT(cute_check_flt_unequal,
                      cute_assess_flt_unequal)
CUTE_CHECK_DEFINE_FLT(cute_check_flt_greater,
                      cute_assess_flt_greater)
CUTE_CHECK_DEFINE_FLT(cute_check_flt_greater_equal,
                      cute_assess_flt_greater_equal)
CUTE_CHECK_DEFINE_FLT(cute_check_flt_lower,
                      cute_assess_flt_lower)
CUTE_CHECK_DEFINE_FLT(cute_check_flt_lower_equal,
                      cute_assess_flt_lower_equal)

typedef bool (cute_assess_flt_range_fn)(struct cute_assess *          assess,
                                        const struct cute_flt *       check,
                                        const struct cute_flt_range * expect);

static void
cute_check_flt_range_assess(cute_assess_flt_range_fn *     assess,
                             const char *                  file,
                             int                           line,
                             const char *                  function,
                             const struct cute_flt *       check,
                             const struct cute_flt_range * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_assert(expect->min <= expect->max);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point number range check failed");
}

#define CUTE_CHECK_DEFINE_FLT_RANGE(_name, _assess) \
	void \
	_name(const char *                  file, \
	      int                           line, \
	      const char *                  function, \
	      const struct cute_flt *       check, \
	      const struct cute_flt_range * expect) \
	{ \
		cute_check_flt_range_assess(_assess, \
		                            file, \
		                            line, \
		                            function, \
		                            check, \
		                            expect); \
	}

CUTE_CHECK_DEFINE_FLT_RANGE(cute_check_flt_in_range,
                            cute_assess_flt_in_range)
CUTE_CHECK_DEFINE_FLT_RANGE(cute_check_flt_not_in_range,
                            cute_assess_flt_not_in_range)

typedef bool (cute_assess_flt_set_fn)(struct cute_assess *        assess,
                                      const struct cute_flt *     check,
                                      const struct cute_flt_set * expect);

static void
cute_check_flt_set_assess(cute_assess_flt_set_fn *    assess,
                          const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_flt *     check,
                          const struct cute_flt_set * expect)
{
	cute_assert(assess);
	cute_assert(file);
	cute_assert(file[0]);
	cute_assert(line >= 0);
	cute_assert(function);
	cute_assert(function[0]);
	cute_assert(check->expr);
	cute_assert(check->expr[0]);
	cute_assert(expect->expr);
	cute_assert(expect->expr[0]);
	cute_run_assert_intern(cute_curr_run);

	if (!assess(&cute_curr_run->assess, check, expect))
		cute_break(CUTE_FAIL_ISSUE,
		           file,
		           line,
		           function,
		           "floating point number set check failed");
}

#define CUTE_CHECK_DEFINE_FLT_SET(_name, _assess) \
	void \
	_name(const char *                file, \
	      int                         line, \
	      const char *                function, \
	      const struct cute_flt *     check, \
	      const struct cute_flt_set * expect) \
	{ \
		cute_check_flt_set_assess(_assess, \
		                          file, \
		                          line, \
		                          function, \
		                          check, \
		                          expect); \
	}

CUTE_CHECK_DEFINE_FLT_SET(cute_check_flt_in_set,
                          cute_assess_flt_in_set)
CUTE_CHECK_DEFINE_FLT_SET(cute_check_flt_not_in_set,
                          cute_assess_flt_not_in_set)
