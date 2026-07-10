
#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define module "Parser"
#define assertstr(x, y) assert(strcmp((x), (y)) == 0)

void test_parser_init() {
    static char *name = "InitParser";
    SyntaxNode *node = alloc_node(argument);
    Parser *p = alloc_parser("test");
    node->value = initbuf("value test");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_assign() {
    static char *name = "ProcAssign";
    Parser *p = alloc_parser("foo=bar");
    SyntaxNode *node = proccess_assignment(p);
    assert(node->type == assignment);
    assertstr(node->name->data, "foo");
    assertstr(node->value->data, "bar");
    free_node(node);
    free_parser(p);
    p = alloc_parser("bar='baz  foo=zoo '");
    node = proccess_assignment(p);
    assert(node->type == assignment);
    assertstr(node->name->data, "bar");
    assertstr(node->value->data, "baz  foo=zoo ");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_arg() {
    static char *name = "ProcArg";
    Parser *p = alloc_parser("-x1");
    SyntaxNode *node = proccess_arg(p);
    assert(node->type == argument);
    assertstr(node->value->data, "-x1");
    free_node(node);
    free_parser(p);
    p = alloc_parser("--foo='bar baz'");
    node = proccess_arg(p);
    assert(node->type == argument);
    assertstr(node->value->data, "--foo=bar baz");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_redirect() {
    static char *name = "ProcRedirect";
    Parser *p = alloc_parser("> text.txt");
    SyntaxNode *node = proccess_redirect(p);
    assert(node->type == redirects);
    assertstr(node->value->data, "text.txt");
    free_node(node);
    free_parser(p);
    p = alloc_parser("< 'test 1.txt'");
    node = proccess_redirect(p);
    assert(node->type == redirects);
    assertstr(node->value->data, "test 1.txt");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_suffix() {
    static char *name = "ProcSuffix";
    Parser *p = alloc_parser(">> text.txt");
    SyntaxNode *node = proccess_suffix(p);
    assert(node->type == redirects);
    assertstr(node->value->data, "text.txt");
    free_node(node);
    free_parser(p);
    p = alloc_parser("--arg");
    node = proccess_suffix(p);
    assert(node->type == argument);
    assertstr(node->value->data, "--arg");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_prefix() {
    static char *name = "ProcPrefix";
    Parser *p = alloc_parser("foo=bar");
    SyntaxNode *node = proccess_prefix(p);
    assert(node->type == assignment);
    assertstr(node->name->data, "foo");
    assertstr(node->value->data, "bar");
    free_node(node);
    free_parser(p);
    p = alloc_parser("> test.txt");
    node = proccess_suffix(p);
    assert(node->type == redirects);
    assertstr(node->value->data, "test.txt");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
