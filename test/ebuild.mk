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

bins                               += simple-pass-utest
simple-pass-utest-objs             := simple_pass.o
simple-pass-utest-cflags           := $(sample-cflags)
simple-pass-utest-ldflags          := $(sample-ldflags) -lcute
simple-pass-utest-path             := $(LIBEXECDIR)/cute/simple-pass-utest

bins                               += simple-fail-utest
simple-fail-utest-objs             := simple_fail.o
simple-fail-utest-cflags           := $(sample-cflags)
simple-fail-utest-ldflags          := $(sample-ldflags) -lcute
simple-fail-utest-path             := $(LIBEXECDIR)/cute/simple-fail-utest

bins                               += simple-fail-setup-utest
simple-fail-setup-utest-objs       := simple_fail_setup.o
simple-fail-setup-utest-cflags     := $(sample-cflags)
simple-fail-setup-utest-ldflags    := $(sample-ldflags) -lcute
simple-fail-setup-utest-path       := $(LIBEXECDIR)/cute/simple-fail-setup-utest

bins                               += simple-fail-teardown-utest
simple-fail-teardown-utest-objs    := simple_fail_teardown.o
simple-fail-teardown-utest-cflags  := $(sample-cflags)
simple-fail-teardown-utest-ldflags := $(sample-ldflags) -lcute
simple-fail-teardown-utest-path    := $(LIBEXECDIR)/cute/simple-fail-teardown-utest

bins                               += simple-fixture-exec-utest
simple-fixture-exec-utest-objs     := simple_fixture_exec.o
simple-fixture-exec-utest-cflags   := $(sample-cflags)
simple-fixture-exec-utest-ldflags  := $(sample-ldflags) -lcute
simple-fixture-exec-utest-path     := $(LIBEXECDIR)/cute/simple-fixture-exec-utest

bins                               += simple-except-utest
simple-except-utest-objs           := simple_except.o
simple-except-utest-cflags         := $(sample-cflags)
simple-except-utest-ldflags        := $(sample-ldflags) -lcute
simple-except-utest-path           := $(LIBEXECDIR)/cute/simple-except-utest

bins                               += simple-except-setup-utest
simple-except-setup-utest-objs     := simple_except_setup.o
simple-except-setup-utest-cflags   := $(sample-cflags)
simple-except-setup-utest-ldflags  := $(sample-ldflags) -lcute
simple-except-setup-utest-path     := $(LIBEXECDIR)/cute/simple-except-setup-utest

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
