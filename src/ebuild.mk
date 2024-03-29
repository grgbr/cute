################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

common-cflags      := -Wall \
                      -Wextra \
                      -Wformat=2 \
                      -Wconversion \
                      -Wundef \
                      -Wshadow \
                      -Wcast-qual \
                      -Wcast-align \
                      -Wmissing-declarations \
                      -D_GNU_SOURCE \
                      -I $(TOPDIR)/include \
                      $(filter-out -DNDEBUG,$(EXTRA_CFLAGS)) \
                      $(filter-out -ffast-math,$(EXTRA_CFLAGS)) -fno-fast-math \
                      -fvisibility=hidden \
                      -pthread
common-ldflags     := $(common-cflags) \
                      $(filter-out -DNDEBUG,$(EXTRA_LDFLAGS)) \
                      $(filter-out -ffast-math,$(EXTRA_LDFLAGS)) \
                      -fno-fast-math \
                      -Wl,-z,start-stop-visibility=hidden

solibs             := libcute.so
libcute.so-objs    += \
	shared/init.o \
	shared/suite.o \
	shared/test.o \
	shared/tap.o \
	shared/xml.o \
	shared/terse.o \
	shared/verb.o \
	shared/report.o \
	shared/cons.o \
	shared/expect.o \
	shared/check.o \
	shared/run.o \
	shared/assess.o \
	shared/iodir.o \
	shared/common.o \
	shared/util.o
libcute.so-cflags  := $(filter-out -fpie -fPIE,$(common-cflags)) -fpic
libcute.so-ldflags := $(filter-out -fpie -fPIE,$(common-ldflags)) \
                      -shared -fpic -Bsymbolic -Wl,-soname,libcute.so
libcute.so-pkgconf := libelf

arlibs             := libcute.a
libcute.a-objs     += \
	static/init.o \
	static/suite.o \
	static/test.o \
	static/tap.o \
	static/xml.o \
	static/terse.o \
	static/verb.o \
	static/report.o \
	static/cons.o \
	static/expect.o \
	static/check.o \
	static/run.o \
	static/assess.o \
	static/iodir.o \
	static/common.o \
	static/util.o
libcute.a-cflags   := $(common-cflags)

# ex: filetype=make :
