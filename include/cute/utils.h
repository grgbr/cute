#ifndef _CUTE_UTILS_H
#define _CUTE_UTILS_H

#define __unused       __attribute__((unused))
#define __used         __attribute__((used))
#define __noreturn     __attribute__((noreturn))
#define __section(...) __attribute__((section(__VA_ARGS__)))
#define __aligned(...) __attribute__((aligned(__VA_ARGS__)))

/*
 * Prevent reordering of top-level statements for optimization purposes (see
 * -fno-toplevel-reorder gcc option.
 */
#define __noreorder    __attribute__((no_reorder))

#define _CUTE_STR(_symbol) # _symbol
#define CUTE_STR(_symbol)  _CUTE_STR(_symbol)

/*
 * __noreorder is needed to ensure gcc optimization preserves order in
 * which symbols were defined in source file, providing test writer with
 * full control over tests execution order.
 */
#define __cute_pnp(...) \
	__section(__VA_ARGS__) __aligned(__SIZEOF_POINTER__) __used __noreorder

#endif
