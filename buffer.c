#include "pshell.h"
#include <stdlib.h>
#include <string.h>

int pushbuf(Buffer *buff, char *data) {
    int dlen = strlen(data);
    if (buff->len + dlen >= buff->cap) {
        int alloc_size =
            ((buff->len + dlen - buff->cap) / BUFF_SIZE + 1) * BUFF_SIZE;
        buff->data = (char *)realloc(buff->data, alloc_size);
        if (buff->data == NULL) {
            return -1;
        }
        buff->cap += alloc_size;
    }
    strcat(buff->data, data);
    buff->len += dlen;
    return dlen;
}

Buffer *allocbuf() {
    Buffer *b = malloc(sizeof(Buffer));
    if (b == NULL)
        return NULL;
    b->data = malloc(BUFF_SIZE);
    if (b->data == NULL)
        return NULL;
    b->cap = BUFF_SIZE;
    b->len = 0;
    return b;
}
