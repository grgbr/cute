#ifndef _CUTE_UTILS_H
#define _CUTE_UTILS_H

#define __unused       __attribute__((unused))
#define __used         __attribute__((used))
#define __noreturn     __attribute__((noreturn))
#define __section(...) __attribute__((section(__VA_ARGS__)))
#define __aligned(...) __attribute__((aligned(__VA_ARGS__)))

#endif
