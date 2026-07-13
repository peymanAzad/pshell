#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define module "Interpret"

void test_exec_command() {
    static char *name = "ExecComm";
    Parser *p = alloc_parser("TZ=Pacific/Auckland date > ./test.txt");
    SyntaxNode *comm = proccess_command(p);
    pid_t pid = fork();
    if (pid == 0) {
        exec_command(comm);
    }
    int stat;
    waitpid(pid, &stat, 0);
    int statcode = WIFEXITED(stat) ? WEXITSTATUS(stat) : -1;
    assert(statcode == 0);
    free_node(comm);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_exec_pipe() {
    static char *name = "ExecPipe";
    Parser *p = alloc_parser("ls | sort -hr | sed 's/\\./*/g' > test.txt");
    SyntaxNode *pipe_node = proccess_pipline(p);
    int stat = exec_pipeline(pipe_node);
    assert(stat == 0);
    free_node(pipe_node);
    free_parser(p);
    p = alloc_parser("foo=bar");
    pipe_node = proccess_pipline(p);
    stat = exec_pipeline(pipe_node);
    char *enval = getenv("foo");
    assertstr(enval, "bar");
    free_node(pipe_node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
