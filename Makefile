CC = gcc
BASE_CFLAGS = -Wall -Wextra -std=c11 -pedantic -MMD -MP -D_POSIX_C_SOURCE=200809L -Iinclude

DEBUG_CFLAGS = $(BASE_CFLAGS) -Werror -ggdb -O0 -DDEBUG -fsanitize=address,undefined
RELEASE_CFLAGS = $(BASE_CFLAGS) -O2 -DNDEBUG -flto

BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

LIBSRCS = tokenizer.c buffer.c parser.c interpreter.c builtins.c runner.c
TESTSRCS = main.c tokenizer.test.c buffer.test.c parser.test.c interpreter.test.c expand.test.c

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

.PHONY: all debug release test clean install uninstall

all: debug

DEBUG_DIR = $(BUILD_DIR)/debug
DEBUG_LIBOBJS = $(addprefix $(DEBUG_DIR)/, $(LIBSRCS:.c=.o))
DEBUG_OBJS = $(DEBUG_DIR)/main.o $(DEBUG_LIBOBJS)

debug: $(DEBUG_DIR)/pshell

$(DEBUG_DIR)/pshell: $(DEBUG_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -o $@ $^

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

RELEASE_DIR = $(BUILD_DIR)/release
RELEASE_LIBOBJS = $(addprefix $(RELEASE_DIR)/, $(LIBSRCS:.c=.o))
RELEASE_OBJS = $(RELEASE_DIR)/main.o $(RELEASE_LIBOBJS)

release: $(RELEASE_DIR)/pshell

$(RELEASE_DIR)/pshell: $(RELEASE_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -o $@ $^

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -c $< -o $@

TEST_DIR_BUILD = $(BUILD_DIR)/test
TESTOBJS = $(addprefix $(TEST_DIR_BUILD)/, $(TESTSRCS:.c=.o)) $(DEBUG_LIBOBJS)

test: $(TEST_DIR_BUILD)/test
	./$(TEST_DIR_BUILD)/test

$(TEST_DIR_BUILD)/test: $(TESTOBJS)
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -o $@ $^

$(TEST_DIR_BUILD)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

install: release
	install -Dm755 $(RELEASE_DIR)/pshell $(BINDIR)/pshell
	install -Dm644 man/pshell.1 $(MANDIR)/pshell.1

uninstall:
	rm -f $(BINDIR)/pshell
	rm -f $(MANDIR)/pshell.1

clean:
	rm -rf $(BUILD_DIR)

-include $(DEBUG_OBJS:.o=.d) $(RELEASE_OBJS:.o=.d) $(TESTOBJS:.o=.d)
