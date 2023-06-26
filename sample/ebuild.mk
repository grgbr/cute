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
sample-ldflags            := $(common-cflags) \
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

#bins                       += cute-ensure-sample
#cute-ensure-sample-objs    := ensure.o
#cute-ensure-sample-cflags  := $(sample-cflags)
#cute-ensure-sample-ldflags := $(sample-ldflags) -lcute

# vim: filetype=make :
