CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic -ggdb -MMD -MP -D_POSIX_C_SOURCE=200809L -Iinclude

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

LIBSRCS = tokenizer.c buffer.c parser.c interpreter.c builtins.c runner.c
LIBOBJS = $(addprefix $(BUILD_DIR)/, $(LIBSRCS:.c=.o))

PSHELLOBJS = $(BUILD_DIR)/main.o $(LIBOBJS)

TESTSRCS = main.c tokenizer.test.c buffer.test.c parser.test.c interpreter.test.c expand.test.c
TESTOBJS = $(addprefix $(BUILD_DIR)/tests/, $(TESTSRCS:.c=.o)) $(LIBOBJS)

.PHONY: all clean

all: pshell

pshell: $(PSHELLOBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf pshell test $(BUILD_DIR)

-include $(PSHELLOBJS:.o=.d) $(TESTOBJS:.o=.d)
