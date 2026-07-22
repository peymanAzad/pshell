#include "pshell.h"
#include <assert.h>
#include <stdio.h>
#define module "Parser"

void test_parser_init(void) {
    static char *name = "InitParser";
    SyntaxNode *node = alloc_node(argument);
    Parser *p = alloc_parser("test");
    node->value = initbuf("value test");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_assign(void) {
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

void test_parser_arg(void) {
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

void test_parser_redirect(void) {
    static char *name = "ProcRedirect";
    Parser *p = alloc_parser("> text.txt");
    SyntaxNode *node = proccess_redirect(p);
    assert(node->type == redirects);
    assert(node->ttype == gt);
    assertstr(node->value->data, "text.txt");
    free_node(node);
    free_parser(p);
    p = alloc_parser("< 'test 1.txt'");
    node = proccess_redirect(p);
    assert(node->type == redirects);
    assert(node->ttype == lt);
    assertstr(node->value->data, "test 1.txt");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_suffix(void) {
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

void test_parser_prefix(void) {
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

void test_parser_command(void) {
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

void test_parser_pipe(void) {
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

void test_parser_statement(void) {
    static char *name = "ProcStatement";
    Parser *p = alloc_parser("mkdir test && cd ./test");
    SyntaxNode *node = proccess_statement(p);
    assert(node->type == statement);
    assert(node->commands->type == pipeline);
    assertstr(node->commands->commands->value->data, "mkdir");
    assertstr(node->commands->commands->suffixes->value->data, "test");
    assertstr(node->commands->next->commands->value->data, "cd");
    assertstr(node->commands->next->commands->suffixes->value->data, "./test");
    p = alloc_parser("t1 && t2 && t3");
    node = proccess_statement(p);
    assert(node->type == statement);
    assertstr(node->commands->commands->value->data, "t1");
    assertstr(node->commands->next->commands->value->data, "t2");
    assertstr(node->commands->next->next->commands->value->data, "t3");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}

void test_parser_script(void) {
    static char *name = "ProcScript";
    Parser *p = alloc_parser("mkdir test ; cd ./test");
    SyntaxNode *node = proccess_script(p);
    assert(node->type == script);
    assert(node->commands->type == statement);
    assertstr(node->commands->commands->commands->value->data, "mkdir");
    assertstr(node->commands->commands->commands->suffixes->value->data,
              "test");
    assertstr(node->commands->next->commands->commands->value->data, "cd");
    assertstr(node->commands->next->commands->commands->suffixes->value->data,
              "./test");
    p = alloc_parser("t1 \n t2 ; t3 \n");
    node = proccess_script(p);
    assert(node->type == script);
    assert(node->commands->type == statement);
    assertstr(node->commands->commands->commands->value->data, "t1");
    assertstr(node->commands->next->commands->commands->value->data, "t2");
    assertstr(node->commands->next->next->commands->commands->value->data,
              "t3");
    free_node(node);
    free_parser(p);
    printf("%-10s/%-15s: test passed successfully.\n", module, name);
}
