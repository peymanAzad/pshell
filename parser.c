#include "pshell.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define isredirect(type) ((type) == gt || (type) == dgt || (type) == lt)
#define isassign(token)                                                        \
    ((token) != NULL && (token)->type == word &&                               \
     (strchr((token)->value->data, '=') != NULL))
#define isprefix(token)                                                        \
    ((token) != NULL && ((isredirect((token)->type)) || (isassign(token))))
#define issuffix(token)                                                        \
    ((token) != NULL &&                                                        \
     ((isredirect((token)->type)) || ((token)->type == word)))
#define LOG_PREFIX "pshell: syntax error:"

void printerror(char *msg) { fprintf(stderr, "%s %s\n", LOG_PREFIX, msg); }

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
    if (node->next != NULL)
        free_node(node->next);
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
    *p = (Parser){.current = t,
                  .input = input,
                  .has_current = false,
                  .cursor = 0,
                  .eof = false};
    return p;
}

void free_parser(Parser *p) {
    if (p->current)
        free_token(p->current);
    free(p);
}

Token *peek(Parser *p) {
    if (p->eof)
        return NULL;
    if (!p->has_current) {
        if (p->current->value) {
            freebuf(p->current->value);
            p->current->value = NULL;
        }
        p->cursor = next_token(p->current, p->input, p->cursor);
        if (p->cursor == 0) {
            p->eof = true;
            p->current = NULL;
            p->has_current = false;
            return NULL;
        }
        p->has_current = true;
    }
    return p->current;
}

Token *advance(Parser *p) {
    p->has_current = false;
    Token *t = peek(p);
    return t;
}

bool check(Parser *p, TokenType ttype) {
    Token *t = peek(p);
    return t != NULL && t->type == ttype;
}

SyntaxNode *proccess_assignment(Parser *p) {
    peek(p);
    assert(p->current->value != NULL);
    assert(p->current->type == word);
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
    assert(p->current->type == word);
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
    assert(isredirect(current->type));
    advance(p);
    Token *next = peek(p);
    if (next == NULL || next->type != word) {
        printerror("expected a file name after redirect.");
        return NULL;
    }
    assert(next->type == word);
    SyntaxNode *node = alloc_node(redirects);
    node->ttype = current->type;
    node->value = initbuf(next->value->data);
    advance(p);
    return node;
}

//* suffix      → argument | redirect
SyntaxNode *proccess_suffix(Parser *p) {
    Token *current = peek(p);
    bool redir = isredirect(current->type);
    assert(redir || current->type == word);
    SyntaxNode *node;
    if (redir) {
        node = proccess_redirect(p);
    } else {
        node = proccess_arg(p);
    }
    return node;
}

//* prefix      → assignment | redirect
SyntaxNode *proccess_prefix(Parser *p) {
    Token *current = peek(p);
    bool isredir = isredirect(current->type);
    assert(isredir || isassign(current));
    SyntaxNode *node;
    if (isredir) {
        node = proccess_redirect(p);
    } else {
        node = proccess_assignment(p);
    }
    return node;
}

//* command     → prefix* word suffix* | prefix+
SyntaxNode *proccess_command(Parser *p) {
    SyntaxNode *comm = alloc_node(command);
    SyntaxNode *prefix;
    SyntaxNode *prefixtail = NULL;
    while (isprefix(peek(p))) {
        prefix = proccess_prefix(p);
        if (prefix == NULL) {
            free_node(comm);
            return NULL;
        }
        if (prefixtail == NULL) {
            comm->prefixes = prefix;
        } else {
            prefixtail->next = prefix;
        }
        prefixtail = prefix;
    }
    Token *current = peek(p);
    if (current == NULL || current->type != word) {
        if (comm->prefixes == NULL) {
            printerror("expected a command.");
            free_node(comm);
            return NULL;
        }
        return comm;
    }
    comm->value = initbuf(current->value->data);
    advance(p);
    SyntaxNode *suffix;
    SyntaxNode *suffixtail = NULL;
    while (issuffix(p->current)) {
        suffix = proccess_suffix(p);
        if (suffix == NULL) {
            free_node(comm);
            return NULL;
        }
        if (suffixtail == NULL) {
            comm->suffixes = suffix;
        } else {
            suffixtail->next = suffix;
        }
        suffixtail = suffix;
    }
    return comm;
}

//* pipeline    → command (PIPE command)*
SyntaxNode *proccess_pipline(Parser *p) {
    SyntaxNode *comm = proccess_command(p);
    if (comm == NULL) {
        return NULL;
    }
    SyntaxNode *pipe_node = alloc_node(pipeline);
    pipe_node->commands = comm;
    SyntaxNode *commtail = comm;
    while (peek(p) != NULL && p->current->type == pipe) {
        advance(p);
        comm = proccess_command(p);
        if (comm == NULL) {
            free_node(pipe_node);
            return NULL;
        }
        commtail->next = comm;
        commtail = comm;
    }
    return pipe_node;
}

//* statement   → pipeline (ANDD pipeline)*
SyntaxNode *proccess_statement(Parser *p) {
    SyntaxNode *pipe_node = proccess_pipline(p);
    if (pipe_node == NULL) {
        return NULL;
    }
    SyntaxNode *state_node = alloc_node(statement);
    state_node->commands = pipe_node;
    SyntaxNode *commtail = pipe_node;
    while (peek(p) != NULL && p->current->type == andd) {
        advance(p);
        pipe_node = proccess_pipline(p);
        if (pipe_node == NULL) {
            free_node(state_node);
            return NULL;
        }
        commtail->next = pipe_node;
        commtail = pipe_node;
    }
    return state_node;
}
