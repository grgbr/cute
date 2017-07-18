#include <cute/cute.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

CUTE_TEST(success)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(failure)
{
	cute_ensure(0 == 1);
}

CUTE_TEST(abrt)
{
	abort();
}

CUTE_TEST(assertion)
{
	assert(0);
}

CUTE_TEST(freeze)
{
	while (true)
		;
}

CUTE_TEST(segv)
{
	*((int *)0) = 2;
}

CUTE_TEST(term)
{
	sleep(1);
	kill(getpid(), SIGINT);
}

CUTE_SUITE(root);

int main(void)
{
	int err;

	cute_setup_text_report();

	cute_register_test(&root, &success);
	cute_register_test(&root, &failure);
	cute_register_test(&root, &abrt);
	cute_register_test(&root, &assertion);
	cute_register_test(&root, &freeze);
	cute_register_test(&root, &segv);
	cute_register_test(&root, &term);

	err = cute_setup_posix_run(2);
	if (err)
		return EXIT_FAILURE;

	return cute_run_suite(&root) ? EXIT_SUCCESS : EXIT_FAILURE;
}
