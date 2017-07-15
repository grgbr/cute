SRC   := $(CURDIR)/src
TEST  := $(CURDIR)/test
BUILD := $(CURDIR)/build

CFLAGS := -Wall -Wextra -D_GNU_SOURCE -MD -ggdb3 -I$(CURDIR)/include

AR := gcc-ar

test_bin := cute_test_success cute_test_abort cute_test_freeze cute_test_segv \
            cute_test_term cute_test_fail cute_test_assert cute_test_root_suite
lib_src  := core.c posix.c report.c

.PHONY: build
build: $(BUILD)/libcute.a

.PHONY: test
test: $(addprefix $(BUILD)/,$(test_bin))

.PHONY: clean
clean:
	$(RM) -r $(BUILD)

$(BUILD)/%: $(TEST)/%.c $(BUILD)/libcute.a
	$(CC) $(CFLAGS) -L$(BUILD) -o $@ $(filter %.c,$^) -lcute

$(BUILD)/%.a: $(addprefix $(BUILD)/,$(lib_src:.c=.o))
	$(AR) crs $@ $(filter-out %.h,$^)

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir -p $@

-include $(wildcard $(BUILD)/*.d)
