#include <cute/cute.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

CUTE_SUITE(root);

CUTE_SUITE(first);

CUTE_TEST(ok00)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(ok01)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(term02)
{
	sleep(1);
	kill(getpid(), SIGINT);
}

CUTE_TEST(ok03)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(ok04)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(fail05)
{
	cute_ensure(0 == 1);
}

CUTE_TEST(ok06)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(abrt07)
{
	abort();
}

CUTE_SUITE(second);

CUTE_TEST(ok10)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(ok11)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(ok12)
{
	cute_ensure(1 == 1);
}

CUTE_TEST(freeze13)
{
	while (true)
		;
}

CUTE_TEST(segv14)
{
	*((int *)0) = 2;
}

CUTE_TEST(ok15)
{
	cute_ensure(1 == 1);
}

int main(void)
{
	int err;

	cute_setup_text_report();

	cute_register_test(&first, &ok00);
	cute_register_test(&first, &ok01);
	cute_register_test(&first, &term02);
	cute_register_test(&first, &ok03);
	cute_register_test(&first, &ok04);
	cute_register_test(&first, &fail05);
	cute_register_test(&first, &ok06);
	cute_register_test(&first, &abrt07);
	cute_register_suite(&root, &first);

	cute_register_test(&second, &ok10);
	cute_register_test(&second, &ok11);
	cute_register_test(&second, &ok12);
	cute_register_test(&second, &freeze13);
	cute_register_test(&second, &segv14);
	cute_register_test(&second, &ok15);
	cute_register_suite(&root, &second);

	err = cute_setup_posix_run(2);
	if (err)
		goto out;

	err = cute_run_suite(&root);

	cute_fini();

out:
	return err ? EXIT_SUCCESS : EXIT_FAILURE;
}
