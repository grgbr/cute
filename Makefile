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
PYTHON_SHEBANG   := /usr/bin/env python3

export VERSION EXTRA_CFLAGS EXTRA_LDFLAGS PYTHON_SHEBANG

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

ifeq ($(CONFIG_CUTE_UTEST),y)

ifeq ($(strip $(CROSS_COMPILE)),)

CHECK_VERBOSE ?= n
ifeq ($(filter y 1,$(CHECK_VERBOSE)),)
override CHECK_VERBOSE := >/dev/null 2>&1
endif # ifneq ($(filter y 1,$(CHECK_VERBOSE)),)

check_lib_search_path := \
	$(BUILDDIR)/src$(if $(LD_LIBRARY_PATH),:$(LD_LIBRARY_PATH))

.PHONY: check
check: | build-check
	@echo "  CHECK   $(@)"
	$(Q)env LD_LIBRARY_PATH="$(check_lib_search_path)" \
	        TMPDIR="$(BUILDDIR)/test" \
	        LIBEXECDIR="$(BUILDDIR)/test" \
	        TESTSRCDIR="$(CURDIR)/test" \
	        $(SHELL) $(BUILDDIR)/test/cute-utest.sh \
	        $(CHECK_VERBOSE)

else  # ifneq ($(strip $(CROSS_COMPILE)),)

.PHONY: check
check:
	$(error Cannot check while cross building !)

endif # ifeq ($(strip $(CROSS_COMPILE)),)

else # ifneq ($(CONFIG_CUTE_UTEST),y)

.PHONY: check
check:

endif # ifeq ($(CONFIG_CUTE_UTEST),y)
