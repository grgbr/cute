config-in   := Config.in
config-h    := $(PACKAGE)/config.h

HEADERDIR   := $(CURDIR)/include
headers     += $(PACKAGE)/cute.h
headers     += $(PACKAGE)/ensure.h

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

# vim: filetype=make :
