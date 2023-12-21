/******************************************************************************
 * SPDX-License-Identifier: LGPL-3.0-only
 *
 * This file is part of CUTe.
 * Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
 ******************************************************************************/

#include "common.h"
#include "run.h"
#include "report.h"
#include <stdlib.h>
#include <string.h>
#include <link.h>
#include <gelf.h>
#include <sys/auxv.h>

const char *        cute_package = "???";
const char *        cute_package_version;
char                cute_hostname[HOST_NAME_MAX + 1];
char *              cute_build_id;
struct cute_prop    cute_build_tool;
struct cute_prop    cute_build_flags;
struct cute_prop    cute_build_conf;
sigjmp_buf          cute_jmp_env;
unsigned int        cute_run_nr;
static const char * cute_pattern;
static regex_t      cute_regex;

const char *
cute_state_label(enum cute_state state)
{
	cute_state_assert_intern(state);

	static const char * const labels[] = {
		[CUTE_INIT_STATE]     = "init",
		[CUTE_SETUP_STATE]    = "setup",
		[CUTE_EXEC_STATE]     = "exec",
		[CUTE_TEARDOWN_STATE] = "teardown",
		[CUTE_DONE_STATE]     = "done",
		[CUTE_OFF_STATE]      = "off",
		[CUTE_FINI_STATE]     = "fini"
	};

	return labels[state];
}

const char *
cute_issue_label(enum cute_issue issue)
{
	cute_issue_assert_intern(issue);

	static const char * const labels[] = {
		[CUTE_UNK_ISSUE]  = "unk",
		[CUTE_PASS_ISSUE] = "pass",
		[CUTE_SKIP_ISSUE] = "skip",
		[CUTE_FAIL_ISSUE] = "fail",
		[CUTE_EXCP_ISSUE] = "excp",
		[CUTE_OFF_ISSUE]  = "off"
	};

	return labels[issue];
}

#define CUTE_MATCH_SIZE (1024U)

int
cute_match_parse(const char * pattern)
{
	cute_assert_intern(pattern);

	size_t len;

	len = strnlen(pattern, CUTE_MATCH_SIZE);
	if (!len || (len >= CUTE_MATCH_SIZE)) {
		cute_error("matching regular expression missing or too long.\n");
		return -EINVAL;
	}

	return 0;
}

bool
cute_match_run(const struct cute_run * run)
{
	cute_run_assert_intern(run);

	if (cute_pattern) {
		cute_assert_intern(cute_pattern[0]);
		cute_assert_intern(strnlen(cute_pattern,
		                           CUTE_MATCH_SIZE) < CUTE_MATCH_SIZE);

		return cute_regex_match(&cute_regex, run->name);
	}

	return true;
}

int
cute_match_init(const char * pattern, bool icase)
{
	if (pattern) {
		cute_assert_intern(pattern[0]);
		cute_assert_intern(strnlen(pattern, CUTE_MATCH_SIZE) <
		                   CUTE_MATCH_SIZE);

		int err;

		err = cute_regex_init(&cute_regex, pattern, icase);
		if (err)
			return err;

		cute_pattern = pattern;
	}

	return 0;
}

void
cute_match_fini(void)
{
	if (cute_pattern) {
		cute_assert_intern(cute_pattern[0]);
		cute_assert_intern(strnlen(cute_pattern,
		                           CUTE_MATCH_SIZE) < CUTE_MATCH_SIZE);

		cute_pattern = NULL;
		cute_regex_fini(&cute_regex);
	}
}

struct cute_iter *
cute_base_create_iter(const struct cute_base * base)
{
	cute_base_assert_intern(base);

	return base->ops->iter(base);
}

struct cute_run *
cute_create_run(const struct cute_base * base, struct cute_run * parent)
{
	cute_base_assert_intern(base);

	struct cute_run * run;

	run = base->ops->run(base, parent);

	cute_run_nr++;

	return run;
}

void
cute_base_foreach(const struct cute_base * root,
                  cute_base_process_fn *   process,
                  void *                   data)
{
	cute_base_assert_intern(root);
	cute_assert_intern(process);

	struct cute_iter * iter;
	struct cute_stack  stk;

	iter = cute_base_create_iter(root);
	if (!iter) {
		process(root, CUTE_ONCE_VISIT, data);
		return;
	}

	cute_assert_intern(iter);

	cute_stack_init(&stk);
	cute_stack_push(&stk, iter);

	process(root, CUTE_BEGIN_VISIT, data);

	do {
		const struct cute_base * base;

		iter = cute_stack_peek(&stk);
		cute_assert_intern(iter);
		if (!cute_iter_end(iter)) {
			enum cute_visit visit = CUTE_ONCE_VISIT;

			base = cute_iter_const_next(iter);
			iter = cute_base_create_iter(base);
			if (iter) {
				cute_stack_push(&stk, iter);
				visit = CUTE_BEGIN_VISIT;
			}

			process(base, visit, data);
			continue;
		}

		base = cute_iter_const_data(iter);
		cute_assert_intern(base);
		process(base, CUTE_END_VISIT, data);

		cute_stack_pop(&stk);
		cute_iter_destroy(iter);
	} while (cute_stack_count(&stk));

	cute_stack_fini(&stk);
}

struct cute_elf {
	int    fd;
	Elf *  desc;
	size_t shstr_indx;
};

typedef int (cute_elf_parse_fn)(const void *, size_t);

struct cute_elf_parser {
	ElfW(Word)          sh_type;
	ElfW(Word)          sh_flags;
	const char *        sh_name;
	Elf_Type            d_type;
	cute_elf_parse_fn * parse;
};

static int
cute_next_elf_shdr(const struct cute_elf * elf,
                   Elf_Scn **              scn,
                   GElf_Shdr *             shdr)
{
	int err;

	*scn = elf_nextscn(elf->desc, *scn);
	if (!*scn) {
		err = elf_errno();
		if (err)
			goto err;
		return 0;
	}

	if (gelf_getshdr(*scn, shdr) != shdr) {
		err = elf_errno();
		goto err;
	}

	return 1;

err:
	cute_error("cannot retrieve ELF section: %s.\n", elf_errmsg(err));

	return -1;
}

static int
cute_parse_elf_shdr(const struct cute_elf *        elf,
                    const GElf_Shdr *              shdr,
                    const struct cute_elf_parser * parser,
                    const char **                  name)
{
	if (shdr->sh_type != parser->sh_type)
		return 0;

	if ((shdr->sh_flags & parser->sh_flags) != parser->sh_flags)
		return 0;

	*name = elf_strptr(elf->desc, elf->shstr_indx, shdr->sh_name);
	if (!*name)
		goto err;

	if (strcmp(*name, parser->sh_name))
		return 0;

	return 1;

err:
	cute_error("cannot retrieve ELF section name: %s.\n",
	           elf_errmsg(elf_errno()));

	return -1;
}

static int
cute_parse_elf_sdata(Elf_Scn *                      scn,
                     const char *                   name,
                     const struct cute_elf_parser * parser)
{
	Elf_Data *   data;
	int          err;
	const char * msg;

	data = elf_getdata(scn, NULL);
	if (!data) {
		err = elf_errno();
		if (!err)
			return 0;
		msg = elf_errmsg(err);
		goto err;
	}

	if (!data->d_size || (data->d_type != parser->d_type))
		return 0;

	err = parser->parse(data->d_buf, data->d_size);
	if (!err)
		return 1;

	if (err < 0)
		msg = strerror(-err);
	else
		msg = elf_errmsg(err);

err:
	cute_error("'%s': cannot retrieve ELF section data: %s.\n", name, msg);

	return -1;
}

static int
cute_load_elf_strings(struct cute_prop * prop, const void * data, size_t size)
{
	cute_assert_intern(prop);
	cute_assert_intern(data);

	char * str;

	if (!size)
		return -ENODATA;

	str = cute_malloc(size);
	memcpy(str, data, size);
	str[size - 1] = '\0';

	prop->size = size;
	prop->data = str;

	return 0;
}

static int
cute_parse_elf_comment(const void * data, size_t size)
{
	return cute_load_elf_strings(&cute_build_tool, data, size);
}

static int
cute_parse_elf_cmdln(const void * data, size_t size)
{
	return cute_load_elf_strings(&cute_build_flags, data, size);
}

static int
cute_parse_elf_config(const void * data, size_t size)
{
	return cute_load_elf_strings(&cute_build_conf, data, size);
}

static int
cute_parse_elf(const struct cute_elf * elf)
{
	static const struct cute_elf_parser parsers[] = {
		{
			.sh_type  = SHT_PROGBITS,
			.sh_flags = SHF_MERGE | SHF_STRINGS,
			.sh_name  = ".comment",
			.d_type   = ELF_T_BYTE,
			.parse    = cute_parse_elf_comment
		},
		{
			.sh_type  = SHT_PROGBITS,
			.sh_flags = SHF_MERGE | SHF_STRINGS,
			.sh_name  = ".GCC.command.line",
			.d_type   = ELF_T_BYTE,
			.parse    = cute_parse_elf_cmdln
		},
		{
			.sh_type  = SHT_PROGBITS,
			.sh_flags = SHF_MERGE | SHF_STRINGS,
			.sh_name  = ".eBuild.config",
			.d_type   = ELF_T_BYTE,
			.parse    = cute_parse_elf_config
		},
	};
	Elf_Scn *                           scn = NULL;
	unsigned int                        cnt = sizeof(parsers) /
	                                          sizeof(parsers[0]);

	while (true) {
		GElf_Shdr    shdr;
		int          ret;
		unsigned int p;

		ret = cute_next_elf_shdr(elf, &scn, &shdr);
		if (ret <= 0)
			return ret;

		for (p = 0; p < (sizeof(parsers) / sizeof(parsers[0])); p++) {
			const char *                   name;
			const struct cute_elf_parser * parser = &parsers[p];

			ret = cute_parse_elf_shdr(elf, &shdr, parser, &name);
			if (!ret)
				continue;
			else if (ret < 0)
				return ret;

			ret = cute_parse_elf_sdata(scn, name, parser);
			if (!ret)
				continue;
			else if (ret < 0)
				return ret;

			if (!(--cnt))
				return 0;
		}
	}
}

static int
cute_open_elf(struct cute_elf * elf, const char * path)
{
	cute_assert_intern(elf);
	cute_assert_intern(path);
	cute_assert_intern(path[0]);

	const char * msg;

	elf->fd = open(path, O_RDONLY, O_NOATIME | O_NOCTTY);
	if (elf->fd < 0) {
		msg = strerror(errno);
		goto err;
	}

	if (elf_version(EV_CURRENT) == EV_NONE) {
		msg = elf_errmsg(elf_errno());
		goto close;
	}

	elf->desc = elf_begin(elf->fd, ELF_C_READ, NULL);
	if (!elf->desc) {
		msg = elf_errmsg(elf_errno());
		goto close;
	}

	if (elf_kind(elf->desc) != ELF_K_ELF) {
		msg = "not a valid ELF file";
		goto end;
	}

	if (elf_getshdrstrndx (elf->desc, &elf->shstr_indx)) {
		msg = elf_errmsg(elf_errno());
		goto end;
	}

	return 0;

end:
	elf_end(elf->desc);
close:
	close(elf->fd);
err:
	cute_error("cannot open ELF file: %s.\n", msg);

	return -1;
}

static void
cute_close_elf(const struct cute_elf * elf)
{
	cute_assert_intern(elf);
	cute_assert_intern(elf->desc);
	cute_assert_intern(elf->fd >= 0);

	elf_end(elf->desc);
	close(elf->fd);
}

static char *
cute_make_build_id(const void * data, size_t size)
{
	if (size) {
		cute_assert_intern(data);

		char *          str;
		const uint8_t * id = data;
		unsigned int    c;

		str = cute_malloc((2 * size) + 1);

		for (c = 0; c < size; c++, id++)
			sprintf(&str[2 * c], "%02x", (unsigned int)*id);

		return str;
	}
	else
		return NULL;
}

/*
 * Retrieve build ID generated using the gcc / ld --build-id command line
 * option. Build ID is located into an ELF note section named
 * `.note.gnu.build-id'.
 */
static void
cute_load_build_id(void)
{
	const ElfW(Phdr) * phdrs;
	unsigned long      nr;
	const void *       addr;
	unsigned int       h;

	phdrs = (const ElfW(Phdr) *)getauxval(AT_PHDR);
	cute_assert_intern(phdrs);

	nr = getauxval(AT_PHNUM);
	cute_assert_intern(nr);

	addr = (const void *)phdrs - phdrs[0].p_offset;

	for (h = 0; h < nr; h++) {
		const ElfW(Phdr) * phdr = &phdrs[h];

		if (phdr->p_type == PT_NOTE) {
			const void * notes = addr + phdr->p_vaddr;
			const void * end = notes + phdr->p_filesz;

			while (notes < end) {
				const ElfW(Nhdr) * nhdr = notes;

				if ((nhdr->n_type == NT_GNU_BUILD_ID) &&
				    (nhdr->n_namesz == sizeof(ELF_NOTE_GNU)) &&
				    !memcmp((const char *)&nhdr[1],
				            ELF_NOTE_GNU,
				            sizeof(ELF_NOTE_GNU))) {
#define __ALIGN(_val, _align) (((_val) + (_align) - 1) & ~((_align) - 1))
#define NHDR_ALIGN(_sz)       __ALIGN(_sz, 4U)
					cute_build_id = cute_make_build_id(
						(const void *)&nhdr[1] +
						NHDR_ALIGN(nhdr->n_namesz),
						nhdr->n_descsz);
					return;
				}

				notes += sizeof(*nhdr) +
				         NHDR_ALIGN(nhdr->n_namesz) +
				         NHDR_ALIGN(nhdr->n_descsz);
			}
		}
	}
}

void
cute_load_props(const char * package, const char * version)
{
	const char *    path;
	struct cute_elf elf;
	int             err;

	path = (const char *)getauxval(AT_EXECFN);
	cute_assert_intern(path);

	err = gethostname(cute_hostname, sizeof(cute_hostname));
	cute_assert_intern(!err);

	if (package && package[0])
		cute_package = package;
	if (version && version[0])
		cute_package_version = version;

	cute_load_build_id();

	if (cute_open_elf(&elf, path))
		goto err;

	if (cute_parse_elf(&elf))
		goto err;

	cute_close_elf(&elf);

	return;

err:
	cute_error("'%s': cannot load ELF properties.\n", path);
}

void
cute_unload_props(void)
{
	free(cute_build_id);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
	free((void *)cute_build_tool.data);
	free((void *)cute_build_flags.data);
	free((void *)cute_build_conf.data);
#pragma GCC diagnostic pop
}
