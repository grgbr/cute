#include "core.h"
#include <cute/cute.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include <errno.h>

#define CUTE_TIMEOUT_MAX (3600U)
#define CUTE_NAME_MAX    (256U)

const struct core_run *core_current_run;

/******************************************************************************
 * Internal stuff
 ******************************************************************************/

static bool
core_issuite_object(const struct cute_object *object)
{
	return object->eldest && object->youngest;
}

static bool
core_isobject_registered(const struct cute_object *object)
{
	return object->registered;
}

static void
core_start_timing(struct cute_object *object)
{
	assert(object);

	clock_gettime(CLOCK_MONOTONIC, &object->start_time);
}

static void
core_stop_timing(struct cute_object *object)
{
	struct timespec stop;

	clock_gettime(CLOCK_MONOTONIC, &stop);

	stop.tv_sec -= object->start_time.tv_sec;
	stop.tv_nsec -= object->start_time.tv_nsec;
	if (stop.tv_nsec < 0) {
		stop.tv_sec--;
		stop.tv_nsec += 1000000000;
	}

	object->usecs = (stop.tv_sec * 1000000ULL) + (stop.tv_nsec / 1000ULL);
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

	child->registered = true;
}

static void
core_list_recurs(const struct cute_object *object)
{
	const struct cute_object *obj = object->eldest;

	if (!obj) {
		text_start_line(report_indent_depth, "%s test\n", object->name);
		return;
	}

	text_start_line(report_indent_depth, "%s suite (#%u)\n",
	                object->name,
	                ((struct cute_suite *)object)->total_count);
	report_indent_depth++;

	do {
		core_list_recurs(obj);

		obj = obj->next;
	} while (obj);

	report_indent_depth--;
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

		core_start_timing(object);

		err = core_current_run->spawn_test(test);

		core_stop_timing(object);

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

			if (report_current->show_test)
				report_current->show_test(test);
		}

		return err;
	}

	suite = (struct cute_suite *)object;

	core_start_timing(object);

	if (report_current->show_suite_begin)
		report_current->show_suite_begin(suite);

	do {
		err = core_run_recurs(obj);

		if (err)
			break;

		obj = obj->next;
	} while (obj);

	core_stop_timing(object);

	if (parent) {
		parent->success_count += suite->success_count;
		parent->failure_count += suite->failure_count;
		parent->error_count += suite->error_count;
		parent->skipped_count += suite->skipped_count;
	}

	if (!err && report_current->show_suite_end)
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

int
cute_register_test(struct cute_suite *suite, struct cute_test *test)
{
	if (core_isobject_registered(&test->object))
		return -EPERM;

	core_register_object(&suite->object, &test->object);

	do {
		suite->total_count++;

		suite = (struct cute_suite *)suite->object.parent;
	} while (suite && core_isobject_registered(&suite->object));

	return 0;
}

int
cute_register_suite(struct cute_suite *parent, struct cute_suite *suite)
{
	unsigned int cnt = suite->total_count;

	if (!cnt)
		return -ENODATA;

	if (core_isobject_registered(&suite->object))
		return -EPERM;

	core_register_object(&parent->object, &suite->object);

	do {
		parent->total_count += cnt;

		parent = (struct cute_suite *)parent->object.parent;
	} while (parent && core_isobject_registered(&parent->object));

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

static CUTE_SUITE(     pnp_root_suite);

static struct cute_suite *
pnp_object_parent(struct cute_object *object)
{
	struct cute_suite *parent = (struct cute_suite *)object->parent;

	if (!parent || (parent == &pnp_root_suite))
		return &pnp_root_suite;

	if ((parent >= (struct cute_suite *)&__start_cute_suites) &&
	    (parent < (struct cute_suite *)&__stop_cute_suites))
		return parent;

	errno = EFAULT;
	return NULL;
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
	int                 ret;
	struct cute_suite  *parent;
	unsigned int        cnt;

	pnp_root_suite.object.name = root_suite_name;
	pnp_root_suite.object.registered = true;

	nr = ((unsigned long)&__stop_cute_suites -
	      (unsigned long)&__start_cute_suites) / sizeof(*suites[0]);

	suites = calloc(nr, sizeof(suites[0]));
	if (!suites)
		return -ENOMEM;

	test = (struct cute_test *)&__start_cute_tests;
	if (test >= (struct cute_test *)&__stop_cute_tests) {
		/* No tests were declared. */
		ret = -ENODATA;
		goto ret;
	}

	do {
		parent = pnp_object_parent(&test->object);
		if (!parent) {
			ret = -errno;
			goto ret;
		}

		cute_register_test(parent, test);

		if (parent != &pnp_root_suite)
			suites[pnp_suite_index(parent)] = parent;

		test++;
	} while (test < (struct cute_test *)&__stop_cute_tests);

	cnt = nr;
	while (cnt) {
		unsigned int s;

		for (s = 0; s < nr; s++) {
			if (!suites[s])
				continue;

			parent = pnp_object_parent(&suites[s]->object);
			if (!parent) {
				ret = -errno;
				goto ret;
			}

			ret = cute_register_suite(parent, suites[s]);
			if (ret)
				goto ret;

			if (parent != &pnp_root_suite &&
			    !core_isobject_registered(&parent->object))
				suites[pnp_suite_index(parent)] = parent;

			suites[s] = NULL;
			cnt--;
		}
	}

	ret = 0;

ret:
	free(suites);

	return ret;
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
	        "    -x|--xunit            output xunit compliant xml\n"
	        "    -h|--help\n",
	        me);
}

int
cute_pnp_main(int argc, char *argv[], const char *root_name)
{
	int                 err;
	bool                list = false;
	bool                xml = false;
	const char         *obj_name = NULL;
	struct cute_object *obj = &pnp_root_suite.object;
	unsigned long       timeout = (unsigned long)CUTE_DEFAULT_TIMEOUT;
	char               *errptr;

	while (true) {
		int                        opt;
		static const struct option lopts[] = {
			{ "list",     0, NULL, 'l' },
			{ "timeout",  1, NULL, 't' },
			{ "xunit",    0, NULL, 'x' },
			{ "help",     0, NULL, 'h' },
			{ 0,          0, 0,    0   }
		};

		opt = getopt_long(argc, argv, "lt:xh", lopts, NULL);
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

		case 'x':
			xml = true;
			break;

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

	if (xml)
		cute_setup_xunit_report();
	else
		cute_setup_text_report();

	err = cute_setup_posix_run((unsigned int)timeout);
	if (err) {
		fprintf(stderr, "failed to setup runner: %s\n", strerror(-err));
		return EXIT_FAILURE;
	}

	return cute_run(obj) ? EXIT_FAILURE : EXIT_SUCCESS;
}
