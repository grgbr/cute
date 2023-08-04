/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_SUITE_H
#define _CUTE_SUITE_H

#include "run.h"

struct cute_suite_run {
	struct cute_run   super;
	struct cute_stats stats;
	struct cute_stats sums;
	unsigned int      count;
	struct cute_run * subs[];
};

#endif /* _CUTE_SUITE_H */
