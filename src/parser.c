#include "lexer.h"
#include "parser.h"

#include <stdlib.h>

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
