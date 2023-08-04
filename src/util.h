/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#ifndef _CUTE_UTIL_H
#define _CUTE_UTIL_H

#include "cute/cute.h"
#include <time.h>
#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>

#define __CUTE_STRING(__str) \
	# __str

#define CUTE_STRING(_str) \
	__CUTE_STRING(_str)

#define __cute_unused \
	__attribute__((unused))

#define cute_assert(_expr) \
	((_expr) ? (void)(0) : cute_fail_assert(# _expr, \
	                                        __FILE__, \
	                                        __LINE__, \
	                                        __func__))

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_assert_intern(_expr) \
	cute_assert(_expr)

#define __cute_unreachable() \
	cute_assert_intern(0)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_assert_intern(_expr)

#define __cute_unreachable() \
        __builtin_unreachable();

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

extern void __cute_noreturn
cute_fail_assert(const char * message,
                 const char * file,
                 int          line,
                 const char * func);

extern int
cute_close_stdio(FILE * stdio);

/******************************************************************************
 * Memory allocation wrappers
 ******************************************************************************/

extern void *
cute_malloc(size_t size);

extern void *
cute_realloc(void * ptr, size_t size);

extern void
cute_free(void * ptr);

/******************************************************************************
 * System primitive wrappers
 ******************************************************************************/

static inline void
cute_lock(pthread_mutex_t * lock)
{
	cute_assert_intern(lock);

	int err __cute_unused;

	err = pthread_mutex_lock(lock);
	cute_assert_intern(!err);
}

static inline void
cute_unlock(pthread_mutex_t * lock)
{
	cute_assert_intern(lock);

	int err __cute_unused;

	err = pthread_mutex_unlock(lock);
	cute_assert_intern(!err);
}

extern int
cute_lock_init(pthread_mutex_t * lock);

static inline void
cute_lock_fini(pthread_mutex_t * lock)
{
	cute_assert_intern(lock);

	int err __cute_unused;

	err = pthread_mutex_destroy(lock);
	cute_assert_intern(!err);
}

static inline void
cute_cond_wait(pthread_cond_t * cond, pthread_mutex_t * lock)
{
	cute_assert_intern(cond);
	cute_assert_intern(lock);

	int err __cute_unused;

	err = pthread_cond_wait(cond, lock);
	cute_assert_intern(!err);
}

static inline void
cute_cond_signal(pthread_cond_t * cond)
{
	int err __cute_unused;

	err = pthread_cond_signal(cond);
	cute_assert_intern(!err);
}

static inline int
cute_cond_init(pthread_cond_t * cond)
{
	cute_assert_intern(cond);

	return - pthread_cond_init(cond, NULL);
}

static inline void
cute_cond_fini(pthread_cond_t * cond)
{
	cute_assert_intern(cond);

	int err __cute_unused;

	err = pthread_cond_destroy(cond);
	cute_assert_intern(!err);
}

extern int
cute_thr_create(pthread_t *       hndl,
                const sigset_t  * mask,
                void *         (* routine)(void *),
                void *            arg);

static inline void
cute_thr_join(pthread_t thr, void ** retval)
{
	int err __cute_unused;

	err = pthread_join(thr, retval);
	cute_assert_intern(!err);
}

static inline void
cute_thr_kill(pthread_t thr, int sig)
{
	cute_assert_intern(sig);
	cute_assert_intern(sig <= NSIG || (sig >= SIGRTMIN && sig <= SIGRTMAX));

	int err __cute_unused;

	err = pthread_kill(thr, sig);
	cute_assert_intern(!err);
}

static inline int
cute_dup(int old_fd)
{
	int fd;

	fd = dup(old_fd);
	if (fd >= 0)
		return fd;

	cute_assert_intern(errno != EBADF);
	cute_assert_intern(errno != EINVAL);
	cute_assert_intern(errno != EBUSY);

	return -errno;
}

static inline int
cute_dup2(int old_fd, int new_fd)
{
	int fd;

	fd = dup2(old_fd, new_fd);
	if (fd >= 0)
		return fd;

	cute_assert_intern(errno != EBADF);
	cute_assert_intern(errno != EINVAL);
	cute_assert_intern(errno != EBUSY);

	return -errno;
}

static inline int
cute_pipe(int pipe_fds[2])
{
	if (!pipe(pipe_fds))
		return 0;

	cute_assert_intern(errno != EFAULT);
	cute_assert_intern(errno != EINVAL);

	return -errno;
}

static inline void
cute_setup_nonblock(int fd)
{
	int err __cute_unused;

	err = fcntl(fd, F_SETFL, O_NONBLOCK);
	cute_assert_intern(!err);
}

/******************************************************************************
 * String utilities
 ******************************************************************************/

#if 0

extern char *
cute_toupper(const char * string, size_t max_size);

extern char *
cute_strdup(const char * string);

extern char *
cute_asprintf(const char * format, ...) __attribute__((format(printf, 1, 2)));

#endif

/******************************************************************************
 * Text block utility
 ******************************************************************************/

struct cute_text_atom {
	char * str;
	bool   own;
};

struct cute_text_block {
	unsigned int          count;
	unsigned int          nr;
	struct cute_text_atom atoms[];
};

#define cute_text_foreach(_blk, _indx, _atom) \
	for ((_indx) = 0, (_atom) = &(_blk)->atoms[_indx]; \
	     _indx < (_blk)->count; \
	     (_atom) = &(_blk)->atoms[++_indx])

#define CUTE_TEXT_YIELD (true)
#define CUTE_TEXT_LEASE (false)

extern void
cute_text_enroll(struct cute_text_block * block, char * string, bool own);

extern void
cute_text_asprintf(struct cute_text_block * block, const char * format, ...)
	__attribute__((format(printf, 2, 3)));

extern struct cute_text_block *
cute_text_create(unsigned int nr);

extern void
cute_text_destroy(struct cute_text_block * block);

/******************************************************************************
 * POSIX extended regular expressions
 ******************************************************************************/

extern bool
cute_regex_match(regex_t * regex, const char * string);

extern int
cute_regex_init(regex_t * regex, const char * expr, bool icase);

extern void
cute_regex_fini(regex_t * regex);

/******************************************************************************
 * Timestamps handling.
 ******************************************************************************/

#if defined(CONFIG_CUTE_INTERN_ASSERT)

static inline void
cute_gettime(struct timespec * tspec)
{
	cute_assert_intern(tspec);

	cute_assert_intern(!clock_gettime(CLOCK_REALTIME, tspec));
}

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline void
cute_gettime(struct timespec * tspec)
{
	clock_gettime(CLOCK_REALTIME, tspec);
}

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

extern void
cute_diff_tspec(struct timespec *       result,
                const struct timespec * begin,
                const struct timespec * end);

#define CUTE_TIME_STRING_SZ (20U)

extern const char *
cute_time_string(char                    string[CUTE_TIME_STRING_SZ],
                 const struct timespec * time);

/******************************************************************************
 * NULL terminated pointer array iterator handling
 ******************************************************************************/

struct cute_iter {
	unsigned int                 next;
	unsigned int                 nr;
	union {
		const void * const * roitems;
		void * const *       rwitems;
	};
	union {
		const void * rodata;
		void *       rwdata;
	};
};

#define cute_iter_assert(_iter) \
	cute_assert(_iter); \
	cute_assert((_iter)->nr); \
	cute_assert((_iter)->next <= (_iter)->nr); \
	cute_assert((_iter)->roitems)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_iter_assert_intern(_iter) \
	cute_iter_assert(_iter)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_iter_assert_intern(_iter)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline const void *
cute_iter_const_data(const struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);

	return iter->rodata;
}

static inline void *
cute_iter_data(const struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);

	return iter->rwdata;
}

static inline bool
cute_iter_end(const struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);

	return iter->next == iter->nr;
}

static inline const void *
cute_iter_const_next(struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);
	cute_assert_intern(iter->next < iter->nr);

	return iter->roitems[iter->next++];
}

static inline void *
cute_iter_next(struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);
	cute_assert_intern(iter->next < iter->nr);

	return iter->rwitems[iter->next++];
}

extern struct cute_iter *
cute_iter_create(void * const * items, unsigned int nr, void * data);

extern struct cute_iter *
cute_iter_const_create(const void * const * items,
                       unsigned int         nr,
                       const void *         data);

static inline void
cute_iter_destroy(struct cute_iter * iter)
{
	cute_iter_assert_intern(iter);

	cute_free(iter);
}

/******************************************************************************
 * Pointer based stack / LIFO handling
 ******************************************************************************/

struct cute_stack {
	unsigned int count;
	unsigned int nr;
	void **      items;
};

#define CUTE_STACK_INIT_NR (4U)

#define cute_stack_assert(_stack) \
	cute_assert(_stack); \
	cute_assert((_stack)->nr); \
	cute_assert((_stack)->count <= (_stack)->nr); \
	cute_assert((_stack)->items)

#if defined(CONFIG_CUTE_INTERN_ASSERT)

#define cute_stack_assert_intern(_stack) \
	cute_stack_assert(_stack)

#else  /* !defined(CONFIG_CUTE_INTERN_ASSERT) */

#define cute_stack_assert_intern(_stack)

#endif /* defined(CONFIG_CUTE_INTERN_ASSERT) */

static inline unsigned int
cute_stack_count(const struct cute_stack * stack)
{
	cute_stack_assert_intern(stack);

	return stack->count;
}

static inline void *
cute_stack_peek(const struct cute_stack * stack)
{
	cute_stack_assert_intern(stack);
	cute_assert_intern(stack->count);

	return stack->items[stack->count - 1];
}

extern void
cute_stack_push(struct cute_stack * stack, void * item);

static inline void *
cute_stack_pop(struct cute_stack * stack)
{
	cute_stack_assert_intern(stack);
	cute_assert_intern(stack->count);

	return stack->items[--stack->count];
}

extern void
cute_stack_init(struct cute_stack * stack);

static inline void
cute_stack_fini(struct cute_stack * stack)
{
	cute_stack_assert_intern(stack);

	cute_free(stack->items);
}

#endif /* _CUTE_UTIL_H */
