################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

config-in   := Config.in
config-h    := $(PACKAGE)/priv/config.h
config-obj  := config.o

HEADERDIR   := $(CURDIR)/include
headers     += $(PACKAGE)/cute.h
headers     += $(PACKAGE)/types.h
headers     += $(PACKAGE)/check.h
headers     += $(PACKAGE)/expect.h
headers     += $(PACKAGE)/priv/core.h
headers     += $(PACKAGE)/priv/types.h
headers     += $(PACKAGE)/priv/check.h
headers     += $(PACKAGE)/priv/expect.h

subdirs     := src

subdirs     += sample
sample-deps := src

ifeq ($(CONFIG_CUTE_UTEST),y)
subdirs     += test
test-deps   := src
endif # ifeq ($(CONFIG_CUTE_UTEST),y)

define libcute_pkgconf_tmpl
prefix=$(PREFIX)
exec_prefix=$${prefix}
libdir=$${exec_prefix}/lib
includedir=$${prefix}/include

Name: libcute
Description: CUTe unit testing library
Version: %%PKG_VERSION%%
Requires.private: libelf
Cflags: -I$${includedir}
Libs: -L$${libdir} -lcute
endef

pkgconfigs      := libcute.pc
libcute.pc-tmpl := libcute_pkgconf_tmpl

ifneq ($(call kconf_is_enabled,CUTE_REPORT),)

build: $(BUILDDIR)/cute-report

$(BUILDDIR)/cute-report: $(CURDIR)/scripts/cute-report | $(BUILDDIR)/
	sed 's;@@PYTHON_SHEBANG@@;$(PYTHON_SHEBANG);g' $(<) > $(@)

clean: _clean

.PHONY: _clean
_clean:
	$(call rm_recipe,$(BUILDDIR)/cute-report)

install install-strip: $(DESTDIR)$(BINDIR)/cute-report \
                       $(DESTDIR)$(DATADIR)/cute/cute-junit.xsd

.PHONY: $(DESTDIR)$(BINDIR)/cute-report
$(DESTDIR)$(BINDIR)/cute-report: $(BUILDDIR)/cute-report
	$(call install_recipe,--mode=755,$(<),$(@))

.PHONY: $(DESTDIR)$(DATADIR)/cute/cute-junit.xsd
$(DESTDIR)$(DATADIR)/cute/cute-junit.xsd: $(CURDIR)/cute-junit.xsd
	$(call install_recipe,--mode=644,$(<),$(@))

uninstall: _uninstall

.PHONY: _uninstall
_uninstall:
	$(call uninstall_recipe,$(DESTDIR)$(BINDIR),cute-report)
	$(call uninstall_recipe,$(DESTDIR)$(DATADIR)/cute,cute-junit.xsd)

endif # ifneq ($(call kconf_is_enabled,CUTE_REPORT),)

################################################################################
# Source code tags generation
################################################################################

tagfiles := $(shell find $(addprefix $(CURDIR)/,$(subdirs)) \
                         $(HEADERDIR) \
                         -type f)

################################################################################
# Documentation generation
################################################################################

doxyconf  := $(CURDIR)/sphinx/Doxyfile
doxyenv   := SRCDIR="$(HEADERDIR) $(SRCDIR)"

sphinxsrc := $(CURDIR)/sphinx
sphinxenv := \
	CONFIG_CUTE_REPORT="$(CONFIG_CUTE_REPORT)" \
	VERSION="$(VERSION)" \
	$(if $(strip $(EBUILDDOC_TARGET_PATH)), \
	     EBUILDDOC_TARGET_PATH="$(strip $(EBUILDDOC_TARGET_PATH))") \
	$(if $(strip $(EBUILDDOC_INVENTORY_PATH)), \
	     EBUILDDOC_INVENTORY_PATH="$(strip $(EBUILDDOC_INVENTORY_PATH))")

################################################################################
# Source distribution generation
################################################################################

# Declare the list of files under revision control to include into final source
# distribution tarball.
override distfiles = $(list_versioned_recipe)

# Override InterSphinx eBuild base documentation URI and make it point to online
# GitHub pages when building final source distribution tarball
dist: export EBUILDDOC_TARGET_PATH := http://grgbr.github.io/ebuild/

# ex: filetype=make :
