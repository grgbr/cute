/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "report.h"
#include "suite.h"
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <locale.h>
#include <unistd.h>

#define CUTE_CONFIG_CONS_REPORTS \
	((unsigned int) \
	 (CUTE_CONFIG_SILENT_REPORT | \
	  CUTE_CONFIG_TERSE_REPORT | \
	  CUTE_CONFIG_VERB_REPORT))

struct cute_config * cute_the_config;

static void
cute_config_enable_debug(struct cute_config * config)
{
	cute_assert_intern(config);

	config->debug = true;
}

static int
cute_config_parse_tty(const char * arg, enum cute_config_tty * tty)
{
	cute_assert_intern(arg);
	cute_assert_intern(arg[0]);
	cute_assert_intern(tty);

	if (!strcmp(arg, "on")) {
		*tty = CUTE_CONFIG_TRUE_TTY;
		return 0;
	}
	else if (!strcmp(arg, "off")) {
		*tty = CUTE_CONFIG_FALSE_TTY;
		return 0;
	}

	cute_error("invalid '%s' tty option.\n", arg);

	return -EINVAL;
}

static bool
cute_config_cons_enabled(const struct cute_config * config,
                         enum cute_config_report    report)
{
	cute_assert_intern(config);
	cute_config_assert_report_intern(report);

	if (config->reports & (CUTE_CONFIG_CONS_REPORTS & ~report))
		return true;

	if ((config->reports & CUTE_CONFIG_TAP_REPORT) && !config->tap_path)
		return true;

	if ((config->reports & CUTE_CONFIG_XML_REPORT) && !config->xml_path)
		return true;

	return false;
}

static bool
cute_config_report_enabled(const struct cute_config * config, unsigned int mask)
{
	cute_assert_intern(config);
	cute_config_assert_report_intern(mask);

	return !!(config->reports & mask);
}

static int
cute_config_enable_cons(struct cute_config *    config,
                        enum cute_config_report report,
                        enum cute_config_tty    tty,
                        const char *            arg)
{
	cute_assert_intern(config);
	cute_config_assert_report_intern(report);
	cute_config_assert_tty_intern(tty);

	int err;

	if (arg) {
		err = cute_config_parse_tty(arg, &tty);
		if (err)
			return err;
	}

	if (cute_config_cons_enabled(config, report)) {
		cute_error("multiple console reporter not allowed.\n");
		return -EBUSY;
	}

	config->tty = tty;
	config->reports |= report;

	return 0;
}

static int
cute_config_enable_silent(struct cute_config * config)
{
	return cute_config_enable_cons(config,
	                               CUTE_CONFIG_SILENT_REPORT,
	                               CUTE_CONFIG_FALSE_TTY,
	                               NULL);
}

static int
cute_config_enable_terse(struct cute_config * config, const char * tty)
{
	return cute_config_enable_cons(config,
	                               CUTE_CONFIG_TERSE_REPORT,
	                               CUTE_CONFIG_PROBE_TTY,
	                               tty);
}

static int
cute_config_enable_verb(struct cute_config * config, const char * tty)
{
	return cute_config_enable_cons(config,
	                               CUTE_CONFIG_VERB_REPORT,
	                               CUTE_CONFIG_PROBE_TTY,
	                               tty);
}

static int
cute_config_enable_tap(struct cute_config * config, const char * path)
{
	if (!path || !path[0] || !(strcmp(path, "-"))) {
		int err;

		err = cute_config_enable_cons(config,
		                              CUTE_CONFIG_TAP_REPORT,
		                              CUTE_CONFIG_FALSE_TTY,
		                              NULL);
		if (err)
			return err;

		config->tap_path = NULL;
	}
	else {
		config->tap_path = path;
		config->reports |= CUTE_CONFIG_TAP_REPORT;
	}

	return 0;
}

static int
cute_config_enable_xml(struct cute_config * config, const char * path)
{
	cute_assert_intern(config);

	if (!path || !path[0] || !(strcmp(path, "-"))) {
		int err;

		err = cute_config_enable_cons(config,
		                              CUTE_CONFIG_XML_REPORT,
		                              CUTE_CONFIG_FALSE_TTY,
		                              NULL);
		if (err)
			return err;

		config->xml_path = NULL;
	}
	else {
		config->xml_path = path;
		config->reports |= CUTE_CONFIG_XML_REPORT;

	}

	return 0;
}

static int
cute_config_load(struct cute_config * config)
{
	cute_config_assert_intern(config);

	int err = 0;

	if (!cute_config_cons_enabled(config, 0U))
		config->reports |= CUTE_CONFIG_TERSE_REPORT;

	if (config->reports & CUTE_CONFIG_SILENT_REPORT)
		err = cute_report_setup_silent(config);
	else if (config->reports & CUTE_CONFIG_TERSE_REPORT)
		err = cute_report_setup_terse(config);
	else if (config->reports & CUTE_CONFIG_VERB_REPORT)
		err = cute_report_setup_verb(config);

	if (err)
		return err;

	if (config->reports & CUTE_CONFIG_TAP_REPORT) {
		err = cute_report_setup_tap(config);
		if (err)
			goto reports;
	}

	if (config->reports & CUTE_CONFIG_XML_REPORT) {
		err = cute_report_setup_xml(config);
		if (err)
			goto reports;
	}

	return 0;

reports:
	cute_report_release();

	return err;
}

int
cute_init(struct cute_config * config,
          const char *         package,
          const char *         version)
{
	cute_config_assert(config);
	cute_assert(!cute_the_config);

	int err;

	setlocale(LC_ALL, "");

	err = cute_iodir_init();
	if (err)
		return err;

	cute_load_props(package, version);

	err = cute_config_load(config);
	if (err)
		goto fini;

	cute_the_config = config;

	cute_run_init_sigs();

	cute_run_nr = 0;

	return 0;

fini:
	cute_unload_props();

	cute_iodir_fini();

	return err;
}

void
cute_fini(void)
{
	cute_run_fini_sigs();

	cute_report_release();

	cute_unload_props();

	cute_the_config = NULL;

	cute_iodir_fini();
}

#define CUTE_HELP \
"Usage:\n" \
"       %1$s [<OPTIONS>] info [<NAME>]\n" \
"       Show informations about requested suite or test.\n" \
"\n" \
"       %1$s [<OPTIONS>] show [<PATTERN>]\n" \
"       List requested suites and tests.\n" \
"\n" \
"       %1$s [<OPTIONS>] run [<PATTERN>]\n" \
"       Run requested suites and / or tests.\n" \
"\n" \
"       %1$s [-h|--help] [help]\n" \
"       This help message.\n" \
"\n" \
"With OPTIONS:\n" \
"    -d|--debug                      -- Run in debug mode without neither\n" \
"                                       exception handling nor timeouts.\n" \
"    -i|--icase                      -- Ignore case when matching against\n" \
"                                       <PATTERN>.\n" \
"    -s|--silent                     -- Enable `silent' reporter to silence all\n" \
"                                       console output.\n" \
"    -t[<COLOR>]|--terse[=<COLOR>]   -- Enable `terse' reporter with minimal\n" \
"                                       console output.\n" \
"    -v[<COLOR>]|--verbose[=<COLOR>] -- Enable `verbose' reporter with full\n" \
"                                       console output.\n" \
"    -x[<PATH>]|--xml[=<PATH>]       -- Enable `xml' reporter to generate output\n" \
"                                       to <PATH> according to JUnit XML format.\n" \
"    -a[<PATH>]|--tap[=<PATH>]       -- Enable `tap' reporter to generate output\n" \
"                                       to <PATH> according to Test Anything\n" \
"                                       Protocol format.\n" \
"\n" \
"Where:\n" \
"    NAME    -- Full name used to select a single suite or test ;\n" \
"               by default, the top-level suite is selected.\n" \
"    PATTERN -- POSIX extended regular expression used to select suites and / or\n" \
"               tests ; by default, all suites and tests are selected.\n" \
"    COLOR   -- enforce output colorization when `on', disable it when `off' ;\n" \
"               when unspecified, it is enabled only if terminal supports it.\n" \
"    PATH    -- pathname to file where output is generated into ; when\n" \
"               unspecified or specified as `-', output is directed to stdout.\n"

static void
cute_usage(FILE * stdio)
{
	fprintf(stdio, CUTE_HELP, program_invocation_short_name);
}

int
cute_main(int                        argc,
          char * const               argv[],
          const struct cute_suite *  suite,
          const char *               package,
          const char *               version)
{
	cute_assert(argc);
	cute_assert(suite);

	struct cute_config            conf = CUTE_CONFIG_INIT;
	enum { INFO, SHOW, RUN, ERR } cmd = ERR;
	const char *                  match = NULL;
	bool                          icase = false;
	int                           ret;

	while (true) {
		int                        o;
		static const struct option opts[] = {
			{ "debug",   no_argument,       NULL, 'd' },
			{ "icase",   no_argument,       NULL, 'i' },
			{ "silent",  no_argument,       NULL, 's' },
			{ "tap",     optional_argument, NULL, 'a' },
			{ "terse",   optional_argument, NULL, 't' },
			{ "verbose", optional_argument, NULL, 'v' },
			{ "xml",     optional_argument, NULL, 'x' },
			{ "help",    no_argument,       NULL, 'h' },
			{ NULL,      0,                 NULL, 0 }
		};

		o = getopt_long(argc, argv, ":a::dist::v::x::h", opts, NULL);
		if (o < 0)
			break;

		switch (o) {
		case 'a':
			ret = cute_config_enable_tap(&conf, optarg);
			break;

		case 'd':
			cute_config_enable_debug(&conf);
			ret = 0;
			break;

		case 'i':
			icase = true;
			ret = 0;
			break;

		case 's':
			ret = cute_config_enable_silent(&conf);
			break;

		case 't':
			ret = cute_config_enable_terse(&conf, optarg);
			break;

		case 'v':
			ret = cute_config_enable_verb(&conf, optarg);
			break;

		case 'x':
			ret = cute_config_enable_xml(&conf, optarg);
			break;

		case 'h':
			goto help;

		case ':':
		case '?':
			cute_error("unrecognized option '%s'.\n\n",
			           argv[optind - 1]);
			goto usage;

		default:
			cute_error("unexpected option parsing error.\n\n");
			goto usage;
		}

		if (ret) {
			fputc('\n', stderr);
			goto usage;
		}
	}

	argc -= optind - 1;
	argv = &argv[optind - 1];
	if ((argc < 2) || (argc > 3)) {
		cute_error("invalid number of arguments.\n\n");
		goto usage;
	}

	if (!strcmp(argv[1], "help"))
		goto help;
	else if (!strcmp(argv[1], "info"))
		cmd = INFO;
	else if (!strcmp(argv[1], "show"))
		cmd = SHOW;
	else if (!strcmp(argv[1], "run"))
		cmd = RUN;

	if (cmd == ERR) {
		cute_error("'%s': unknown command.\n\n", argv[1]);
		goto usage;
	}

	if ((cmd == INFO) || (cmd == SHOW)) {
		if (cute_config_cons_enabled(&conf, 0U) &&
		    !cute_config_report_enabled(&conf,
		                                CUTE_CONFIG_TERSE_REPORT |
		                                CUTE_CONFIG_VERB_REPORT)) {
			cute_error("'%s' command: "
			           "unsupported console reporter "
			           "(use either -t or -v option).\n\n",
			           argv[1]);
			goto usage;
		}
	}

	if (argc == 3) {
		ret = cute_match_parse(argv[2]);
		if (ret)
			goto out;
		match = argv[2];
	}

	ret = cute_init(&conf, package, version);
	if (ret)
		goto out;

	if (cmd == INFO)
		ret = cute_suite_info(suite, match);
	else if (cmd == SHOW)
		ret = cute_show_suite(suite, match, icase);
	else if (cmd == RUN)
		ret = cute_run_suite(suite, match, icase);

	cute_fini();

out:
	return !ret ? EXIT_SUCCESS : EXIT_FAILURE;

usage:
	cute_usage(stderr);
	return EXIT_FAILURE;

help:
	cute_usage(stdout);
	return EXIT_SUCCESS;
}
