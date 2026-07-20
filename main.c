#include "pshell.h"

int main(int argc, char **argv) {
    int stat = 0;
    if (argc < 2)
        stat = run_repl();
    else
        stat = run_file(argv[1]);
    return stat;
}
