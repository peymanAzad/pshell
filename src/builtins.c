
#include "pshell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PATH_MAX 1024

static Builtin builtins[] = {{"cd", builtin_cd},
                             {"pwd", builtin_pwd},
                             {"exit", builtin_exit},
                             {NULL, NULL}};

int builtin_cd(SyntaxNode *comm) {
    if (comm->suffixes) {
        Buffer *expanded = expand_value(comm->suffixes->value);
        freebuf(comm->suffixes->value);
        comm->suffixes->value = expanded;
    }
    char *target =
        comm->suffixes ? comm->suffixes->value->data : getenv("HOME");
    if (chdir(target) == -1) {
        perror("cd");
        return 1;
    }
    char pwd[PATH_MAX];
    if (getcwd(pwd, PATH_MAX) != NULL)
        setenv("PWD", pwd, 1);
    return 0;
}

int builtin_pwd(SyntaxNode *comm) {
    (void)comm;
    char pwd[PATH_MAX];
    if (getcwd(pwd, PATH_MAX) != NULL)
        printf("%s\n", pwd);
    return 0;
}

int builtin_exit(SyntaxNode *comm) {
    int code = comm->suffixes ? atoi(comm->suffixes->value->data) : 0;
    exit(code);
}

builtin_fn find_builtin(char *name) {
    for (int i = 0; builtins[i].name != NULL; ++i) {
        if (strcmp(builtins[i].name, name) == 0) {
            return builtins[i].fn;
        }
    }
    return NULL;
}
