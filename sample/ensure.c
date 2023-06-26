#include <cute/cute.h>
#include "../src/assess.h"

static struct cute_assess assess;

#undef cute_ensure_sint
#define cute_ensure_sint(_a, _op, _b) \
	cute_assess_sint_ ## _op (&assess, # _a, _a, # _b, _b)

int main(void)
{
	int fst;
	//int snd = 2;

	cute_ensure_sint(fst, equal, 1);

	cute_assess_desc(&assess, CUTE_ASSESS_EXPECT_DESC, stdout);
	fputc('\n', stdout);
	cute_assess_desc(&assess, CUTE_ASSESS_FOUND_DESC, stdout);
	fputc('\n', stdout);
}
