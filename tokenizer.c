#include "pshell.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int get_to_char(char *tokenval, char *input, char ch, int max_len) {
    int wcount;
    char c;
    for (wcount = 0; (c = *input++) != '\0' && c != EOF && c != ch; ++wcount) {
        if (wcount < max_len)
            tokenval[wcount] = c;
    }
    tokenval[wcount] = '\0';
    if (c == ch)
        ++wcount;
    return wcount;
}

int tokenize(Token *result, char *input, size_t count) {
    char c;
    size_t cursor = 0;
    size_t tcount;
    int wcount;
    char *tokenval;
    for (tcount = 0; (c = *input++) != '\0' && c != EOF && tcount < count;) {
        switch (c) {
        case '\n':
            result[cursor++] = (Token){.type = newline, .value = NULL};
            ++tcount;
            break;

        case '|':
            result[cursor++] = (Token){.type = pipe, .value = NULL};
            ++tcount;
            break;

        case '<':
            result[cursor++] = (Token){.type = lt, .value = NULL};
            ++tcount;
            break;

        case '>':
            if (*(input) == '>') {
                result[cursor++] = (Token){.type = dgt, .value = NULL};
                ++input;
            } else
                result[cursor++] = (Token){.type = gt, .value = NULL};
            ++tcount;
            break;

        case '&':
            if (*(input) == '&') {
                result[cursor++] = (Token){.type = andd, .value = NULL};
                ++tcount;
                ++input;
            }
            break;

        case '\\':
            result[cursor++] = (Token){.type = backslash, .value = NULL};
            ++tcount;
            break;

        case '\'':
            tokenval = (char *)malloc(MAX_WORD_LEN + 1);
            wcount = get_to_char(tokenval, input, '\'', MAX_WORD_LEN);
            if (wcount == 0) {
                free(tokenval);
                tokenval = NULL;
            }
            input += wcount;
            result[cursor++] = (Token){.type = qoute, .value = tokenval};
            ++tcount;
            break;

        case '"':
            tokenval = (char *)malloc(MAX_WORD_LEN + 1);
            wcount = get_to_char(tokenval, input, '"', MAX_WORD_LEN);
            if (wcount == 0) {
                free(tokenval);
                tokenval = NULL;
            }
            input += wcount;
            result[cursor++] = (Token){.type = dqoute, .value = tokenval};
            ++tcount;
            break;

        default:
            if (isspace(c))
                continue;

            tokenval = (char *)malloc(MAX_WORD_LEN + 1);
            wcount = 0;
            do {
                if (wcount < MAX_WORD_LEN)
                    tokenval[wcount++] = c;
                if (c == '\'' || c == '"') {
                    int qcount = get_to_char(tokenval + wcount, input, c,
                                             MAX_WORD_LEN - wcount - 1);
                    input += qcount;
                    wcount += qcount;
                    tokenval[wcount - 1] = c;
                }
            } while ((c = *input++) != '\0' && c != EOF && !isspace(c));
            tokenval[wcount] = '\0';
            result[cursor++] = (Token){.type = word, .value = tokenval};
            ++tcount;
            --input;
        }
    }
    return tcount;
}
