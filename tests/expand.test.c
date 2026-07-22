#include "../pshell.h"
#include <assert.h>
#include <stdlib.h>

#define module "Expand"

void test_expand_simple() {
    static char *name = "simple";
    setenv("foo", "bar", 1);
    Buffer *b = allocbuf();
    pushbuf(b, "$foo");
    Buffer *expanded = expand_value(b);
    assert(expanded->len == 3);
    assert(expanded->cap == 16);
    assert(strcmp(expanded->data, "bar") == 0);
    free(b);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_expand() {
    static char *name = "comp";
    setenv("foo", "bar", 1);
    setenv("bar", "baz", 1);
    Buffer *b = allocbuf();
    pushbuf(b, "foo=$foo $bar > test_$bar.txt");
    Buffer *expanded = expand_value(b);
    assert(strcmp(expanded->data, "foo=bar baz > test_baz.txt") == 0);
    free(b);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_expand_undef() {
    static char *name = "undefined";
    unsetenv("foo");
    Buffer *b = allocbuf();
    pushbuf(b, "foo=$foo");
    Buffer *expanded = expand_value(b);
    assert(strcmp(expanded->data, "foo=") == 0);
    free(b);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_expand_empty() {
    static char *name = "empty";
    Buffer *b = allocbuf();
    pushbuf(b, "");
    Buffer *expanded = expand_value(b);
    assert(strcmp(expanded->data, "") == 0);
    free(b);
    b = allocbuf();
    pushbuf(b, "echo hello");
    expanded = expand_value(b);
    assert(strcmp(expanded->data, "echo hello") == 0);
    free(b);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
