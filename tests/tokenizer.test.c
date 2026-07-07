#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define module "Tokenizer"

void test_whitespaces() {
    static char *name = "Whitespaces";
    Token tokens[3];
    int c = next_token(tokens, "  \t    ", 0);
    assert(c == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_pipes() {
    static char *name = "Pipes";
    Token tokens[3];
    int c = tokenize_all(tokens, "|");
    assert(c == 1);
    assert(tokens[0].type == pipe);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, "|||");
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == pipe && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_ands() {
    static char *name = "&&";
    Token tokens[3];
    int c = tokenize_all(tokens, "&&");
    assert(c == 1);
    assert(tokens[0].type == andd);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, "&&&&&");
    assert(c == 2);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == andd && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_newlines() {
    static char *name = "Newline";
    Token tokens[6];
    int c = tokenize_all(tokens, "\n");
    assert(c == 1);
    c = tokenize_all(tokens, ";");
    assert(c == 1);
    assert(tokens[0].type == newline);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, "\n; ;; \n\n");
    assert(c == 6);
    for (int i = 0; i < 6; ++i)
        assert(tokens[i].type == newline && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_backslashes() {
    static char *name = "Backslash";
    Token tokens[3];
    int c = tokenize_all(tokens, "\\");
    assert(c == 0);
    c = tokenize_all(tokens, "\\\n \\\n\\\n\\ \\\n\\");
    assert(c == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_lts() {
    static char *name = "<";
    Token tokens[3];
    int c = tokenize_all(tokens, "<");
    assert(c == 1);
    assert(tokens[0].type == lt);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, "< < <");
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == lt && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_gts() {
    static char *name = ">";
    Token tokens[3];
    int c = tokenize_all(tokens, ">");
    assert(c == 1);
    assert(tokens[0].type == gt);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, "> > >");
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == gt && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_dgts() {
    static char *name = ">>";
    Token tokens[3];
    int c = tokenize_all(tokens, ">>");
    assert(c == 1);
    assert(tokens[0].type == dgt);
    assert(tokens[0].value == NULL);
    c = tokenize_all(tokens, ">>>>>");
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == dgt && tokens[i].value == NULL);
    assert(tokens[2].type == gt);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_qoutes() {
    static char *name = "Qoute";
    Token tokens[3];
    int c = tokenize_all(tokens, "'");
    assert(c == 1);
    assert(tokens[0].type == word);
    assert(strcmp(tokens[0].value->data, "") == 0);
    c = tokenize_all(tokens, "'test1' 'test2' ' '");
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == word);
    assert(strcmp(tokens[0].value->data, "test1") == 0);
    assert(strcmp(tokens[1].value->data, "test2") == 0);
    assert(strcmp(tokens[2].value->data, " ") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_dqoutes() {
    static char *name = "DoubleQoute";
    Token tokens[3];
    int c = tokenize_all(tokens, "\"");
    assert(c == 1);
    assert(tokens[0].type == word);
    assert(strcmp(tokens[0].value->data, "") == 0);
    c = tokenize_all(tokens, "abc\"test1\"def \"test2\" \" \"");
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == word);
    assert(strcmp(tokens[0].value->data, "abctest1def") == 0);
    assert(strcmp(tokens[1].value->data, "test2") == 0);
    assert(strcmp(tokens[2].value->data, " ") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_words() {
    static char *name = "Words";
    Token tokens[3];
    int c = tokenize_all(tokens, "test");
    assert(c == 1);
    assert(tokens[0].type == word);
    assert(strcmp(tokens[0].value->data, "test") == 0);
    c = tokenize_all(tokens, "test1='x y 2' --test2 -x=\"test  3\"");
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == word);
    assert(strcmp(tokens[0].value->data, "test1=x y 2") == 0);
    assert(strcmp(tokens[1].value->data, "--test2") == 0);
    assert(strcmp(tokens[2].value->data, "-x=test  3") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_mixed() {
    static char *name = "Mixed";
    Token tokens[11];
    int c = tokenize_all(
        tokens, "test -t=\"{arg1}\" \\\n ls . && cp >> test.txt < d |");
    assert(c == 11);
    assert(tokens[0].type == word &&
           strcmp(tokens[0].value->data, "test") == 0);
    assert(tokens[1].type == word &&
           strcmp(tokens[1].value->data, "-t={arg1}") == 0);
    assert(tokens[2].type == word && strcmp(tokens[2].value->data, "ls") == 0);
    assert(tokens[3].type == word && strcmp(tokens[3].value->data, ".") == 0);
    assert(tokens[4].type == andd && tokens[4].value == NULL);
    assert(tokens[5].type == word && strcmp(tokens[5].value->data, "cp") == 0);
    assert(tokens[6].type == dgt && tokens[6].value == NULL);
    assert(tokens[7].type == word &&
           strcmp(tokens[7].value->data, "test.txt") == 0);
    assert(tokens[8].type == lt && tokens[8].value == NULL);
    assert(tokens[9].type == word && strcmp(tokens[9].value->data, "d") == 0);
    assert(tokens[10].type == pipe && tokens[10].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}
