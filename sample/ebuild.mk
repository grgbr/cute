################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

sample-cflags             := -Wall \
                             -Wextra \
                             -Wformat=2 \
                             -Wconversion \
                             -Wundef \
                             -Wshadow \
                             -Wcast-qual \
                             -Wcast-align \
                             -Wmissing-declarations \
                             -D_GNU_SOURCE \
                             -DCUTE_VERSION_STRING=\"$(VERSION)\" \
                             -I $(TOPDIR)/include \
                             $(filter-out -DNDEBUG,$(EXTRA_CFLAGS))
sample-ldflags            := $(sample-cflags) \
                             -L$(BUILDDIR)/../src \
                             $(filter-out -DNDEBUG,$(EXTRA_LDFLAGS)) \
                             -Wl,-z,start-stop-visibility=hidden

bins                      += cute-hello-sample
cute-hello-sample-objs    := hello.o
cute-hello-sample-cflags  := $(sample-cflags)
cute-hello-sample-ldflags := $(sample-ldflags) -lcute

bins                      += cute-full-sample
cute-full-sample-objs     := full.o
cute-full-sample-cflags   := $(sample-cflags)
cute-full-sample-ldflags  := $(sample-ldflags) -lcute

bins                      += cute-mock-sample
cute-mock-sample-objs     := mock.o
cute-mock-sample-cflags   := $(sample-cflags)
cute-mock-sample-ldflags  := $(sample-ldflags) -lcute

#config.o-src := $(BUILDDIR)/config.c
#
##$(kconf_config) | $(BUILDDIR)
#$(BUILDDIR)/config.c: $(BUILDDIR)/../.config \
#                      $(EBUILDDIR)/scripts/gen_conf_obj_src.sh
#	@echo "  CONFSRC $(strip $(@))"
#	$(EBUILDDIR)/scripts/gen_conf_obj_src.sh $(<) > $(@) || \
#		{ $(RM) $(@); exit 1; }

# ex: filetype=make :
