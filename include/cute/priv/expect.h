/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_PRIV_EXPECT_H
#define _CUTE_PRIV_EXPECT_H

#include <cute/priv/core.h>
#include <cute/types.h>
#include <stdbool.h>
#include <setjmp.h>

extern sigjmp_buf cute_expect_assert_env __cute_export;
extern bool       cute_expect_assert __cute_export;

extern void
cute_expect_fail_assert(const char * file, int line, const char * function)
	__cute_noreturn __cute_export;

extern void
cute_expect_sched_call(const char * file, int line, const char * function)
	__cute_export;

extern void
cute_expect_check_call(const char * file, int line, const char * function)
	__cute_export;

extern void
cute_expect_check_sint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check)
	__cute_export;

extern void
cute_expect_sched_sint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_sint * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_sint * expect)
	__cute_export;

#endif /* _CUTE_PRIV_EXPECT_H */
