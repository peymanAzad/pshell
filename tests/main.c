void test_whitespaces();
void test_pipes();
void test_ands();
void test_newlines();
void test_backslashes();
void test_lts();
void test_gts();
void test_dgts();
void test_qoutes();
void test_dqoutes();
void test_words();
void test_mixed();

void test_buff_alloc();
void test_buffer_push();
void test_buffer_pushc();
void test_buffer_strtok();
void test_buffer_strtokspace();
void test_buffer_slice();

void test_parser_init();
void test_parser_assign();
void test_parser_arg();
void test_parser_redirect();
void test_parser_suffix();
void test_parser_prefix();
void test_parser_command();
void test_parser_pipe();

int main() {
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
}
