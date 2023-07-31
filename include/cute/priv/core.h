#ifndef _CUTE_PRIV_CORE_H
#define _CUTE_PRIV_CORE_H

#define __cute_export \
	__attribute__((visibility("default")))

#define __cute_noreturn \
	__attribute__((noreturn))

struct cute_base;
struct cute_iter;
struct cute_run;

typedef void (cute_test_fn)(void);


#define CUTE_NULL_FIXTURE ((cute_test_fn *)0)
#define CUTE_INHR_FIXTURE ((cute_test_fn *)1)

typedef struct cute_iter * (cute_iter_fn)(const struct cute_base * base);

typedef struct cute_run * (cute_run_fn)(const struct cute_base * base,
                                        struct cute_run *        parent);

struct cute_ops {
	cute_iter_fn * iter;
	cute_run_fn *  run;
};

struct cute_base {
	const struct cute_ops * ops;
	cute_test_fn *          setup;
	cute_test_fn *          teardown;
	const char *            name;
	unsigned int            tmout;
	const char *            file;
	int                     line;
};

#define CUTE_BASE_INIT(_name, _ops, _setup, _teardown, _tmout) \
	{ \
		.ops      = _ops, \
		.setup    = _setup, \
		.teardown = _teardown, \
		.name     = _name, \
		.tmout    = _tmout, \
		.file     = __FILE__, \
		.line     = __LINE__ \
	}

extern const struct cute_ops cute_test_ops __cute_export;

struct cute_test {
	struct cute_base super;
	cute_test_fn *   exec;
};

#define CUTE_TEST_INIT(_name, _exec, _setup, _teardown, _tmout) \
	{ \
		.super = CUTE_BASE_INIT(_name, \
		                        &cute_test_ops, \
		                        _setup, \
		                        _teardown, \
		                        _tmout), \
		.exec  = _exec, \
	}

extern const struct cute_ops cute_suite_ops __cute_export;

struct cute_suite {
	struct cute_base                 super;
	unsigned int                     nr;
	const struct cute_base * const * tests;
};

#define CUTE_SUITE_INIT(_name, _tests, _setup, _teardown, _tmout) \
	{ \
		.super = CUTE_BASE_INIT(_name, \
		                        &cute_suite_ops, \
		                        _setup, \
		                        _teardown, \
		                        _tmout), \
		.nr    = sizeof(_tests) / sizeof((_tests)[0]), \
		.tests = _tests \
	}

extern int
cute_main(int                       argc,
          char * const              argv[],
          const struct cute_suite * suite,
          const char *              package,
          const char *              version) __cute_export;

extern void
_cute_skip(const char * reason,
           const char * file,
           int          line,
           const char * function) __cute_export;

extern void
_cute_fail(const char * reason,
           const char * file,
           int          line,
           const char * function) __cute_export;

#endif /* _CUTE_PRIVATE_CORE_H */
