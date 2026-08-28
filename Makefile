CC ?= cc
CFLAGS ?= -std=gnu99 -Wall -Wextra -Werror -O2
TEST_FLAGS = -DMERC -Dunix -Dlinux -ffunction-sections -fdata-sections -Isrc
LINK_FLAGS = -Wl,--gc-sections

ifeq ($(shell uname -s),Darwin)
LINK_FLAGS = -Wl,-dead_strip -Wl,-undefined,dynamic_lookup
endif
BUILD_DIR = .build
TEST_BINARY = $(BUILD_DIR)/test_bit

.PHONY: test clean

test: $(TEST_BINARY)
	$(TEST_BINARY)

$(TEST_BINARY): tests/test_bit.c src/bit.c src/merc.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_FLAGS) tests/test_bit.c src/bit.c $(LINK_FLAGS) -o $(TEST_BINARY)

clean:
	rm -rf $(BUILD_DIR)
