#include <cute/cute.h>
#include <stdbool.h>
#include <stdlib.h>

CUTE_TEST(freeze)
{
	while (true)
		;
}

int main(void)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(2);
	if (err)
		return EXIT_FAILURE;

	return cute_run_test(&freeze) ? EXIT_SUCCESS : EXIT_FAILURE;
}
