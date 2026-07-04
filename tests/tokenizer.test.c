#include "../pshell.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define module "Tokenizer"

void test_whitespaces() {
    static char *name = "Whitespaces";
    Token tokens[3];
    int c = tokenize(tokens, "  \t    ", 3);
    assert(c == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_pipes() {
    static char *name = "Pipes";
    Token tokens[3];
    int c = tokenize(tokens, "|", 3);
    assert(c == 1);
    assert(tokens[0].type == pipe);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "|||", 3);
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == pipe && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_ands() {
    static char *name = "&&";
    Token tokens[3];
    int c = tokenize(tokens, "&&", 3);
    assert(c == 1);
    assert(tokens[0].type == andd);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "&&&&&", 3);
    assert(c == 2);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == andd && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_newlines() {
    static char *name = "Newline";
    Token tokens[3];
    int c = tokenize(tokens, "\n", 3);
    assert(c == 1);
    assert(tokens[0].type == newline);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "\n \n\n", 3);
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == newline && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_backslashes() {
    static char *name = "Backslash";
    Token tokens[3];
    int c = tokenize(tokens, "\\", 3);
    assert(c == 1);
    assert(tokens[0].type == backslash);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "\\\\ \\", 3);
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == backslash && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_lts() {
    static char *name = "<";
    Token tokens[3];
    int c = tokenize(tokens, "<", 3);
    assert(c == 1);
    assert(tokens[0].type == lt);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "< < <", 3);
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == lt && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_gts() {
    static char *name = ">";
    Token tokens[3];
    int c = tokenize(tokens, ">", 3);
    assert(c == 1);
    assert(tokens[0].type == gt);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "> > >", 3);
    assert(c == 3);
    for (int i = 0; i < 3; ++i)
        assert(tokens[i].type == gt && tokens[i].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_dgts() {
    static char *name = ">>";
    Token tokens[3];
    int c = tokenize(tokens, ">>", 3);
    assert(c == 1);
    assert(tokens[0].type == dgt);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, ">>>>>", 3);
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == dgt && tokens[i].value == NULL);
    assert(tokens[2].type == gt);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_qoutes() {
    static char *name = "Qoute";
    Token tokens[3];
    int c = tokenize(tokens, "'", 3);
    assert(c == 1);
    assert(tokens[0].type == qoute);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "'test1' 'test2' ' '", 3);
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == qoute);
    assert(strcmp(tokens[0].value, "test1") == 0);
    assert(strcmp(tokens[1].value, "test2") == 0);
    assert(strcmp(tokens[2].value, " ") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_dqoutes() {
    static char *name = "Double Qoute";
    Token tokens[3];
    int c = tokenize(tokens, "\"", 3);
    assert(c == 1);
    assert(tokens[0].type == dqoute);
    assert(tokens[0].value == NULL);
    c = tokenize(tokens, "\"test1\" \"test2\" \" \"", 3);
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == dqoute);
    assert(strcmp(tokens[0].value, "test1") == 0);
    assert(strcmp(tokens[1].value, "test2") == 0);
    assert(strcmp(tokens[2].value, " ") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_words() {
    static char *name = "Words";
    Token tokens[3];
    int c = tokenize(tokens, "test", 3);
    assert(c == 1);
    assert(tokens[0].type == word);
    assert(strcmp(tokens[0].value, "test") == 0);
    c = tokenize(tokens, "test1='x y 2' --test2 -x=\"test  3\"", 3);
    assert(c == 3);
    for (int i = 0; i < 2; ++i)
        assert(tokens[i].type == word);
    assert(strcmp(tokens[0].value, "test1='x y 2'") == 0);
    assert(strcmp(tokens[1].value, "--test2") == 0);
    assert(strcmp(tokens[2].value, "-x=\"test  3\"") == 0);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}

void test_mixed() {
    static char *name = "Mixed";
    Token tokens[12];
    int c = tokenize(tokens,
                     "test -t=\"{arg1}\" \\ ls . && cp >> test.txt < d |", 12);
    assert(c == 12);
    assert(tokens[0].type == word && strcmp(tokens[0].value, "test") == 0);
    assert(tokens[1].type == word &&
           strcmp(tokens[1].value, "-t=\"{arg1}\"") == 0);
    assert(tokens[2].type == backslash && tokens[2].value == NULL);
    assert(tokens[3].type == word && strcmp(tokens[3].value, "ls") == 0);
    assert(tokens[4].type == word && strcmp(tokens[4].value, ".") == 0);
    assert(tokens[5].type == andd && tokens[5].value == NULL);
    assert(tokens[6].type == word && strcmp(tokens[6].value, "cp") == 0);
    assert(tokens[7].type == dgt && tokens[7].value == NULL);
    assert(tokens[8].type == word && strcmp(tokens[8].value, "test.txt") == 0);
    assert(tokens[9].type == lt && tokens[9].value == NULL);
    assert(tokens[10].type == word && strcmp(tokens[10].value, "d") == 0);
    assert(tokens[11].type == pipe && tokens[11].value == NULL);
    printf("%10s/%-15s: test passed successfully.\n", module, name);
}
