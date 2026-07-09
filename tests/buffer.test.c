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

void test_buffer_pushc() {
    static char *name = "pushcbuff";
    Buffer *b = allocbuf();
    char *str = "hello world foo bar baz";
    for (size_t i = 0; i < strlen(str); ++i) {
        pushcbuf(b, str[i]);
    }
    assert(b->len == 23);
    assert(b->cap == 32);
    assert(strcmp(b->data, "hello world foo bar baz") == 0);
    free(b);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_buffer_strtok() {
    static char *name = "buffstrtok";
    char *str = "=hello=world=foo==";
    size_t cursor = 0;
    Buffer *b = initbuf("=");
    assert(buffstrtok(b, '=', &cursor) == NULL);
    free(b);
    cursor = 0;
    b = initbuf(str);
    assert(b->len == 18);
    for (int i = 0; i < 3; ++i) {
        Buffer *s = buffstrtok(b, '=', &cursor);
        assert(strcmp(s->data, i == 0   ? "hello"
                               : i == 1 ? "world"
                                        : "foo") == 0);
        free(s);
    }
    free(b);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_buffer_strtokspace() {
    static char *name = "buffstrtokspace";
    char *str = " hello  \t  world foo  ";
    size_t cursor = 0;
    Buffer *b = initbuf(" ");
    assert(buffstrtokspace(b, &cursor) == NULL);
    free(b);
    cursor = 0;
    b = initbuf(str);
    assert(b->len == 22);
    for (int i = 0; i < 3; ++i) {
        Buffer *s = buffstrtokspace(b, &cursor);
        assert(strcmp(s->data, i == 0   ? "hello"
                               : i == 1 ? "world"
                                        : "foo") == 0);
        free(s);
    }
    free(b);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}
