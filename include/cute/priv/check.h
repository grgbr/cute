/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_PRIV_CHECK_H
#define _CUTE_PRIV_CHECK_H

#include <cute/priv/core.h>
#include <cute/types.h>
#include <stdbool.h>

extern void
__cute_check_assert(bool         fail,
                    const char * file,
                    int          line,
                    const char * function,
                    const char * expr) __cute_export;

extern void
cute_check_sint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_sint * check,
                        const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_sint * check,
                              const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_sint * check,
                      const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_sint * check,
                            const struct cute_sint * expect) __cute_export;

extern void
cute_check_sint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_sint *       check,
                         const struct cute_sint_range * expect) __cute_export;

extern void
cute_check_sint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_sint *       check,
                             const struct cute_sint_range * expect)
	__cute_export;

extern void
cute_check_sint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_sint *     check,
                       const struct cute_sint_set * expect) __cute_export;

extern void
cute_check_sint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_sint *     check,
                           const struct cute_sint_set * expect) __cute_export;

extern void
cute_check_uint_equal(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_unequal(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_greater(const char *             file,
                        int                      line,
                        const char *             function,
                        const struct cute_uint * check,
                        const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_greater_equal(const char *             file,
                              int                      line,
                              const char *             function,
                              const struct cute_uint * check,
                              const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_lower(const char *             file,
                      int                      line,
                      const char *             function,
                      const struct cute_uint * check,
                      const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_lower_equal(const char *             file,
                            int                      line,
                            const char *             function,
                            const struct cute_uint * check,
                            const struct cute_uint * expect) __cute_export;

extern void
cute_check_uint_in_range(const char *                   file,
                         int                            line,
                         const char *                   function,
                         const struct cute_uint *       check,
                         const struct cute_uint_range * expect) __cute_export;

extern void
cute_check_uint_not_in_range(const char *                   file,
                             int                            line,
                             const char *                   function,
                             const struct cute_uint *       check,
                             const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_check_uint_in_set(const char *                 file,
                       int                          line,
                       const char *                 function,
                       const struct cute_uint *     check,
                       const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_uint_not_in_set(const char *                 file,
                           int                          line,
                           const char *                 function,
                           const struct cute_uint *     check,
                           const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_hex_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_uint * check,
                     const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_unequal(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_uint * check,
                       const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_greater(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_uint * check,
                       const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_greater_equal(const char *             file,
                             int                      line,
                             const char *             function,
                             const struct cute_uint * check,
                             const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_lower(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_uint * check,
                     const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_lower_equal(const char *             file,
                           int                      line,
                           const char *             function,
                           const struct cute_uint * check,
                           const struct cute_uint * expect) __cute_export;

extern void
cute_check_hex_in_range(const char *                   file,
                        int                            line,
                        const char *                   function,
                        const struct cute_uint *       check,
                        const struct cute_uint_range * expect) __cute_export;

extern void
cute_check_hex_not_in_range(const char *                   file,
                            int                            line,
                            const char *                   function,
                            const struct cute_uint *       check,
                            const struct cute_uint_range * expect)
	__cute_export;

extern void
cute_check_hex_in_set(const char *                 file,
                      int                          line,
                      const char *                 function,
                      const struct cute_uint *     check,
                      const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_hex_not_in_set(const char *                 file,
                          int                          line,
                          const char *                 function,
                          const struct cute_uint *     check,
                          const struct cute_uint_set * expect) __cute_export;

extern void
cute_check_flt_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_unequal(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_greater(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_flt * check,
                       const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_greater_equal(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_flt * check,
                             const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_lower(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_flt * check,
                     const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_lower_equal(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_flt * check,
                           const struct cute_flt * expect) __cute_export;

extern void
cute_check_flt_in_range(const char *                  file,
                        int                           line,
                        const char *                  function,
                        const struct cute_flt *       check,
                        const struct cute_flt_range * expect) __cute_export;

extern void
cute_check_flt_not_in_range(const char *                  file,
                            int                           line,
                            const char *                  function,
                            const struct cute_flt *       check,
                            const struct cute_flt_range * expect)
	__cute_export;

extern void
cute_check_flt_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_flt *     check,
                      const struct cute_flt_set * expect) __cute_export;

extern void
cute_check_flt_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_flt *     check,
                          const struct cute_flt_set * expect) __cute_export;

extern void
cute_check_str_equal(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_str * check,
                     const struct cute_str * expect) __cute_export;

extern void
cute_check_str_unequal(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_begin(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_str * check,
                     const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_begin(const char *            file,
                         int                     line,
                         const char *            function,
                         const struct cute_str * check,
                         const struct cute_str * expect) __cute_export;

extern void
cute_check_str_end(const char *            file,
                   int                     line,
                   const char *            function,
                   const struct cute_str * check,
                   const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_end(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_contain(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_str * check,
                       const struct cute_str * expect) __cute_export;

extern void
cute_check_str_not_contain(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_str * check,
                           const struct cute_str * expect) __cute_export;

extern void
cute_check_str_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_str *     check,
                      const struct cute_str_set * expect) __cute_export;

extern void
cute_check_str_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_str *     check,
                          const struct cute_str_set * expect) __cute_export;

extern void
cute_check_ptr_equal(const char *             file,
                     int                      line,
                     const char *             function,
                     const struct cute_ptr *  check,
                     const struct cute_ptr *  expect) __cute_export;

extern void
cute_check_ptr_unequal(const char *             file,
                       int                      line,
                       const char *             function,
                       const struct cute_ptr *  check,
                       const struct cute_ptr *  expect) __cute_export;

extern void
cute_check_ptr_greater(const char *            file,
                       int                     line,
                       const char *            function,
                       const struct cute_ptr * check,
                       const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_greater_equal(const char *            file,
                             int                     line,
                             const char *            function,
                             const struct cute_ptr * check,
                             const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_lower(const char *            file,
                     int                     line,
                     const char *            function,
                     const struct cute_ptr * check,
                     const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_lower_equal(const char *            file,
                           int                     line,
                           const char *            function,
                           const struct cute_ptr * check,
                           const struct cute_ptr * expect) __cute_export;

extern void
cute_check_ptr_in_range(const char *                  file,
                        int                           line,
                        const char *                  function,
                        const struct cute_ptr *       check,
                        const struct cute_ptr_range * expect) __cute_export;

extern void
cute_check_ptr_not_in_range(const char *                  file,
                            int                           line,
                            const char *                  function,
                            const struct cute_ptr *       check,
                            const struct cute_ptr_range * expect)
	__cute_export;

extern void
cute_check_ptr_in_set(const char *                file,
                      int                         line,
                      const char *                function,
                      const struct cute_ptr *     check,
                      const struct cute_ptr_set * expect) __cute_export;

extern void
cute_check_ptr_not_in_set(const char *                file,
                          int                         line,
                          const char *                function,
                          const struct cute_ptr *     check,
                          const struct cute_ptr_set * expect) __cute_export;

#endif /* _CUTE_PRIV_CHECK_H */
