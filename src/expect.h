#ifndef _CUTE_INTERN_EXPECT_H
#define _CUTE_INTERN_EXPECT_H

#include "assess.h"

/* Types of mock expectations */
enum cute_expect_type {
	CUTE_EXPECT_CALL_TYPE = 0,
	CUTE_EXPECT_PARM_TYPE,
	CUTE_EXPECT_RET_TYPE,
	CUTE_EXPECT_TYPE_NR
};

struct cute_expect {
	struct cute_assess    super;
	struct cute_expect *  next;
	enum cute_expect_type xpct_type;
	const char *          xpct_file;
	int                   xpct_line;
	const char *          xpct_func;
	enum cute_expect_type got_type;
};

struct cute_expect_parm {
	struct cute_expect super;
	const char *       xpct_parm;
	uintmax_t          xpct_val;
	const char *       chk_parm;
	uintmax_t          chk_val;
};

struct cute_expect_retval {
	struct cute_expect super;
	intmax_t           code;
};

extern int
cute_expect_release(struct cute_assess * assess);

#endif /* _CUTE_INTERN_EXPECT_H */
