#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#define module "Interpret"

void test_exec_command() {
    static char *name = "ExecComm";
    Parser *p = alloc_parser("TZ=Pacific/Auckland date > ./test.txt");
    SyntaxNode *comm = proccess_command(p);
    assert(exec_command(comm) == 0);
    free_node(comm);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
