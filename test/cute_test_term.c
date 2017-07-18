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
		return EXIT_FAILURE;

	return cute_run_test(&term) ? EXIT_SUCCESS : EXIT_FAILURE;
}
