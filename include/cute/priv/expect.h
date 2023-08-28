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

extern void
cute_expect_sched_sint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_sint_range * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_sint_range * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_sint_set * expect)
	__cute_export;

extern void
cute_expect_sched_sint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_sint_set * expect)
	__cute_export;

extern void
cute_expect_sched_sint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * retval)
	__cute_export;

extern intmax_t
cute_expect_check_sint_retval(const char * file,
                              int          line,
                              const char * function)
	__cute_export;

extern void
cute_expect_check_uint_parm(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check)
	__cute_export;

extern void
cute_expect_sched_uint_parm_equal(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_unequal(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_greater(const char *             file,
                                    int                      line,
                                    const char *             function,
                                    const char *             parm,
                                    const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_greater_equal(const char *             file,
                                          int                      line,
                                          const char *             function,
                                          const char *             parm,
                                          const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_lower(const char *             file,
                                  int                      line,
                                  const char *             function,
                                  const char *             parm,
                                  const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_lower_equal(const char *             file,
                                        int                      line,
                                        const char *             function,
                                        const char *             parm,
                                        const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_in_range(const char *                   file,
                                     int                            line,
                                     const char *                   function,
                                     const char *                   parm,
                                     const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_in_set(const char *                 file,
                                   int                          line,
                                   const char *                 function,
                                   const char *                 parm,
                                   const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_sched_uint_parm_not_in_set(const char *                 file,
                                       int                          line,
                                       const char *                 function,
                                       const char *                 parm,
                                       const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_sched_uint_retval(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * retval)
	__cute_export;

extern uintmax_t
cute_expect_check_uint_retval(const char * file,
                              int          line,
                              const char * function)
	__cute_export;

extern void
cute_expect_sched_hex_parm_equal(const char *             file,
                                 int                      line,
                                 const char *             function,
                                 const char *             parm,
                                 const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_unequal(const char *             file,
                                   int                      line,
                                   const char *             function,
                                   const char *             parm,
                                   const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_greater(const char *             file,
                                   int                      line,
                                   const char *             function,
                                   const char *             parm,
                                   const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_greater_equal(const char *             file,
                                         int                      line,
                                         const char *             function,
                                         const char *             parm,
                                         const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_lower(const char *             file,
                                 int                      line,
                                 const char *             function,
                                 const char *             parm,
                                 const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_lower_equal(const char *             file,
                                       int                      line,
                                       const char *             function,
                                       const char *             parm,
                                       const struct cute_uint * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_in_range(const char *                   file,
                                    int                            line,
                                    const char *                   function,
                                    const char *                   parm,
                                    const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_in_set(const char *                 file,
                                  int                          line,
                                  const char *                 function,
                                  const char *                 parm,
                                  const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_sched_hex_parm_not_in_set(const char *                 file,
                                      int                          line,
                                      const char *                 function,
                                      const char *                 parm,
                                      const struct cute_uint_set * expect)
	__cute_export;

extern void
cute_expect_check_flt_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check)
	__cute_export;

extern void
cute_expect_sched_flt_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_greater(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_greater_equal(const char *            file,
                                         int                     line,
                                         const char *            function,
                                         const char *            parm,
                                         const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_lower(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_lower_equal(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_flt * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_in_range(const char *                  file,
                                    int                           line,
                                    const char *                  function,
                                    const char *                  parm,
                                    const struct cute_flt_range * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_not_in_range(
	const char *                  file,
	int                           line,
	const char *                  function,
	const char *                  parm,
	const struct cute_flt_range * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_flt_set * expect)
	__cute_export;

extern void
cute_expect_sched_flt_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_flt_set * expect)
	__cute_export;

extern void
cute_expect_sched_flt_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * retval)
	__cute_export;

extern long double
cute_expect_check_flt_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

extern void
cute_expect_check_str_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_str * check)
	__cute_export;

extern void
cute_expect_sched_str_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_begin(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_begin(const char *            file,
                                     int                     line,
                                     const char *            function,
                                     const char *            parm,
                                     const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_end(const char *            file,
                               int                     line,
                               const char *            function,
                               const char *            parm,
                               const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_end(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_contain(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_contain(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_str * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_str_set * expect)
	__cute_export;

extern void
cute_expect_sched_str_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_str_set * expect)
	__cute_export;

extern void
cute_expect_sched_str_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_str * retval)
	__cute_export;

extern char *
cute_expect_check_str_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

extern void
cute_expect_check_ptr_parm(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_ptr * check)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_equal(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_unequal(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_greater(const char *            file,
                                   int                     line,
                                   const char *            function,
                                   const char *            parm,
                                   const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_greater_equal(const char *            file,
                                         int                     line,
                                         const char *            function,
                                         const char *            parm,
                                         const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_lower(const char *            file,
                                 int                     line,
                                 const char *            function,
                                 const char *            parm,
                                 const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_lower_equal(const char *            file,
                                       int                     line,
                                       const char *            function,
                                       const char *            parm,
                                       const struct cute_ptr * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_in_range(const char *                  file,
                                    int                           line,
                                    const char *                  function,
                                    const char *                  parm,
                                    const struct cute_ptr_range * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_not_in_range(
	const char *                   file,
	int                            line,
	const char *                   function,
	const char *                   parm,
	const struct cute_ptr_range * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_in_set(const char *                file,
                                  int                         line,
                                  const char *                function,
                                  const char *                parm,
                                  const struct cute_ptr_set * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_parm_not_in_set(const char *                file,
                                      int                         line,
                                      const char *                function,
                                      const char *                parm,
                                      const struct cute_ptr_set * expect)
	__cute_export;

extern void
cute_expect_sched_ptr_retval(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_ptr * retval)
	__cute_export;

extern void *
cute_expect_check_ptr_retval(const char * file,
                             int          line,
                             const char * function)
	__cute_export;

#endif /* _CUTE_PRIV_EXPECT_H */
