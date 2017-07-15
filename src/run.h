#ifndef _CUTE_RUN_H
#define _CUTE_RUN_H

struct cute_reporter;

typedef int (cute_spawn_test_fn)(struct cute_test *test);

typedef void (cute_fini_fn)(void);

struct cute_run {
	cute_spawn_test_fn         *spawn_test;
	cute_fini_fn               *fini;
	const struct cute_reporter *reporter;
};



#endif
