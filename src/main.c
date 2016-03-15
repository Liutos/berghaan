#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 词法解析 BEGIN */

typedef enum {
    TOKEN_END,
    TOKEN_ID,
    TOKEN_LP,
    TOKEN_RP,
} TOKEN_T;

typedef struct {
    char *string;
    int index;
} lexer_t;

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

/* 词法解析 END */

/* 语法解析 BEGIN */

typedef struct {
    lexer_t *lexer;
} parser_t;

static void expr_list(parser_t *);
static void expr(parser_t *);
static void list(parser_t *);

parser_t *
parser_new(lexer_t *lexer)
{
    parser_t *parser = calloc(1, sizeof(parser_t));
    parser->lexer = lexer;
    return parser;
}

void
program(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (token == TOKEN_ID
        || token == TOKEN_LP
        || token == TOKEN_END)
        expr_list(parser);
    else
        exit(EXIT_FAILURE);
}

static void
expr_list(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (token == TOKEN_ID
        || token == TOKEN_LP) {
        expr(parser);
        expr_list(parser);
    } else if (token == TOKEN_RP
               || token == TOKEN_END)
        return;
    else
        exit(EXIT_FAILURE);
}

static void
expr(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (token == TOKEN_ID)
        lexer_next(parser->lexer);
    else if (token == TOKEN_LP)
        list(parser);
    else
        exit(EXIT_FAILURE);
}

static void
list(parser_t *parser)
{
    TOKEN_T token = lexer_next(parser->lexer);
    expr(parser);
    expr_list(parser);
    token = lexer_next(parser->lexer);
    if (token != TOKEN_RP)
        exit(EXIT_FAILURE);
}

/* 语法解析 END */

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    return 0;
}
