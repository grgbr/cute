SRC   := $(CURDIR)/src
TEST  := $(CURDIR)/test
BUILD := $(CURDIR)/build

CFLAGS := -Wall -Wextra -D_GNU_SOURCE -DNDEBUG -MD -I$(CURDIR)/include -O2

TARGET_CC ?= gcc
TARGET_AR ?= gcc-ar

test_bin := cute_test_success cute_test_abort cute_test_freeze cute_test_segv \
            cute_test_term cute_test_fail cute_test_assert \
            cute_test_root_suite cute_test_complex_suite cute_test_pnp_suite
lib_src  := core.c posix.c report.c

.PHONY: build
build: $(BUILD)/libcute.a $(BUILD)/libcute.so

.PHONY: test
test: $(addprefix $(BUILD)/static/,$(test_bin)) \
      $(addprefix $(BUILD)/shared/,$(test_bin))

.PHONY: clean
clean:
	find build/ -name "*.[od]" -delete

# Build test with static libcute (use shared version for all other libs )
$(BUILD)/static/%: $(BUILD)/%.o $(BUILD)/libcute.a
	$(TARGET_CC) $(CFLAGS) -L$(BUILD) -Wl,--strip-all -T $(SRC)/cute.ld \
		-o $@ $(filter %.o,$^) -Wl,-static -lcute -Wl,-Bdynamic

# Build tests linked with shared libraries
$(BUILD)/shared/%: $(BUILD)/%.o $(BUILD)/libcute.so
	$(TARGET_CC) $(CFLAGS) -L$(BUILD) -Wl,--strip-all -T $(SRC)/cute.ld \
		-o $@ $(filter %.o,$^) -Wl,-Bdynamic -lcute

$(BUILD)/%.o: $(TEST)/%.c | $(BUILD)
	$(TARGET_CC) $(CFLAGS) -o $@ -c $<

$(BUILD)/%.a: $(addprefix $(BUILD)/static/,$(lib_src:.c=.o))
	$(TARGET_AR) crs $@ $(filter-out %.h,$^)

$(BUILD)/static/%.o: $(SRC)/%.c | $(BUILD)/static
	$(TARGET_CC) $(CFLAGS) -o $@ -c $<

$(BUILD)/%.so: $(addprefix $(BUILD)/shared/,$(lib_src:.c=.o))
	$(TARGET_CC) $(CFLAGS) -shared -fpic -o $@ $(filter-out %.h,$^)

$(BUILD)/shared/%.o: $(SRC)/%.c | $(BUILD)/shared
	$(TARGET_CC) $(CFLAGS) -fpic -o $@ -c $<

$(BUILD) $(BUILD)/static $(BUILD)/shared:
	mkdir -p $@

.SECONDARY:

-include $(wildcard $(BUILD)/static/*.d)
-include $(wildcard $(BUILD)/shared/*.d)
