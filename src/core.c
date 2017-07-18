#include "core.h"
#include <cute/cute.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>

#define CUTE_TIMEOUT_MAX (3600U)
#define CUTE_NAME_MAX    (256U)

CUTE_SUITE(            cute_root_suite);
const struct core_run *core_current_run;

/******************************************************************************
 * Internal stuff
 ******************************************************************************/

static bool
core_issuite_object(const struct cute_object *object)
{
	return object->eldest && object->youngest;
}

static void
core_register_object(struct cute_object *parent, struct cute_object *child)
{
	child->parent = parent;
	child->next = NULL;

	if (!parent->eldest)
		parent->eldest = child;

	if (parent->youngest)
		parent->youngest->next = child;

	parent->youngest = child;
}

static void
core_list_recurs(const struct cute_object *object)
{
	const struct cute_object *obj = object->eldest;

	if (!obj) {
		report_indent(report_indent_depth);
		printf("%s test\n", object->name);
		return;
	}

	report_indent(report_indent_depth);
	printf("%s suite\n", object->name);

	report_indent_depth++;

	do {
		core_list_recurs(obj);

		obj = obj->next;
	} while (obj);

	report_indent(--report_indent_depth);

	printf("[total: %u]\n", ((struct cute_suite *)object)->total_count);
}

static int
core_run_recurs(struct cute_object *object)
{
	struct cute_suite  *parent = (struct cute_suite *)object->parent;
	struct cute_suite  *suite;
	struct cute_object *obj = object->eldest;
	int                 err;

	if (!obj) {
		struct cute_test *test = (struct cute_test *)object;

		err = core_current_run->spawn_test(test);
		if (!err) {
			/*
			 * When running a single test with no declared parent
			 * suite, don't update statistics.
			 */
			if (parent) {
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
			}

			report_current->show_test(test);
		}

		return err;
	}

	suite = (struct cute_suite *)object;

	report_current->show_suite_begin(suite);

	do {
		err = core_run_recurs(obj);
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
		report_current->show_suite_end((struct cute_suite *)object);

	return err;
}

static void
core_fini_test(struct cute_test *test)
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

static void
core_fini_object_recurs(struct cute_object *object)
{
	struct cute_object *obj = object->eldest;

	if (!obj) {
		core_fini_test((struct cute_test *)object);
		return;
	}

	do {
		core_fini_object_recurs(obj);

		obj = obj->next;
	} while (obj);
}

/******************************************************************************
 * Exposed API
 ******************************************************************************/

void
cute_expect_failed(const char *line, const char *file, const char *reason)
{
	assert(core_current_run && report_current);

	core_current_run->expect_failed(line, file, reason);
}

void
cute_register_test(struct cute_suite *suite, struct cute_test *test)
{
	core_register_object(&suite->object, &test->object);

	suite->total_count++;
}

int
cute_register_suite(struct cute_suite *parent, struct cute_suite *suite)
{
	if (!suite->total_count)
		return -ENODATA;

	core_register_object(&parent->object, &suite->object);

	parent->total_count += suite->total_count;

	return 0;
}

void
cute_list(const struct cute_object *object)
{
	report_indent_depth = 0;

	core_list_recurs(object);
}

struct cute_object *
cute_find(struct cute_object *object, const char *name)
{
	struct cute_object *obj;

	if (!strncmp(object->name, name, CUTE_NAME_MAX))
		return object;

	obj = object->eldest;
	while (obj) {
		struct cute_object *o;

		o = cute_find(obj, name);
		if (o)
			return o;

		obj = obj->next;
	}

	return NULL;
}

int
cute_run(struct cute_object *object)
{
	int err;

	if (!core_current_run || !report_current)
		return -EPERM;

	err = core_run_recurs(object);

	if (core_issuite_object(object))
		report_current->show_footer((struct cute_suite *)object, err);
	else
		report_current->show_footer(NULL, err);

	core_fini_object_recurs(object);

	core_current_run->fini_run();

	return err;
}

/******************************************************************************
 * Plug aNd Play handling
 ******************************************************************************/

extern char __start_cute_tests;
extern char __stop_cute_tests;
extern char __start_cute_suites;
extern char __stop_cute_suites;

static struct cute_suite *
pnp_object_parent(struct cute_object *object)
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
pnp_suite_index(const struct cute_suite *suite)
{
	return ((unsigned long)suite - (unsigned long)&__start_cute_suites) /
	       sizeof(*suite);
}

static int
pnp_register(const char *root_suite_name)
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
		parent = pnp_object_parent(&test->object);
		if (!parent)
			return -errno;

		cute_register_test(parent, test);

		if (parent != &cute_root_suite) {
			suites[pnp_suite_index(parent)] = parent;
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

			parent = pnp_object_parent(&suites[s]->object);
			if (!parent)
				return -errno;

			err = cute_register_suite(parent, suites[s]);
			if (err)
				return err;

			if (parent != &cute_root_suite) {
				unsigned int p = pnp_suite_index(parent);

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

static void
pnp_usage(const char *me)
{
	fprintf(stderr,
	        "Usage: %s [OPTIONS] [SUITE_NAME|TEST_NAME]\n"
	        "    SUITE_NAME  run a single suite specified by SUITE_NAME\n"
	        "    TEST_NAME   run a single test specified by TEST_NAME\n"
	        "\n"
	        "where OPTIONS:\n"
	        "    -l|--list             list available suite tests\n"
	        "    -t|--timeout SECONDS  default per test timeout in seconds\n"
	        "    -h|--help\n",
	        me);
}

int
cute_pnp_main(int argc, char *argv[], const char *root_name)
{
	int                 err;
	bool                list = false;
	const char         *obj_name = NULL;
	struct cute_object *obj = CUTE_ROOT_OBJECT;
	unsigned long       timeout = (unsigned long)CUTE_DEFAULT_TIMEOUT;
	char               *errptr;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{ "list",     0, NULL, 'l' },
			{ "timeout",  1, NULL, 't' },
			{ "help",     0, NULL, 'h' },
			{ 0,          0, 0,    0   }
		};

		opt = getopt_long(argc, argv, "lt:h", lopts, NULL);
		if (opt < 0)
			break;

		switch (opt) {
		case 'l':
			list = true;
			break;

		case 't':
			timeout = strtoul(optarg, &errptr, 10);
			if (*optarg && !*errptr && (timeout < CUTE_TIMEOUT_MAX))
				break;

			fprintf(stderr,
			        "invalid \"%s\" timeout argument\n", optarg);
			pnp_usage(argv[0]);
			return EXIT_FAILURE;

		case 'h': /* Help message. */
			pnp_usage(argv[0]);
			return EXIT_SUCCESS;

		case '?': /* Unknown option. */
		default:
			pnp_usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	argc -= optind;

	switch (argc) {
	case 1:
		obj_name = argv[optind];
		break;

	case 0:
		break;

	default:
		fprintf(stderr, "missing argument\n");
		pnp_usage(argv[0]);
		return EXIT_FAILURE;
	}

	err = pnp_register(root_name);
	if (err) {
		fprintf(stderr, "failed to register test suites: %s\n",
		        strerror(-err));
		return EXIT_FAILURE;
	}

	if (obj_name) {
		obj = cute_find(obj, obj_name);
		if (!obj) {
			fprintf(stderr,
			        "\"%s\" testing object not found\n", obj_name);
			return EXIT_FAILURE;
		}
	}

	if (list) {
		cute_list(obj);
		return EXIT_SUCCESS;
	}

	cute_setup_text_report();

	err = cute_setup_posix_run((unsigned int)timeout);
	if (err) {
		fprintf(stderr, "failed to setup runner: %s\n", strerror(-err));
		return EXIT_FAILURE;
	}

	return cute_run(obj) ? EXIT_FAILURE : EXIT_SUCCESS;
}
