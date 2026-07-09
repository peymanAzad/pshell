#include "pshell.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

SyntaxNode *alloc_node(SyntaxType type) {
    SyntaxNode *node = (SyntaxNode *)malloc(sizeof(SyntaxNode));
    assert(node != NULL);
    *node = (SyntaxNode){.type = type,
                         .prefixes = NULL,
                         .suffixes = NULL,
                         .commands = NULL,
                         .redirects = NULL,
                         .value = NULL,
                         .name = NULL,
                         .next = NULL};
    return node;
}

Parser *alloc_parser(char *input) {
    Parser *p = (Parser *)malloc(sizeof(Parser));
    assert(p != NULL);
    *p = (Parser){
        .current = NULL, .input = input, .has_current = false, .cursor = 0};
    return p;
}

Token *peek(Parser *p) {
    if (!p->has_current) {
        p->cursor = next_token(p->current, p->input, p->cursor);
        p->has_current = true;
    }
    return p->current;
}
Token *advance(Parser *p) {
    Token *t = peek(p);
    p->has_current = false;
    return t;
}
bool check(Parser *p, TokenType ttype) {
    Token *t = peek(p);
    return t != NULL && t->type == ttype;
}
