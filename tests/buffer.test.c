#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define module "Buffer"

void test_buff_alloc() {
    static char *name = "init";
    Buffer *b = allocbuf();
    pushbuf(b, "hello world");
    assert(b->len == 11);
    assert(b->cap == 16);
    assert(strcmp(b->data, "hello world") == 0);
    free(b);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_buffer_push() {
    static char *name = "pushbuff";
    Buffer *b = allocbuf();
    pushbuf(b, "hello world");
    pushbuf(b, "foobar baz");
    assert(b->len == 21);
    assert(b->cap == 32);
    assert(strcmp(b->data, "hello worldfoobar baz") == 0);
    free(b);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}
