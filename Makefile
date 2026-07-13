CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic -ggdb -MMD -MP -D_POSIX_C_SOURCE=200809L
LIBOBJS = tokenizer.o buffer.o parser.o interpreter.o
PSHELLOBJS = main.o $(LIBOBJS)
TESTOBJS = tests/main.o tests/tokenizer.test.o tests/buffer.test.o tests/parser.test.o tests/interpreter.test.o $(LIBOBJS)

.PHONY: all clean

all: pshell

pshell: $(PSHELLOBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TESTOBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f \
		pshell test \
		$(PSHELLOBJS) $(TESTOBJS) \
		$(PSHELLOBJS:.o=.d) $(TESTOBJS:.o=.d)

-include $(PSHELLOBJS:.o=.d) $(TESTOBJS:.o=.d)
