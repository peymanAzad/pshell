CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic -ggdb

pshell: pshell.h main.c tokenizer.c
	$(CC) $(CFLAGS) -o pshell main.c tokenizer.c
