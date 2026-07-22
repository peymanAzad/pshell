#include "pshell.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int pushbuf(Buffer *buff, char *data) {
    int dlen = strlen(data);
    if (buff->len + dlen >= buff->cap) {
        int alloc_size = ((buff->len + dlen) / BUFF_SIZE + 1) * BUFF_SIZE;
        buff->data = (char *)realloc(buff->data, alloc_size);
        if (buff->data == NULL) {
            return -1;
        }
        buff->cap = alloc_size;
    }
    strcat(buff->data, data);
    buff->len += dlen;
    return dlen;
}

int pushcbuf(Buffer *buff, char c) {
    if (buff->len + 1 >= buff->cap) {
        buff->data = (char *)realloc(buff->data, buff->cap + BUFF_SIZE);
        if (buff->data == NULL)
            return -1;
        buff->cap += BUFF_SIZE;
    }
    buff->data[buff->len++] = c;
    buff->data[buff->len] = '\0';
    return 1;
}

Buffer *slicebuf(Buffer *buff, size_t start, size_t end) {
    Buffer *b = allocbuf();
    char str[end - start + 1];
    strncpy(str, buff->data + start, end - start);
    str[end - start] = '\0';
    pushbuf(b, str);
    return b;
}

Buffer *buffstrtok(Buffer *buff, char c, size_t *cursor) {
    for (; buff->data[*cursor] == c && *cursor < buff->len; ++*cursor)
        ;
    if (*cursor == buff->len)
        return NULL;
    Buffer *sliced = allocbuf();
    for (; buff->data[*cursor] != c && *cursor < buff->len; ++*cursor) {
        pushcbuf(sliced, buff->data[*cursor]);
    }
    return sliced;
}

Buffer *buffstrtokspace(Buffer *buff, size_t *cursor) {
    for (; isspace(buff->data[*cursor]) && *cursor < buff->len; ++*cursor)
        ;
    if (*cursor == buff->len)
        return NULL;
    Buffer *sliced = allocbuf();
    for (; !isspace(buff->data[*cursor]) && *cursor < buff->len; ++*cursor) {
        pushcbuf(sliced, buff->data[*cursor]);
    }
    return sliced;
}

Buffer *allocbuf(void) {
    Buffer *b = malloc(sizeof(Buffer));
    assert(b != NULL);
    b->data = calloc(BUFF_SIZE, sizeof(char));
    assert(b->data != NULL);
    b->cap = BUFF_SIZE;
    b->len = 0;
    return b;
}

void freebuf(Buffer *b) {
    if (b->data)
        free(b->data);
    free(b);
}

Buffer *initbuf(char *data) {
    Buffer *b = allocbuf();
    pushbuf(b, data);
    return b;
}
