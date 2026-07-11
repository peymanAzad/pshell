
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

void test_parser_command() {
    static char *name = "ProcCommand";
    Parser *p = alloc_parser("echo");
    SyntaxNode *node = proccess_command(p);
    assert(node->type == command);
    assertstr(node->value->data, "echo");
    free_node(node);
    free_parser(p);
    p = alloc_parser("foo=bar echo hello");
    node = proccess_command(p);
    assert(node->type == command);
    assertstr(node->value->data, "echo");
    assertstr(node->prefixes->name->data, "foo");
    assertstr(node->prefixes->value->data, "bar");
    assertstr(node->suffixes->value->data, "hello");
    free_node(node);
    free_parser(p);
    p = alloc_parser("foo=bar < input.txt echo hello >> output.txt");
    node = proccess_command(p);
    assert(node->type == command);
    assertstr(node->value->data, "echo");
    assert(node->prefixes->type == assignment);
    assertstr(node->prefixes->name->data, "foo");
    assertstr(node->prefixes->value->data, "bar");
    assert(node->prefixes->next->type == redirects);
    assertstr(node->prefixes->next->value->data, "input.txt");
    assertstr(node->suffixes->value->data, "hello");
    assert(node->suffixes->type == argument);
    assertstr(node->suffixes->next->value->data, "output.txt");
    assert(node->suffixes->next->type == redirects);
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_pipe() {
    static char *name = "ProcPipe";
    Parser *p = alloc_parser("echo hello | cat test.txt");
    SyntaxNode *node = proccess_pipline(p);
    assert(node->type == pipeline);
    assertstr(node->commands->value->data, "echo");
    assertstr(node->commands->suffixes->value->data, "hello");
    assertstr(node->commands->next->value->data, "cat");
    assertstr(node->commands->next->suffixes->value->data, "test.txt");
    p = alloc_parser("t1 | t2 | t3");
    node = proccess_pipline(p);
    assert(node->type == pipeline);
    assertstr(node->commands->value->data, "t1");
    assertstr(node->commands->next->value->data, "t2");
    assertstr(node->commands->next->next->value->data, "t3");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
