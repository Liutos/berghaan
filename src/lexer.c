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

static bool
is_end(char c)
{
    return c == '\0';
}

static bool
is_integer(char *s)
{
    for (int i = 0; s[i] != '\0'; i++) {
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}

static TOKEN_T
lexer_next_id(lexer_t *lexer)
{
    char c = lexer->code[lexer->index];
    while (!is_end(c) && !isspace(c) && !is_delimiter(c)) {
        string_push_back(lexer->token, c);
        lexer->index += 1;
        c = lexer->code[lexer->index];
    }
    if (is_integer(lexer->token->data))
        return TOKEN_INT;
    return TOKEN_ID;
}

lexer_t *
lexer_new(const char *string)
{
    lexer_t *lexer = calloc(1, sizeof(lexer_t));
    lexer->code = strdup(string);
    lexer->index = 0;
    lexer->token = string_new();
    return lexer;
}

TOKEN_T
lexer_next(lexer_t *lexer)
{
    char c = lexer->code[lexer->index];
    if (is_end(c))
        return TOKEN_END;
    string_clear(lexer->token);
    string_push_back(lexer->token, c);
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
