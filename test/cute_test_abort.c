#include <cute/cute.h>
#include <stdlib.h>

CUTE_TEST(abrt)
{
	abort();
}

int main(void)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(CUTE_DEFAULT_TIMEOUT);
	if (err)
		goto out;

	err = cute_run_test(&abrt);

	cute_fini();

out:
	return err ? EXIT_SUCCESS : EXIT_FAILURE;
}
