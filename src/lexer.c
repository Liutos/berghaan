#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool
is_delimiter(char c)
{
    return c == '(' || c == ')';
}

static TOKEN_T
lexer_next_id(lexer_t *lexer)
{
    char c;
    while (lexer->string[lexer->index] != '\0'
           && !isspace(c = lexer->string[lexer->index])
           && !is_delimiter(c))
        lexer->index += 1;
    return TOKEN_ID;
}

lexer_t *
lexer_new(const char *string)
{
    lexer_t *lexer = calloc(1, sizeof(lexer_t));
    lexer->string = strdup(string);
    lexer->index = 0;
    return lexer;
}

TOKEN_T
lexer_next(lexer_t *lexer)
{
    if (lexer->string[lexer->index] == '\0')
        return TOKEN_END;
    char c = lexer->string[lexer->index];
    lexer->index += 1;
    switch (c) {
        case '\t':
        case '\n':
        case ' ':
            return lexer_next(lexer);
        case '(': return TOKEN_LP;
        case ')': return TOKEN_RP;
        default:
            return lexer_next_id(lexer);
    }
}

TOKEN_T
lexer_peek(lexer_t *lexer)
{
    int index = lexer->index;
    TOKEN_T token = lexer_next(lexer);
    lexer->index = index;
    return token;
}
