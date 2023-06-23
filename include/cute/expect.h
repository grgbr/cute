#ifndef _CUTE_EXPECT_H
#define _CUTE_EXPECT_H

#define __CUTE_STRING(_expr) # _expr
#define CUTE_STRING(_expr)   __CUTE_STRING(_expr)

extern void
cute_expect_push_call(const char * file,
                      int          line,
                      const char * function) __cute_export;

#define cute_expect_call(_function) \
	cute_expect_push_call(__FILE__, __LINE__, CUTE_STRING(_function))

#define cute_ensure_call(_function) \
	cute_expect_pull_call(__FILE__, __LINE__, CUTE_STRING(_function))

#endif /* _CUTE_EXPECT_H */
