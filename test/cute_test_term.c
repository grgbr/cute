#include <cute/cute.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

CUTE_TEST(term)
{
	sleep(2);
	kill(getpid(), SIGTERM);
}

int main(void)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(5);
	if (err)
		goto out;

	err = cute_run_test(&term);

	cute_fini();

out:
	return err ? EXIT_SUCCESS : EXIT_FAILURE;
}
