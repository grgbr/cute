#include "core.h"
#include <cute/cute.h>
#include <assert.h>
#include <errno.h>

const struct report *current_report;
const struct run    *current_run;

void
cute_expect_failed(const char *line, const char *file, const char *reason)
{
	assert(current_run && current_report);

	current_run->expect_failed(line, file, reason);
}

static void
cute_fini_test(struct cute_test *test)
{
	struct cute_result *res = &test->result;

	free(res->reason);
	res->reason = NULL;

	free(res->line);
	res->line = NULL;

	free(res->file);
	res->file = NULL;

	free(res->console);
	res->console = NULL;
}

int
cute_run_test(struct cute_test *test)
{
	int err;

	if (!current_run || !current_report)
		return -EPERM;

	err = current_run->spawn_test(test);

	if (!err)
		current_report->show_test(test);

	current_report->show_footer(err);

	cute_fini_test(test);

	return err;
}

void
cute_fini(void)
{
	if (current_run)
		current_run->fini_run();
}
