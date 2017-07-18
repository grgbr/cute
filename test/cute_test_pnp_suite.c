#include <cute/cute.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>

static CUTE_PNP_SUITE(first, NULL);

CUTE_PNP_TEST(ok00, &first)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(ok01, &first)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(term02, &first)
{
	sleep(1);
	kill(getpid(), SIGINT);
}

CUTE_PNP_TEST(ok03, &first)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(ok04, &first)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(fail05, &first)
{
	cute_ensure(0 == 1);
}

CUTE_PNP_TEST(ok06, &first)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(abrt07, &first)
{
	abort();
}

static CUTE_PNP_SUITE(second, NULL);

CUTE_PNP_TEST(ok10, &second)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(ok11, &second)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(ok12, &second)
{
	cute_ensure(1 == 1);
}

CUTE_PNP_TEST(freeze13, &second)
{
	while (true)
		;
}

CUTE_PNP_TEST(segv14, &second)
{
	*((int *)0) = 2;
}

CUTE_PNP_TEST(ok15, &second)
{
	cute_ensure(1 == 1);
}

int main(int argc, char *argv[])
{
	return cute_pnp_main(argc, argv, "root");
}
