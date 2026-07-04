#include "pshell.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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
            break;

        case '|':
            result[cursor++] = (Token){.type = pipe, .value = NULL};
            break;

        case '<':
            result[cursor++] = (Token){.type = lt, .value = NULL};
            break;

        case '>':
            if (*(input + 1) == '>') {
                result[cursor++] = (Token){.type = dgt, .value = NULL};
                ++input;
            } else
                result[cursor++] = (Token){.type = gt, .value = NULL};
            break;

        case '&':
            if (*(input + 1) == '&') {
                result[cursor++] = (Token){.type = andd, .value = NULL};
                ++input;
            }
            break;

        case '\\':
            result[cursor++] = (Token){.type = backslash, .value = NULL};
            break;

        case '\'':
            tokenval = (char *)malloc(MAX_WORD_LEN + 1);
            for (wcount = 0; (c = *input++) != '\0' && c != EOF && c != '\'';
                 ++wcount) {
                if (wcount < MAX_WORD_LEN)
                    tokenval[wcount] = c;
            }
            tokenval[wcount] = '\0';
            result[cursor++] = (Token){.type = qoute, .value = tokenval};
            break;

        case '"':
            result[cursor++] = (Token){.type = dqoute, .value = NULL};
            break;

        default:
            if (isspace(c))
                continue;

            tokenval = (char *)malloc(MAX_WORD_LEN + 1);
            wcount = 0;
            do {
                if (wcount < MAX_WORD_LEN)
                    tokenval[wcount++] = c;
            } while ((c = *input++) != '\0' && c != EOF && !isspace(c));
            tokenval[wcount] = '\0';
            result[cursor++] = (Token){.type = word, .value = tokenval};
            --input;
        }
        ++tcount;
    }
    return tcount;
}
