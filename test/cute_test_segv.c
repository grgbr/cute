#include <cute/cute.h>
#include <stdlib.h>

CUTE_TEST(segv)
{
	*((int *)0) = 2;
}

int main(void)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(CUTE_DEFAULT_TIMEOUT);
	if (err)
		return EXIT_FAILURE;

	return cute_run_test(&segv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
