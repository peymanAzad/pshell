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
}
