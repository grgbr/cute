################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

test-cflags     := -Wall \
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
                   $(filter-out -DNDEBUG,$(EXTRA_CFLAGS))
test-ldflags := $(common-cflags) \
                -L$(BUILDDIR)/../src \
                $(filter-out -DNDEBUG,$(EXTRA_LDFLAGS)) \
                -Wl,-z,start-stop-visibility=hidden

checkbins                          += simple-exec-pass-utest
simple-exec-pass-utest-objs        := simple_exec_pass.o $(config-obj)
simple-exec-pass-utest-cflags      := $(test-cflags)
simple-exec-pass-utest-ldflags     := $(test-ldflags) -lcute
simple-exec-pass-utest-path        := $(LIBEXECDIR)/cute/simple-exec-pass-utest

checkbins                          += simple-exec-fail-utest
simple-exec-fail-utest-objs        := simple_exec_fail.o $(config-obj)
simple-exec-fail-utest-cflags      := $(test-cflags)
simple-exec-fail-utest-ldflags     := $(test-ldflags) -lcute
simple-exec-fail-utest-path        := $(LIBEXECDIR)/cute/simple-exec-fail-utest

checkbins                          += simple-exec-error-utest
simple-exec-error-utest-objs       := simple_exec_error.o $(config-obj)
simple-exec-error-utest-cflags     := $(test-cflags)
simple-exec-error-utest-ldflags    := $(test-ldflags) -lcute
simple-exec-error-utest-path       := $(LIBEXECDIR)/cute/simple-exec-error-utest

checkbins                          += simple-setup-fail-utest
simple-setup-fail-utest-objs       := simple_setup_fail.o $(config-obj)
simple-setup-fail-utest-cflags     := $(test-cflags)
simple-setup-fail-utest-ldflags    := $(test-ldflags) -lcute
simple-setup-fail-utest-path       := $(LIBEXECDIR)/cute/simple-setup-fail-utest

checkbins                          += simple-setup-error-utest
simple-setup-error-utest-objs      := simple_setup_error.o $(config-obj)
simple-setup-error-utest-cflags    := $(test-cflags)
simple-setup-error-utest-ldflags   := $(test-ldflags) -lcute
simple-setup-error-utest-path      := $(LIBEXECDIR)/cute/simple-setup-error-utest

checkbins                          += simple-teardown-fail-utest
simple-teardown-fail-utest-objs    := simple_teardown_fail.o $(config-obj)
simple-teardown-fail-utest-cflags  := $(test-cflags)
simple-teardown-fail-utest-ldflags := $(test-ldflags) -lcute
simple-teardown-fail-utest-path    := $(LIBEXECDIR)/cute/simple-teardown-fail-utest

checkbins                          += simple-fixture-fail-utest
simple-fixture-fail-utest-objs     := simple_fixture_fail.o $(config-obj)
simple-fixture-fail-utest-cflags   := $(test-cflags)
simple-fixture-fail-utest-ldflags  := $(test-ldflags) -lcute
simple-fixture-fail-utest-path     := $(LIBEXECDIR)/cute/simple-fixture-fail-utest

checkbins                          += simple-tmout-utest
simple-tmout-utest-objs            := simple_tmout.o $(config-obj)
simple-tmout-utest-cflags          := $(test-cflags)
simple-tmout-utest-ldflags         := $(test-ldflags) -lcute
simple-tmout-utest-path            := $(LIBEXECDIR)/cute/simple-tmout-utest

checkbins                          += simple-iodir-utest
simple-iodir-utest-objs            := simple_iodir.o $(config-obj)
simple-iodir-utest-cflags          := $(test-cflags)
simple-iodir-utest-ldflags         := $(test-ldflags) -lcute
simple-iodir-utest-path            := $(LIBEXECDIR)/cute/simple-iodir-utest

checkbins                          += check-bool-utest
check-bool-utest-objs              := check_bool.o $(config-obj)
check-bool-utest-cflags            := $(test-cflags)
check-bool-utest-ldflags           := $(test-ldflags) -lcute
check-bool-utest-path              := $(LIBEXECDIR)/cute/check-bool-utest

checkbins                          += check-sint-utest
check-sint-utest-objs              := check_sint.o $(config-obj)
check-sint-utest-cflags            := $(test-cflags)
check-sint-utest-ldflags           := $(test-ldflags) -lcute
check-sint-utest-path              := $(LIBEXECDIR)/cute/check-sint-utest

checkbins                          += check-uint-utest
check-uint-utest-objs              := check_uint.o $(config-obj)
check-uint-utest-cflags            := $(test-cflags)
check-uint-utest-ldflags           := $(test-ldflags) -lcute
check-uint-utest-path              := $(LIBEXECDIR)/cute/check-uint-utest

checkbins                          += check-hex-utest
check-hex-utest-objs               := check_hex.o $(config-obj)
check-hex-utest-cflags             := $(test-cflags)
check-hex-utest-ldflags            := $(test-ldflags) -lcute
check-hex-utest-path               := $(LIBEXECDIR)/cute/check-hex-utest

checkbins                          += check-flt-utest
check-flt-utest-objs               := check_flt.o $(config-obj)
check-flt-utest-cflags             := $(test-cflags)
check-flt-utest-ldflags            := $(test-ldflags) -lcute
check-flt-utest-path               := $(LIBEXECDIR)/cute/check-flt-utest

checkbins                          += check-dbl-utest
check-dbl-utest-objs               := check_dbl.o $(config-obj)
check-dbl-utest-cflags             := $(test-cflags)
check-dbl-utest-ldflags            := $(test-ldflags) -lcute
check-dbl-utest-path               := $(LIBEXECDIR)/cute/check-dbl-utest

checkbins                          += check-str-utest
check-str-utest-objs               := check_str.o $(config-obj)
check-str-utest-cflags             := $(test-cflags)
check-str-utest-ldflags            := $(test-ldflags) -lcute
check-str-utest-path               := $(LIBEXECDIR)/cute/check-str-utest

checkbins                          += check-ptr-utest
check-ptr-utest-objs               := check_ptr.o $(config-obj)
check-ptr-utest-cflags             := $(test-cflags)
check-ptr-utest-ldflags            := $(test-ldflags) -lcute
check-ptr-utest-path               := $(LIBEXECDIR)/cute/check-ptr-utest

checkbins                          += check-mem-utest
check-mem-utest-objs               := check_mem.o $(config-obj)
check-mem-utest-cflags             := $(test-cflags)
check-mem-utest-ldflags            := $(test-ldflags) -lcute
check-mem-utest-path               := $(LIBEXECDIR)/cute/check-mem-utest

checkbins                          += expect-bool-utest
expect-bool-utest-objs             := expect_bool.o $(config-obj)
expect-bool-utest-cflags           := $(test-cflags)
expect-bool-utest-ldflags          := $(test-ldflags) -lcute
expect-bool-utest-path             := $(LIBEXECDIR)/cute/expect-bool-utest

checkbins                          += expect-sint-utest
expect-sint-utest-objs             := expect_sint.o $(config-obj)
expect-sint-utest-cflags           := $(test-cflags)
expect-sint-utest-ldflags          := $(test-ldflags) -lcute
expect-sint-utest-path             := $(LIBEXECDIR)/cute/expect-sint-utest

checkbins                          += expect-uint-utest
expect-uint-utest-objs             := expect_uint.o $(config-obj)
expect-uint-utest-cflags           := $(test-cflags)
expect-uint-utest-ldflags          := $(test-ldflags) -lcute
expect-uint-utest-path             := $(LIBEXECDIR)/cute/expect-uint-utest

checkbins                          += expect-hex-utest
expect-hex-utest-objs              := expect_hex.o $(config-obj)
expect-hex-utest-cflags            := $(test-cflags)
expect-hex-utest-ldflags           := $(test-ldflags) -lcute
expect-hex-utest-path              := $(LIBEXECDIR)/cute/expect-hex-utest

checkbins                          += expect-flt-utest
expect-flt-utest-objs              := expect_flt.o $(config-obj)
expect-flt-utest-cflags            := $(test-cflags)
expect-flt-utest-ldflags           := $(test-ldflags) -lcute
expect-flt-utest-path              := $(LIBEXECDIR)/cute/expect-flt-utest

checkbins                          += expect-assert-utest
expect-assert-utest-objs           := expect_assert.o $(config-obj)
expect-assert-utest-cflags         := $(test-cflags)
expect-assert-utest-ldflags        := $(test-ldflags) -lcute
expect-assert-utest-path           := $(LIBEXECDIR)/cute/expect-assert-utest

checkbins                          += expect-str-utest
expect-str-utest-objs              := expect_str.o $(config-obj)
expect-str-utest-cflags            := $(test-cflags)
expect-str-utest-ldflags           := $(test-ldflags) -lcute
expect-str-utest-path              := $(LIBEXECDIR)/cute/expect-str-utest

checkbins                          += expect-ptr-utest
expect-ptr-utest-objs              := expect_ptr.o $(config-obj)
expect-ptr-utest-cflags            := $(test-cflags)
expect-ptr-utest-ldflags           := $(test-ldflags) -lcute
expect-ptr-utest-path              := $(LIBEXECDIR)/cute/expect-ptr-utest

checkbins                          += expect-mem-utest
expect-mem-utest-objs              := expect_mem.o $(config-obj)
expect-mem-utest-cflags            := $(test-cflags)
expect-mem-utest-ldflags           := $(test-ldflags) -lcute
expect-mem-utest-path              := $(LIBEXECDIR)/cute/expect-mem-utest

build-check: $(BUILDDIR)/cute-utest.sh

$(BUILDDIR)/cute-utest.sh: $(SRCDIR)/cute-utest.sh | $(BUILDDIR)/
	sed 's;@@LIBEXECDIR@@;$(LIBEXECDIR);g' $(<) > $(@)

clean-check: _clean-check

.PHONY: _clean-check
_clean-check:
	$(call rm_recipe,$(BUILDDIR)/cute-utest.sh)

_outrefs := $(foreach b, \
                      $(checkbins), \
                      $(DESTDIR)$(LIBEXECDIR)/cute/$(b)-outref.txt)

install-check install-strip-check: $(DESTDIR)$(BINDIR)/cute-utest.sh $(_outrefs)

.PHONY: $(DESTDIR)$(BINDIR)/cute-utest.sh
$(DESTDIR)$(BINDIR)/cute-utest.sh: $(BUILDDIR)/cute-utest.sh
	$(call install_recipe,--mode=755,$(<),$(@))

.PHONY: $(_outrefs)
$(_outrefs): $(DESTDIR)$(LIBEXECDIR)/cute/%: $(SRCDIR)/%
	$(call install_recipe,--mode=644,$(<),$(@))

uninstall-check: _uninstall-check

.PHONY: _uninstall-check
_uninstall-check:
	$(foreach b, \
	          $(_outrefs), \
	          $(call rm_recipe,$(b))$(newline))
	$(call rm_recipe,$(DESTDIR)$(BINDIR)/cute-utest.sh)

# ex: filetype=make :
