#ifndef _CUTE_UTIL_H
#define _CUTE_UTIL_H

#include "cute/cute.h"
#include <time.h>
#include <stdio.h>
#include <regex.h>
#include <errno.h>

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

#define cute_error(_format, ...) \
	fprintf(stderr, \
	        "%s: " _format, \
	        program_invocation_short_name, \
	        ## __VA_ARGS__)

extern void __cute_noreturn
cute_fail_assert(const char * message,
                 const char * file,
                 int          line,
                 const char * func);

extern int
cute_close_stdio(FILE * stdio);

/******************************************************************************
 * Memory allocation wrapping.
 ******************************************************************************/

extern void *
cute_malloc(size_t size);

extern void *
cute_realloc(void * ptr, size_t size);

extern void
cute_free(void * ptr);

/******************************************************************************
 * String utilities
 ******************************************************************************/

extern char *
cute_toupper(const char * string, size_t max_size);

extern char *
cute_dup(const char * string);

extern char *
cute_asprintf(const char * format, ...) __attribute__((format(printf, 1, 2)));

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

#define cute_text_foreach(_blk, _indx, _str) \
	for (_indx = 0, _str = (_blk)->atoms[_indx].str; \
	     _indx < (_blk)->count; \
	     _str = (_blk)->atoms[++_indx].str)

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
