#include "pshell.h"

void test_whitespaces(void);
void test_pipes(void);
void test_ands(void);
void test_newlines(void);
void test_backslashes(void);
void test_lts(void);
void test_gts(void);
void test_dgts(void);
void test_qoutes(void);
void test_dqoutes(void);
void test_words(void);
void test_mixed(void);

void test_buff_alloc(void);
void test_buffer_push(void);
void test_buffer_pushc(void);
void test_buffer_strtok(void);
void test_buffer_strtokspace(void);
void test_buffer_slice(void);

void test_parser_init(void);
void test_parser_assign(void);
void test_parser_arg(void);
void test_parser_redirect(void);
void test_parser_suffix(void);
void test_parser_prefix(void);
void test_parser_command(void);
void test_parser_pipe(void);
void test_parser_statement(void);
void test_parser_script(void);

void test_exec_command(void);
void test_exec_pipe(void);

void test_expand_simple(void);
void test_expand(void);
void test_expand_undef(void);
void test_expand_empty(void);

int main(void) {
    test_whitespaces();
    test_pipes();
    test_ands();
    test_newlines();
    test_backslashes();
    test_lts();
    test_gts();
    test_dgts();
    test_qoutes();
    test_dqoutes();
    test_words();
    test_mixed();

    test_buff_alloc();
    test_buffer_push();
    test_buffer_pushc();
    test_buffer_strtok();
    test_buffer_strtokspace();
    test_buffer_slice();

    test_parser_init();
    test_parser_assign();
    test_parser_arg();
    test_parser_redirect();
    test_parser_suffix();
    test_parser_prefix();
    test_parser_command();
    test_parser_pipe();
    test_parser_statement();
    test_parser_script();

    test_exec_command();
    test_exec_pipe();

    test_expand_simple();
    test_expand();
    test_expand_undef();
    test_expand_empty();
}
