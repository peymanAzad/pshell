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
void free_node(SyntaxNode *node) {
    while (node->next != NULL) {
        SyntaxNode *n = node->next;
        node->next = node->next->next;
        free_node(n);
    }
    if (node->commands)
        free_node(node->commands);
    if (node->prefixes)
        free_node(node->prefixes);
    if (node->suffixes)
        free_node(node->suffixes);
    if (node->redirects)
        free_node(node->redirects);
    if (node->value)
        freebuf(node->value);
    if (node->name)
        freebuf(node->name);
    free(node);
}

Parser *alloc_parser(char *input) {
    Parser *p = (Parser *)malloc(sizeof(Parser));
    assert(p != NULL);
    Token *t = alloc_token();
    *p = (Parser){
        .current = t, .input = input, .has_current = false, .cursor = 0};
    return p;
}

void free_parser(Parser *p) {
    free_token(p->current);
    free(p);
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

SyntaxNode *proccess_assignment(Parser *p) {
    peek(p);
    assert(p->current->value != NULL);
    size_t cursor = 0;
    Buffer *name = buffstrtok(p->current->value, '=', &cursor);
    Buffer *value =
        slicebuf(p->current->value, cursor + 1, p->current->value->len);
    assert(name != NULL);
    assert(value != NULL);
    SyntaxNode *node = alloc_node(assignment);
    node->value = value;
    node->name = name;
    advance(p);
    return node;
}

SyntaxNode *proccess_arg(Parser *p) {
    peek(p);
    assert(p->current->value != NULL);
    SyntaxNode *node = alloc_node(argument);
    Buffer *valbuf = initbuf(p->current->value->data);
    node->value = valbuf;
    advance(p);
    return node;
}

//* redirect    → (GT | DGT | LT) WORD
SyntaxNode *proccess_redirect(Parser *p) {
    Token *current = peek(p);
    assert(current != NULL);
    assert(current->type == gt || current->type == dgt || current->type == lt);
    advance(p);
    Token *next = peek(p);
    assert(next->type == word);
    SyntaxNode *node = alloc_node(redirects);
    node->ttype = current->type;
    node->value = initbuf(next->value->data);
    advance(p);
    return node;
}
