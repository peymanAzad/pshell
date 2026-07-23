#include "pshell.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 4096

int eval(char *str) {
    Parser *p = alloc_parser(str);
    SyntaxNode *ast = proccess_script(p);
    int stat = 0;
    if (ast == NULL) {
        stat = 1;
    } else {
        stat = exec_script(ast);
        free_node(ast);
    }
    free_parser(p);
    return stat;
}

int run_repl(void) {
    int stat = 0;
    Buffer *buff = allocbuf();
    printf("Welcome to pshell, the simple interactive shell for POSIX\n");
    while (true) {
        char line[MAX_LINE];
        char *user = getenv("USER");
        char *pwd = getenv("PWD");
        if (buff->len > 0)
            printf("> ");
        else
            printf("%s@%s> ", user ? user : "?", pwd ? pwd : "?");
        fflush(stdout);
        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }
        int size;
        if ((size = strlen(line)) > 2 && line[size - 2] == '\\' &&
            line[size - 1] == '\n') {
            line[size - 2] = '\0';
            pushbuf(buff, line);
            continue;
        }
        pushbuf(buff, line);
        stat = eval(buff->data);
        freebuf(buff);
        buff = allocbuf();
    }
    return stat;
}

int run_file(char *path) {
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror("pshell");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *str = malloc(size + 1);
    fread(str, 1, size, f);
    str[size] = '\0';
    fclose(f);

    char *content = str;
    if (size > 2 && str[0] == '#' && str[1] == '!') {
        char *newline = strchr(str, '\n');
        content = newline != NULL ? newline + 1 : str + size;
    }

    int stat = eval(content);
    free(str);
    return stat;
}
