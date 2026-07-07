#include "pshell.h"
#include <stdio.h>
#define MAX 128

int main(int argc, char **argv) {
    printf("argc is %d\n", argc);
    --argc;
    ++argv;
    while (--argc >= 0) {
        Token tokens[MAX];
        char *test = *argv;
        while (*test != '\0')
            putchar(*test++);
        printf("after\n");
        int count = tokenize_all(tokens, *argv++);
        for (int i = 0; i < count; ++i) {
            printf("token[%d]: %d -> %s\n", i + 1, tokens[i].type,
                   tokens[i].value->data);
        }
        printf("****\n");
    }
    return 0;
}
