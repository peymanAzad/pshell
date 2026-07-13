#include "pshell.h"
#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

size_t sizeof_list(SyntaxNode *node, SyntaxType type) {
    size_t c = 0;
    for (SyntaxNode *cursor = node; cursor != NULL; cursor = cursor->next) {
        if (cursor->type == type)
            ++c;
    }
    return c;
}

size_t list_to_arr(char *arr[], SyntaxNode *node, size_t size,
                   SyntaxType type) {
    size_t c = 0;
    for (SyntaxNode *cursor = node; cursor != NULL && c < size;
         cursor = cursor->next) {
        if (cursor->type == type)
            arr[c++] = cursor->value->data;
    }
    return c;
}

size_t set_prefix_envs(SyntaxNode *node) {
    size_t c = 0;
    for (SyntaxNode *cursor = node; cursor != NULL;
         cursor = cursor->next, ++c) {
        if (cursor->type == assignment) {
            setenv(cursor->name->data, cursor->value->data, 1);
        }
    }
    return c;
}

int handle_redirect(SyntaxNode *node) {
    assert(node->type == redirects);
    int fd;
    switch (node->ttype) {
    case gt:
        fd = open(node->value->data, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
        break;
    case dgt:
        fd = open(node->value->data, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("open");
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
        break;
    case lt:
        fd = open(node->value->data, O_RDONLY);
        if (fd == -1) {
            perror("open");
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
        break;
    default:
        return -1;
    }
    return 0;
}

int handle_redirects(SyntaxNode *comm) {
    SyntaxNode *cursor;
    SyntaxNode *list[] = {comm->prefixes, comm->suffixes};
    for (int i = 0; i < 2; ++i)
        for (cursor = list[i]; cursor != NULL; cursor = cursor->next)
            if (cursor->type == redirects)
                if (handle_redirect(cursor) == -1)
                    return -1;

    return 0;
}

int exec_command(SyntaxNode *comm) {
    assert(comm->type == command);
    assert(comm->value->data);
    pid_t pid = fork();
    if (pid == 0) {
        size_t argc = sizeof_list(comm->suffixes, argument);
        char *argv[argc + 2];
        list_to_arr(argv + 1, comm->suffixes, argc, argument);
        argv[0] = comm->value->data;
        argv[argc + 1] = NULL;
        set_prefix_envs(comm->prefixes);
        if (handle_redirects(comm) == -1) {
            fprintf(stderr, "error while setting redirects\n");
            _exit(127);
        }
        execvp(argv[0], argv);
        perror("execvp");
        _exit(127);
    }
    int stat;
    waitpid(pid, &stat, 0);
    return WIFEXITED(stat) ? WEXITSTATUS(stat) : -1;
}
