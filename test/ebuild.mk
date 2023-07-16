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

bins                               += simple-exec-pass-utest
simple-exec-pass-utest-objs        := simple_exec_pass.o
simple-exec-pass-utest-cflags      := $(test-cflags)
simple-exec-pass-utest-ldflags     := $(test-ldflags) -lcute
simple-exec-pass-utest-path        := $(LIBEXECDIR)/cute/simple-exec-pass-utest

bins                               += simple-exec-fail-utest
simple-exec-fail-utest-objs        := simple_exec_fail.o
simple-exec-fail-utest-cflags      := $(test-cflags)
simple-exec-fail-utest-ldflags     := $(test-ldflags) -lcute
simple-exec-fail-utest-path        := $(LIBEXECDIR)/cute/simple-exec-fail-utest

bins                               += simple-exec-error-utest
simple-exec-error-utest-objs       := simple_exec_error.o
simple-exec-error-utest-cflags     := $(test-cflags)
simple-exec-error-utest-ldflags    := $(test-ldflags) -lcute
simple-exec-error-utest-path       := $(LIBEXECDIR)/cute/simple-exec-error-utest

bins                               += simple-setup-fail-utest
simple-setup-fail-utest-objs       := simple_setup_fail.o
simple-setup-fail-utest-cflags     := $(test-cflags)
simple-setup-fail-utest-ldflags    := $(test-ldflags) -lcute
simple-setup-fail-utest-path       := $(LIBEXECDIR)/cute/simple-setup-fail-utest

bins                               += simple-setup-error-utest
simple-setup-error-utest-objs      := simple_setup_error.o
simple-setup-error-utest-cflags    := $(test-cflags)
simple-setup-error-utest-ldflags   := $(test-ldflags) -lcute
simple-setup-error-utest-path      := $(LIBEXECDIR)/cute/simple-setup-error-utest

bins                               += simple-teardown-fail-utest
simple-teardown-fail-utest-objs    := simple_teardown_fail.o
simple-teardown-fail-utest-cflags  := $(test-cflags)
simple-teardown-fail-utest-ldflags := $(test-ldflags) -lcute
simple-teardown-fail-utest-path    := $(LIBEXECDIR)/cute/simple-teardown-fail-utest

bins                               += simple-fixture-fail-utest
simple-fixture-fail-utest-objs     := simple_fixture_fail.o
simple-fixture-fail-utest-cflags   := $(test-cflags)
simple-fixture-fail-utest-ldflags  := $(test-ldflags) -lcute
simple-fixture-fail-utest-path     := $(LIBEXECDIR)/cute/simple-fixture-fail-utest

bins                               += simple-iodir-utest
simple-iodir-utest-objs            := simple_iodir.o
simple-iodir-utest-cflags          := $(test-cflags)
simple-iodir-utest-ldflags         := $(test-ldflags) -lcute
simple-iodir-utest-path            := $(LIBEXECDIR)/cute/simple-iodir-utest

bins                               += check-sint-utest
check-sint-utest-objs              := check_sint.o
check-sint-utest-cflags            := $(test-cflags)
check-sint-utest-ldflags           := $(test-ldflags) -lcute
check-sint-utest-path              := $(LIBEXECDIR)/cute/check-sint-utest

bins                               += check-uint-utest
check-uint-utest-objs              := check_uint.o
check-uint-utest-cflags            := $(test-cflags)
check-uint-utest-ldflags           := $(test-ldflags) -lcute
check-uint-utest-path              := $(LIBEXECDIR)/cute/check-uint-utest

bins                               += check-flt-utest
check-flt-utest-objs               := check_flt.o
check-flt-utest-cflags             := $(test-cflags)
check-flt-utest-ldflags            := $(test-ldflags) -lcute
check-flt-utest-path               := $(LIBEXECDIR)/cute/check-flt-utest

bins                               += check-dbl-utest
check-dbl-utest-objs               := check_dbl.o
check-dbl-utest-cflags             := $(test-cflags)
check-dbl-utest-ldflags            := $(test-ldflags) -lcute
check-dbl-utest-path               := $(LIBEXECDIR)/cute/check-dbl-utest

bins                               += expect-sint-utest
expect-sint-utest-objs             := expect_sint.o
expect-sint-utest-cflags           := $(test-cflags)
expect-sint-utest-ldflags          := $(test-ldflags) -lcute
expect-sint-utest-path             := $(LIBEXECDIR)/cute/expect-sint-utest

bins                               += expect-uint-utest
expect-uint-utest-objs             := expect_uint.o
expect-uint-utest-cflags           := $(test-cflags)
expect-uint-utest-ldflags          := $(test-ldflags) -lcute
expect-uint-utest-path             := $(LIBEXECDIR)/cute/expect-uint-utest

bins                               += expect-flt-utest
expect-flt-utest-objs              := expect_flt.o
expect-flt-utest-cflags            := $(test-cflags)
expect-flt-utest-ldflags           := $(test-ldflags) -lcute
expect-flt-utest-path              := $(LIBEXECDIR)/cute/expect-flt-utest

bins                               += expect-assert-utest
expect-assert-utest-objs           := expect_assert.o
expect-assert-utest-cflags         := $(test-cflags)
expect-assert-utest-ldflags        := $(test-ldflags) -lcute
expect-assert-utest-path           := $(LIBEXECDIR)/cute/expect-assert-utest

build: $(BUILDDIR)/cute-utest.sh

$(BUILDDIR)/cute-utest.sh: $(SRCDIR)/cute-utest.sh | $(BUILDDIR)
	sed 's;@@LIBEXECDIR@@;$(LIBEXECDIR);g' $(<) > $(@)

install: $(BINDIR)/cute-utest.sh \
         $(foreach b,$(bins),$(LIBEXECDIR)/cute/$(b)-outref.txt)

$(BINDIR)/cute-utest.sh: $(BUILDDIR)/cute-utest.sh | $(BINDIR)
	$(call install_recipe,--mode=755,$(<),$(@))

$(LIBEXECDIR)/cute/%-utest-outref.txt: $(SRCDIR)/%-outref.txt
	$(call install_recipe,--mode=644,$(<),$(@))

uninstall: uninstall-tests

uninstall-tests:
	$(foreach b, \
	          $(bins), \
	          $(call rmr_recipe, \
	                 $(LIBEXECDIR)/cute/$(b)-outref.txt)$(newline))
	$(call rm_recipe,$(BINDIR)/cute-utest.sh)

# vim: filetype=make :
