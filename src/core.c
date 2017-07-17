#include "core.h"
#include <cute/cute.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

const struct report *current_report;
const struct run    *current_run;

static void
cute_register_object(struct cute_object *parent, struct cute_object *child)
{
	child->parent = parent;
	child->next = NULL;

	if (!parent->eldest)
		parent->eldest = child;

	if (parent->youngest)
		parent->youngest->next = child;

	parent->youngest = child;
}

void
cute_register_test(struct cute_suite *suite, struct cute_test *test)
{
	cute_register_object(&suite->object, &test->object);

	suite->total_count++;
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
cute_register_suite(struct cute_suite *parent, struct cute_suite *suite)
{
	if (!suite->total_count)
		return -ENODATA;

	cute_register_object(&parent->object, &suite->object);

	parent->total_count += suite->total_count;

	return 0;
}

static int
cute_run_object_recurs(struct cute_object *object)
{
	struct cute_suite  *parent = (struct cute_suite *)object->parent;
	struct cute_suite  *suite;
	struct cute_object *obj = object->eldest;
	int                 err;

	if (!obj) {
		struct cute_test *test = (struct cute_test *)object;

		err = current_run->spawn_test(test);
		if (!err) {
			switch (test->result.state) {
			case CUTE_SUCCESS_STATE:
				parent->success_count++;
				break;

			case CUTE_FAILURE_STATE:
				parent->failure_count++;
				break;

			case CUTE_ERROR_STATE:
				parent->error_count++;
				break;

			default:
				return -EPROTO;
			}

			current_report->show_test(test);
		}

		return err;
	}

	suite = (struct cute_suite *)object;

	current_report->show_suite_begin(suite);

	do {
		err = cute_run_object_recurs(obj);
		if (err)
			break;

		obj = obj->next;
	} while (obj);

	if (parent) {
		parent->success_count += suite->success_count;
		parent->failure_count += suite->failure_count;
		parent->error_count += suite->error_count;
		parent->skipped_count += suite->skipped_count;
	}

	if (!err)
		current_report->show_suite_end((struct cute_suite *)object);

	return err;
}

static void
cute_fini_object_recurs(struct cute_object *object)
{
	struct cute_object *obj = object->eldest;

	if (!obj) {
		cute_fini_test((struct cute_test *)object);
		return;
	}

	do {
		cute_fini_object_recurs(obj);

		obj = obj->next;
	} while (obj);
}

int
cute_run_suite(struct cute_suite *suite)
{
	int err;

	if (!current_run || !current_report)
		return -EPERM;

	err = cute_run_object_recurs(&suite->object);

	current_report->show_footer(suite, err);

	cute_fini_object_recurs(&suite->object);

	return err;
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

	current_report->show_footer(NULL, err);

	cute_fini_test(test);

	return err;
}

void
cute_expect_failed(const char *line, const char *file, const char *reason)
{
	assert(current_run && current_report);

	current_run->expect_failed(line, file, reason);
}

void
cute_fini(void)
{
	if (current_run)
		current_run->fini_run();
}

static CUTE_SUITE(cute_root_suite);

extern char __start_cute_tests;
extern char __stop_cute_tests;
extern char __start_cute_suites;
extern char __stop_cute_suites;

static struct cute_suite *
cute_pnp_object_parent(struct cute_object *object)
{
	struct cute_suite *parent = (struct cute_suite *)object->parent;

	if (parent) {
		if ((parent < (struct cute_suite *)&__start_cute_suites) ||
		    (parent >= (struct cute_suite *)&__stop_cute_suites)) {
			errno = EFAULT;
			return NULL;
		}
	}
	else
		parent = &cute_root_suite;

	return parent;
}

static unsigned long
cute_pnp_suite_index(const struct cute_suite *suite)
{
	return ((unsigned long)suite - (unsigned long)&__start_cute_suites) /
	       sizeof(*suite);
}

static int
cute_register_pnp(const char *root_suite_name)
{
	unsigned long       nr;
	struct cute_suite **suites;
	struct cute_test   *test;
	struct cute_suite  *parent;
	unsigned int        cnt = 0;

	cute_root_suite.object.name = root_suite_name;

	nr = ((unsigned long)&__stop_cute_suites -
	      (unsigned long)&__start_cute_suites) / sizeof(*suites[0]);

	suites = calloc(nr, sizeof(suites[0]));
	if (!suites)
		return -ENOMEM;

	test = (struct cute_test *)&__start_cute_tests;
	while (test < (struct cute_test *)&__stop_cute_tests) {
		parent = cute_pnp_object_parent(&test->object);
		if (!parent)
			return -errno;

		cute_register_test(parent, test);

		if (parent != &cute_root_suite) {
			suites[cute_pnp_suite_index(parent)] = parent;
			cnt++;
		}

		test++;
	}

	while (cnt) {
		unsigned int s;

		for (s = 0, cnt = 0; s < nr; s++) {
			int err;

			if (!suites[s])
				continue;

			parent = cute_pnp_object_parent(&suites[s]->object);
			if (!parent)
				return -errno;

			err = cute_register_suite(parent, suites[s]);
			if (err)
				return err;

			if (parent != &cute_root_suite) {
				unsigned int p = cute_pnp_suite_index(parent);

				if (!suites[p]) {
					suites[p] = parent;
					cnt++;
				}
			}

			suites[s] = NULL;
		}
	}

	free(suites);

	return 0;
}

int
cute_main(const char *root_suite_name)
{
	int err;

	cute_setup_text_report();

	err = cute_setup_posix_run(CUTE_DEFAULT_TIMEOUT);
	if (err)
		return err;

	err = cute_register_pnp(root_suite_name);
	if (err)
		goto fini;

	err = cute_run_suite(&cute_root_suite);

fini:
	cute_fini();

	return err;
}
