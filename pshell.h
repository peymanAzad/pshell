#ifndef P_SHELL_H
#define P_SHELL_H

#include <stddef.h>

#define MAX_WORD_LEN 16
#define BUFF_SIZE 16

typedef enum TokenType {
    word,
    qoute,
    dqoute,
    gt,
    dgt,
    lt,
    andd,
    pipe,
    backslash,
    newline
} TokenType;

typedef struct Token {
    TokenType type;
    char *value;
} Token;

typedef struct Buffer {
    char *data;
    size_t len;
    size_t cap;
} Buffer;

int pushbuf(Buffer *buff, char *data);
int pushcbuf(Buffer *buff, char c);
Buffer *allocbuf();

int tokenize(Token *result, char *input, size_t count);

#endif
