/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_IODIR_H
#define _CUTE_IODIR_H

#include <stdio.h>
#include <stdbool.h>

extern FILE * cute_iodir_stdout;
extern FILE * cute_iodir_stderr;

struct cute_iodir_atom;

struct cute_iodir_block {
	struct cute_iodir_atom * head;
	struct cute_iodir_atom * tail;
};

extern bool
cute_iodir_is_block_busy(const struct cute_iodir_block * block);

extern void
cute_iodir_print_block(FILE *                          stdio,
                       int                             depth,
                       const struct cute_iodir_block * block);

extern void
cute_iodir_init_block(struct cute_iodir_block * block);

extern void
cute_iodir_fini_block(struct cute_iodir_block * block);

extern void
cute_iodir_redirect(struct cute_iodir_block * stdout_block,
                    struct cute_iodir_block * stderr_block);

extern void
cute_iodir_restore(void);

extern int
cute_iodir_init(void);

extern void
cute_iodir_fini(void);

#endif /* _CUTE_IODIR_H */
