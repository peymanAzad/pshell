#include "pshell.h"

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--version") == 0) {
        printf("pshell %s\n", PSHELL_VERSION);
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("usage: pshell [script-file]\n");
        return 0;
    }

    int stat = 0;
    if (argc < 2)
        stat = run_repl();
    else
        stat = run_file(argv[1]);
    return stat;
}
