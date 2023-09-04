################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

override PACKAGE := cute
override VERSION := 1.0
EXTRA_CFLAGS     := -O2
EXTRA_LDFLAGS    := $(EXTRA_CFLAGS)

export VERSION EXTRA_CFLAGS EXTRA_LDFLAGS

ifeq ($(strip $(EBUILDDIR)),)
ifneq ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath ebuild)
else  # ($(realpath ebuild/main.mk),)
EBUILDDIR := $(realpath /usr/share/ebuild)
endif # !($(realpath ebuild/main.mk),)
endif # ($(strip $(EBUILDDIR)),)

ifeq ($(realpath $(EBUILDDIR)/main.mk),)
$(error '$(EBUILDDIR)': no valid eBuild install found !)
endif # ($(realpath $(EBUILDDIR)/main.mk),)


include $(EBUILDDIR)/main.mk

override ebuilddoc_online_url := http://grgbr.github.io/ebuild/

# Install eBuild construction logic
#
# $(1): eBuild top-level source directory
# $(2): destination install directory
define sync_ebuild_recipe
$(foreach f, \
          $(notdir $(wildcard $(strip $(1))/*.mk)), \
          $(call install_recipe,-D --mode=644, \
                                $(strip $(1))/$(f), \
                                $(strip $(2))/$(f))$(newline))
$(foreach f, \
          $(notdir $(wildcard $(strip $(1))/scripts/*)), \
          $(call install_recipe,-D --mode=644, \
                                $(strip $(1))/scripts/$(f), \
                                $(strip $(2))/scripts/$(f))$(newline))
endef

define sync_src_recipe
@echo "  SYNC    $(strip $(1))"
$(Q)mkdir -p -m755 $(1)
$(Q)env GIT=$(GIT) SVN="$(SVN)" \
    $(EBUILDDIR)/scripts/list_version_files.sh | \
    $(RSYNC) --recursive \
             --links \
             --times \
             --perms \
             --delete \
             --chmod=D755 --chmod=F644 \
             --files-from=- \
             $(CURDIR)/ \
             $(1)/
endef

define make_tarball_recipe
@echo "  TARBALL $(strip $(1))"
$(Q)$(TAR) --owner=1000 \
           --group=1000 \
           --directory $(dir $(2)) \
           -cJf $(1) $(notdir $(2))
endef

distdir := $(BUILDDIR)/$(PACKAGE)-$(VERSION)

# Make sure that source distribution tarball includes documentation pointing to
# online GitHub pages.
.PHONY: dist
dist: export EBUILDDOC_TARGET_PATH := $(ebuilddoc_online_url)
dist: doc
	$(call sync_src_recipe,$(distdir))
	$(call sync_ebuild_recipe,$(EBUILDDIR),$(distdir)/ebuild)
	$(call installdir_recipe,--chmod=D755 --chmod=F644, \
	                         $(sphinxhtmldir), \
	                         $(distdir)/docs/html)
	$(foreach f, \
	          $(sphinx_list_pdf), \
	          $(call install_recipe, \
	                 -m644, \
	                 $(sphinxpdfdir)/$(f), \
	                 $(distdir)/docs/$(f))$(newline))
	$(foreach f, \
	          $(sphinx_list_info), \
	          $(call install_recipe,-m644, \
	                                $(sphinxinfodir)/$(f), \
	                                $(distdir)/docs/$(f))$(newline))
	$(call make_tarball_recipe,$(BUILDDIR)/$(PACKAGE)-$(VERSION).tar.xz, \
	                           $(distdir))

.PHONY: distclean
distclean: clean
