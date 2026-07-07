#ifndef P_SHELL_H
#define P_SHELL_H

#include <stddef.h>

#define MAX_WORD_LEN 16
#define BUFF_SIZE 16

/*====================================================
 * Buffer
 *====================================================*/
typedef struct Buffer {
    char *data;
    size_t len;
    size_t cap;
} Buffer;

int pushbuf(Buffer *buff, char *data);
int pushcbuf(Buffer *buff, char c);
Buffer *allocbuf();

/*====================================================
 * Tokenizer
 *====================================================*/
typedef enum TokenType {
    word,
    gt,
    dgt,
    lt,
    andd,
    pipe,
    newline
} TokenType;

typedef struct Token {
    TokenType type;
    Buffer *value;
} Token;

size_t next_token(Token *token, char *input, size_t cursor);
int tokenize_all(Token *tokens, char *input);


/*====================================================
 * Parser
 *====================================================*/
typedef enum SyntaxType {
    script, statement, command, assignment, andor, pipeline
} SyntaxType;

typedef struct SyntaxNode {
    SyntaxType type;
    Buffer *value;
    struct SyntaxNode *suffixes;
    struct SyntaxNode *prefixes;
    struct SyntaxNode *redirects;
    struct SyntaxNode *next;
} SyntaxNode;
#endif
