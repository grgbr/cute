#include <cute/cute.h>
#include <stdlib.h>

CUTE_TEST(fail)
{
	cute_ensure(0 == 1);
}

int main(void)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(CUTE_DEFAULT_TIMEOUT);
	if (err)
		goto out;

	err = cute_run_test(&fail);

	cute_fini();

out:
	return err ? EXIT_SUCCESS : EXIT_FAILURE;
}
