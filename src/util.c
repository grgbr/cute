#include "common.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

void
cute_fail_assert(const char * message,
                 const char * file,
                 int          line,
                 const char * func)
{
	fprintf(cute_iodir_stderr,
	        "\n%s: assertion failed @ %s:%d in %s(): '%s'.\n",
	        program_invocation_short_name,
	        file,
	        line,
	        func,
	        message);
	abort();
}

int
cute_close_stdio(FILE * stdio)
{
	cute_assert_intern(stdio);

	const char * msg;
	int          err;

	if (fflush(stdio)) {
		msg = "flushing";
		goto err;
	}

	if (fsync(fileno(stdio))) {
		if (errno != EINVAL) {
			msg = "syncing";
			goto err;
		}
	}

	if (fclose(stdio)) {
		err = errno;
		msg = "closing";
		goto log;
	}

	return 0;

err:
	err = errno;
	fclose(stdio);
log:
	cute_error("%s file failed: %s (%d).\n", msg, strerror(err), err);

	return -err;
}

/******************************************************************************
 * Memory allocation wrapping.
 ******************************************************************************/

void *
cute_malloc(size_t size)
{
	cute_assert_intern(size);

	void * ptr;

	extern void * __libc_malloc(size_t);
	ptr = __libc_malloc(size);
	if (!ptr)
		exit(EXIT_FAILURE);

	return ptr;
}

void *
cute_realloc(void * ptr, size_t size)
{
	cute_assert_intern(size);

	extern void * __libc_realloc(void *, size_t);
	ptr = __libc_realloc(ptr, size);
	if (!ptr)
		exit(EXIT_FAILURE);

	return ptr;
}

void
cute_free(void * ptr)
{
	extern void __libc_free(void *);
	__libc_free(ptr);
}

/******************************************************************************
 * System primitive wrappers
 ******************************************************************************/

int
cute_lock_init(pthread_mutex_t * lock)
{
	cute_assert_intern(lock);

	pthread_mutexattr_t attr;
	int                 ret;

	ret = pthread_mutexattr_init(&attr);
	if (ret)
		return -ret;

	ret = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
	if (ret) {
		cute_assert_intern(ret != EINVAL);
		cute_assert_intern(ret != EPERM);
		goto destroy;
	}
	
	ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (ret) {
		cute_assert_intern(ret != EINVAL);
		goto destroy;
	}

	ret = pthread_mutex_init(lock, &attr);
	cute_assert_intern(ret != EPERM);
	cute_assert_intern(ret != EINVAL);

destroy:
	pthread_mutexattr_destroy(&attr);

	return -ret;
}

int
cute_thr_create(pthread_t *       hndl,
                const sigset_t  * mask,
                void *         (* routine)(void *),
                void *            arg)
{
	sigset_t old;
	int      err __cute_unused;
	int      ret;

	err = pthread_sigmask(SIG_SETMASK, mask, &old);
	cute_assert_intern(!err);

	ret = pthread_create(hndl, NULL, routine, arg);
	cute_assert_intern(ret != EINVAL);
	cute_assert_intern(ret != EPERM);

	err = pthread_sigmask(SIG_SETMASK, &old, NULL);
	cute_assert_intern(!err);

	return -ret;
}

/******************************************************************************
 * String utilities
 ******************************************************************************/

static char * __attribute__((format(printf, 1, 0)))
cute_vasprintf(const char * format, va_list args)
{
	va_list tmp;
	char *  str;
	int     len;

	va_copy(tmp, args);

#define CUTE_ASPRINTF_ERROR_MSG  "cannot create formatted string"
	str = cute_malloc(sizeof(CUTE_ASPRINTF_ERROR_MSG));

	len = vsnprintf(str, sizeof(CUTE_ASPRINTF_ERROR_MSG), format, tmp);
	if (len < 0)
		goto err;

	if (len < (int)sizeof(CUTE_ASPRINTF_ERROR_MSG))
		return str;

	cute_free(str);
	str = cute_malloc((size_t)len + 1);

	len = vsnprintf(str, (size_t)len + 1, format, args);
	if (len < 0)
		goto err;

	return str;

err:
	memcpy(str,
	       CUTE_ASPRINTF_ERROR_MSG,
	       sizeof(CUTE_ASPRINTF_ERROR_MSG));

	return str;
}

/******************************************************************************
 * Text block utility
 ******************************************************************************/

void
cute_text_enroll(struct cute_text_block * block, char * string, bool own)
{
	cute_assert_intern(block);
	cute_assert_intern(block->nr);
	cute_assert_intern(block->count < block->nr);
	cute_assert_intern(string);
	cute_assert_intern(string[0]);

	struct cute_text_atom * atom = &block->atoms[block->count++];

	atom->str = string;
	atom->own = own;
}

void
cute_text_asprintf(struct cute_text_block * block, const char * format, ...)
{
	cute_assert_intern(block);
	cute_assert_intern(block->nr);
	cute_assert_intern(block->count < block->nr);
	cute_assert_intern(format);
	cute_assert_intern(format[0]);

	struct cute_text_atom * atom = &block->atoms[block->count++];
	va_list                 args;
	char *                  str;

	va_start(args, format);
	str = cute_vasprintf(format, args);
	va_end(args);

	atom->str = str;
	atom->own = true;
}

struct cute_text_block *
cute_text_create(unsigned int nr)
{
	cute_assert_intern(nr);

	struct cute_text_block * blk;

	blk = cute_malloc(sizeof(*blk) + (nr * sizeof(blk->atoms[0])));
	blk->count = 0;
	blk->nr = nr;

	return blk;
}

void
cute_text_destroy(struct cute_text_block * block)
{
	cute_assert_intern(block);
	cute_assert_intern(block);
	cute_assert_intern(block->nr);
	cute_assert_intern(block->count <= block->nr);

	unsigned int cnt;

	for (cnt = 0; cnt < block->nr; cnt++) {
		const struct cute_text_atom * atom = &block->atoms[cnt];

		if (atom->own)
			cute_free(atom->str);
	}

	cute_free(block);
}

/******************************************************************************
 * POSIX extended regular expressions
 ******************************************************************************/

bool
cute_regex_match(regex_t * regex, const char * string)
{
       return !regexec(regex, string, 0, NULL, 0);
}

int
cute_regex_init(regex_t * regex, const char * expr, bool icase)
{
	int flags = REG_EXTENDED |
	            (icase ? REG_ICASE : 0) |
	            REG_NOSUB |
	            REG_NEWLINE;
	int err;

	err = regcomp(regex, expr, flags);
	if (err) {
		char * msg;
		size_t sz;

		sz = regerror(err, regex, NULL, 0);
		msg = cute_malloc(sz);
		regerror(err, regex, msg, sz);
		cute_error("invalid regular expression: %s.\n", msg);
		cute_free(msg);

		return -EINVAL;
	}

	return 0;
}

void
cute_regex_fini(regex_t * regex)
{
	regfree(regex);
}

/******************************************************************************
 * Timestamps handling.
 ******************************************************************************/

void
cute_diff_tspec(struct timespec *       result,
                const struct timespec * begin,
                const struct timespec * end)
{
	long nsec = end->tv_nsec - begin->tv_nsec;

	if (nsec < 0) {
		result->tv_sec = end->tv_sec - begin->tv_sec - 1;
		result->tv_nsec = nsec + 1000000000L;
	}
	else {
		result->tv_sec = end->tv_sec - begin->tv_sec;
		result->tv_nsec = nsec;
	}
}

const char *
cute_time_string(char                    string[CUTE_TIME_STRING_SZ],
                 const struct timespec * time)
{
	cute_assert_intern(string);
	cute_assert_intern(time);

	struct tm tmp;

	if (gmtime_r(&time->tv_sec, &tmp))
		if (strftime(string, CUTE_TIME_STRING_SZ, "%FT%T", &tmp) ==
		    (CUTE_TIME_STRING_SZ - 1))
			return string;

	return NULL;
}

/******************************************************************************
 * NULL terminated pointer array iterator handling
 ******************************************************************************/

static struct cute_iter *
cute_iter_alloc(unsigned int nr)
{
	cute_assert_intern(nr);

	struct cute_iter * iter;

	iter = cute_malloc(sizeof(*iter));

	iter->next = 0;
	iter->nr = nr;

	return iter;
}

struct cute_iter *
cute_iter_create(void * const * items, unsigned int nr, void * data)
{
	cute_assert_intern(items);
	cute_assert_intern(nr);

	struct cute_iter * iter;

	iter = cute_iter_alloc(nr);

	iter->rwitems = items;
	iter->rwdata = data;

	return iter;
}

struct cute_iter *
cute_iter_const_create(const void * const * items,
                       unsigned int         nr,
                       const void *         data)
{
	cute_assert_intern(items);
	cute_assert_intern(nr);

	struct cute_iter * iter;

	iter = cute_iter_alloc(nr);

	iter->roitems = items;
	iter->rodata = data;

	return iter;
}

/******************************************************************************
 * Pointer based stack / LIFO handling
 ******************************************************************************/

void
cute_stack_push(struct cute_stack * stack, void * item)
{
	cute_stack_assert_intern(stack);

	if (stack->count == stack->nr) {
		unsigned int nr = 2 * stack->nr;

		stack->items = cute_realloc(stack->items,
		                            nr * sizeof(*stack->items));
		stack->nr = nr;
	}

	stack->items[stack->count++] = item;
}

void
cute_stack_init(struct cute_stack * stack)
{
	cute_assert_intern(stack);

	stack->items = cute_malloc(CUTE_STACK_INIT_NR * sizeof(*stack->items));
	stack->count = 0;
	stack->nr = CUTE_STACK_INIT_NR;
}
