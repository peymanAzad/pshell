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

int exec_command_toplevel(SyntaxNode *comm) {
    assert(comm->type == command);
    builtin_fn bfn = find_builtin(comm->value->data);
    if (bfn != NULL) {
        return bfn(comm);
    }
    pid_t pid = fork();
    if (pid == 0) {
        exec_command(comm);
    }
    int stat;
    waitpid(pid, &stat, 0);
    return WIFEXITED(stat) ? WEXITSTATUS(stat) : -1;
}

void exec_command(SyntaxNode *comm) {
    assert(comm->type == command);
    size_t argc = sizeof_list(comm->suffixes, argument);
    char *argv[argc + 2];
    list_to_arr(argv + 1, comm->suffixes, argc, argument);
    argv[0] = comm->value->data;
    argv[argc + 1] = NULL;
    set_prefix_envs(comm->prefixes);

    builtin_fn bfn = find_builtin(comm->value->data);
    if (bfn != NULL) {
        int stat = bfn(comm);
        _exit(stat);
    }

    if (handle_redirects(comm) == -1) {
        fprintf(stderr, "error while setting redirects\n");
        _exit(127);
    }
    execvp(argv[0], argv);
    perror("execvp");
    _exit(127);
}

int exec_pipeline(SyntaxNode *pipe_node) {
    assert(pipe_node->type == pipeline);
    size_t comm_count = sizeof_list(pipe_node->commands, command);
    int pips[comm_count - 1][2];
    for (size_t i = 0; i < comm_count - 1; ++i) {
        pipe(pips[i]);
    }

    pid_t pids[comm_count];
    SyntaxNode *comm = pipe_node->commands;

    if (comm_count == 1) {
        if (comm->value == NULL) {
            set_prefix_envs(comm->prefixes);
            return 0;
        }
        return exec_command_toplevel(comm);
    }

    for (size_t i = 0; i < comm_count; ++i, comm = comm->next) {
        if (comm->value == NULL) {
            set_prefix_envs(comm->prefixes);
            pids[i] = -1;
            continue;
        }
        pids[i] = fork();
        if (pids[i] == 0) {
            if (i >= 1)
                dup2(pips[i - 1][0], STDIN_FILENO);
            if (i < comm_count - 1)
                dup2(pips[i][1], STDOUT_FILENO);
            for (size_t j = 0; j < comm_count - 1; ++j) {
                close(pips[j][0]);
                close(pips[j][1]);
            }
            exec_command(comm);
        }
    }
    for (size_t i = 0; i < comm_count - 1; ++i) {
        close(pips[i][0]);
        close(pips[i][1]);
    }

    int last_stat;
    for (size_t i = 0; i < comm_count; ++i) {
        int stat;
        if (pids[i] == -1)
            stat = 0;
        else
            waitpid(pids[i], &stat, 0);
        if (i == comm_count - 1)
            last_stat = WIFEXITED(stat) ? WEXITSTATUS(stat) : -1;
    }
    return last_stat;
}

int exec_statement(SyntaxNode *state_node) {
    int stat = 0;
    for (SyntaxNode *cursor = state_node->commands; cursor != NULL;
         cursor = cursor->next) {
        assert(cursor->type == pipeline);
        stat = exec_pipeline(cursor);
        if (stat != 0)
            break;
    }
    return stat;
}

int exec_script(SyntaxNode *script) {
    int stat = 0;
    for (SyntaxNode *cursor = script->commands; cursor != NULL;
         cursor = cursor->next) {
        assert(cursor->type == statement);
        stat = exec_statement(cursor);
    }
    return stat;
}
