################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

config-in   := Config.in
config-h    := $(PACKAGE)/priv/config.h

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

subdirs     += $(call kconf_enabled,CUTE_UTEST,test)
test-deps   := src

define libcute_pkgconf_tmpl
prefix=$(PREFIX)
exec_prefix=$${prefix}
libdir=$${exec_prefix}/lib
includedir=$${prefix}/include

Name: libcute
Description: CUTe unit testing library
Version: %%PKG_VERSION%%
Cflags: -I$${includedir}
Libs: -L$${libdir} -lcute
endef

pkgconfigs      := libcute.pc
libcute.pc-tmpl := libcute_pkgconf_tmpl

################################################################################
# Documentation generation
################################################################################

doxyconf  := $(CURDIR)/sphinx/Doxyfile
doxyenv   := SRCDIR="$(HEADERDIR) $(SRCDIR)"

sphinxsrc := $(CURDIR)/sphinx
sphinxenv := \
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
