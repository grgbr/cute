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
                             -I $(TOPDIR)/include \
                             $(filter-out -DNDEBUG,$(EXTRA_CFLAGS))
sample-ldflags            := $(common-cflags) \
                             -L$(BUILDDIR)/../src \
                             $(filter-out -DNDEBUG,$(EXTRA_LDFLAGS)) \
                             -Wl,-z,start-stop-visibility=hidden

bins                               += simple-exec-pass-utest
simple-exec-pass-utest-objs        := simple_exec_pass.o
simple-exec-pass-utest-cflags      := $(sample-cflags)
simple-exec-pass-utest-ldflags     := $(sample-ldflags) -lcute
simple-exec-pass-utest-path        := $(LIBEXECDIR)/cute/simple-exec-pass-utest

bins                               += simple-exec-fail-utest
simple-exec-fail-utest-objs        := simple_exec_fail.o
simple-exec-fail-utest-cflags      := $(sample-cflags)
simple-exec-fail-utest-ldflags     := $(sample-ldflags) -lcute
simple-exec-fail-utest-path        := $(LIBEXECDIR)/cute/simple-exec-fail-utest

bins                               += simple-exec-error-utest
simple-exec-error-utest-objs       := simple_exec_error.o
simple-exec-error-utest-cflags     := $(sample-cflags)
simple-exec-error-utest-ldflags    := $(sample-ldflags) -lcute
simple-exec-error-utest-path       := $(LIBEXECDIR)/cute/simple-exec-error-utest

bins                               += simple-setup-fail-utest
simple-setup-fail-utest-objs       := simple_setup_fail.o
simple-setup-fail-utest-cflags     := $(sample-cflags)
simple-setup-fail-utest-ldflags    := $(sample-ldflags) -lcute
simple-setup-fail-utest-path       := $(LIBEXECDIR)/cute/simple-setup-fail-utest

bins                               += simple-setup-error-utest
simple-setup-error-utest-objs      := simple_setup_error.o
simple-setup-error-utest-cflags    := $(sample-cflags)
simple-setup-error-utest-ldflags   := $(sample-ldflags) -lcute
simple-setup-error-utest-path      := $(LIBEXECDIR)/cute/simple-setup-error-utest

bins                               += simple-teardown-fail-utest
simple-teardown-fail-utest-objs    := simple_teardown_fail.o
simple-teardown-fail-utest-cflags  := $(sample-cflags)
simple-teardown-fail-utest-ldflags := $(sample-ldflags) -lcute
simple-teardown-fail-utest-path    := $(LIBEXECDIR)/cute/simple-teardown-fail-utest

bins                               += simple-fixture-fail-utest
simple-fixture-fail-utest-objs     := simple_fixture_fail.o
simple-fixture-fail-utest-cflags   := $(sample-cflags)
simple-fixture-fail-utest-ldflags  := $(sample-ldflags) -lcute
simple-fixture-fail-utest-path     := $(LIBEXECDIR)/cute/simple-fixture-fail-utest

bins                               += check-sint-utest
check-sint-utest-objs             := check_sint.o
check-sint-utest-cflags           := $(sample-cflags)
check-sint-utest-ldflags          := $(sample-ldflags) -lcute
check-sint-utest-path             := $(LIBEXECDIR)/cute/check-sint-utest

bins                              += check-uint-utest
check-uint-utest-objs             := check_uint.o
check-uint-utest-cflags           := $(sample-cflags)
check-uint-utest-ldflags          := $(sample-ldflags) -lcute
check-uint-utest-path             := $(LIBEXECDIR)/cute/check-uint-utest

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
