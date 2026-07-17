#include "pshell.h"
#include <stdio.h>
#define MAX 128

int main() {
    char c;
    char comm[1024];
    while ((c = getchar()) != EOF) {
        int i = 0;
        while (c != '\n') {
            comm[i++] = c;
            c = getchar();
        }
        comm[i] = '\0';
        Parser *p = alloc_parser(comm);
        SyntaxNode *ast = proccess_script(p);
        exec_script(ast);
    }
}
