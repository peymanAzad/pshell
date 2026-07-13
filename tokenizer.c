#include "pshell.h"
#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Token *alloc_token() {
    Token *t = (Token *)malloc(sizeof(Token));
    assert(t != NULL);
    return t;
}

void free_token(Token *t) {
    if (t->value)
        freebuf(t->value);
    free(t);
}

int get_to_char(Buffer *b, char *input, char ch) {
    char c;
    int wcount = 0;
    for (wcount = 0; (c = *input++) != '\0'; ++wcount) {
        if (c == ch) {
            ++wcount;
            break;
        }
        int count = pushcbuf(b, c);
        assert(count == 1);
    }
    return wcount;
}

size_t next_token(Token *token, char *input, size_t cursor) {
    char c;
    while ((c = input[cursor++]) != '\0') {
        switch (c) {
        case ';':
        case '\n':
            *token = (Token){.type = newline, .value = NULL};
            return cursor;

        case '|':
            *token = (Token){.type = pip, .value = NULL};
            return cursor;

        case '<':
            *token = (Token){.type = lt, .value = NULL};
            return cursor;

        case '>':
            if (input[cursor] == '>') {
                *token = (Token){.type = dgt, .value = NULL};
                ++cursor;
            } else
                *token = (Token){.type = gt, .value = NULL};
            return cursor;

        case '&':
            if (input[cursor] == '&') {
                *token = (Token){.type = andd, .value = NULL};
                return ++cursor;
            }
            break;

        case '\\':
            if (input[cursor] == '\n')
                ++cursor;
            break;

        default:
            if (isspace(c))
                continue;

            Buffer *tokenval = allocbuf();
            do {
                if (c == '\'' || c == '"')
                    cursor += get_to_char(tokenval, input + cursor, c);
                else
                    pushcbuf(tokenval, c);
            } while ((c = input[cursor++]) != '\0' && !isspace(c));
            *token = (Token){.type = word, .value = tokenval};
            return cursor - 1;
        }
    }
    return 0;
}

int tokenize_all(Token *result, char *input) {
    size_t cursor = 0;
    int count;
    for (count = 0; (cursor = next_token(result++, input, cursor)) != 0;
         ++count)
        ;
    return count;
}
