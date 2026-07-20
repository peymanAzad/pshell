#ifndef P_SHELL_H
#define P_SHELL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD_LEN 16
#define BUFF_SIZE 16
#define assertstr(x, y) assert(strcmp((x), (y)) == 0)

/*====================================================
 * Buffer
 *====================================================*/
typedef struct Buffer {
    char *data;
    size_t len;
    size_t cap;
} Buffer;

Buffer *allocbuf();
Buffer *initbuf(char *data);
void freebuf(Buffer *);
int pushbuf(Buffer *buff, char *data);
int pushcbuf(Buffer *buff, char c);
Buffer *buffstrtok(Buffer *buff, char c, size_t *cursor);
Buffer *buffstrtokspace(Buffer *buff, size_t *cursor);
Buffer *slicebuf(Buffer *buff, size_t start, size_t end);

/*====================================================
 * Tokenizer
 *====================================================*/
typedef enum TokenType { word, gt, dgt, lt, andd, pip, newline } TokenType;

typedef struct Token {
    TokenType type;
    Buffer *value;
} Token;

Token *alloc_token();
void free_token(Token *t);
size_t next_token(Token *token, char *input, size_t cursor);
int tokenize_all(Token *tokens, char *input);

/*====================================================
 * Parser
 *====================================================*/
typedef enum SyntaxType {
    script,
    statement,
    command,
    assignment,
    argument,
    redirects,
    pipeline
} SyntaxType;

typedef struct SyntaxNode {
    SyntaxType type;
    TokenType ttype; // only used for redirect value: gt, dgt, lt
    Buffer *value;
    Buffer *name; // only used for assignments
    struct SyntaxNode *suffixes;
    struct SyntaxNode *prefixes;
    struct SyntaxNode *redirects;
    struct SyntaxNode *commands;
    struct SyntaxNode *next;
} SyntaxNode;

SyntaxNode *alloc_node(SyntaxType type);
void free_node(SyntaxNode *node);

typedef struct Parser {
    char *input;
    size_t cursor;
    Token *current;
    bool has_current;
    bool eof;
} Parser;

Parser *alloc_parser(char *input);
void free_parser(Parser *p);

Token *peek(Parser *p);
Token *advance(Parser *p);
bool check(Parser *p, TokenType ttype);

/*
 * script      → statement (NEWLINE statement)* NEWLINE?
 * statement   → pipeline (ANDD pipeline)*
 * pipeline    → command (PIPE command)*
 * command     → prefix* word suffix* | prefix+
 * prefix      → assignment | redirect
 * suffix      → argument | redirect
 * assignment  → WORD
 * argument    → WORD
 * redirect    → (GT | DGT | LT) WORD
 */
SyntaxNode *proccess_script(Parser *parser);
SyntaxNode *proccess_statement(Parser *parser);
SyntaxNode *proccess_pipline(Parser *parser);
SyntaxNode *proccess_command(Parser *parser);
SyntaxNode *proccess_prefix(Parser *parser);
SyntaxNode *proccess_suffix(Parser *parser);
SyntaxNode *proccess_redirect(Parser *parser);
SyntaxNode *proccess_arg(Parser *parser);
SyntaxNode *proccess_assignment(Parser *parser);

/*====================================================
 * Interprator
 *====================================================*/
int exec_command_toplevel(SyntaxNode *command);
void exec_command(SyntaxNode *command);
int exec_pipeline(SyntaxNode *pipe_node);
int exec_statement(SyntaxNode *state_node);
int exec_script(SyntaxNode *script);

/*====================================================
 * Built-ins
 *====================================================*/
int builtin_cd(SyntaxNode *comm);
int builtin_pwd(SyntaxNode *comm);
int builtin_exit(SyntaxNode *comm);

typedef int (*builtin_fn)(SyntaxNode *comm);
typedef struct Builtin {
    char *name;
    builtin_fn fn;
} Builtin;

builtin_fn find_builtin(char *name);

#endif
