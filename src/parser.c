#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void expr_list(parser_t *);
static void expr(parser_t *);
static void list(parser_t *);

static void
parser_match(parser_t *parser, TOKEN_T type)
{
    lexer_t *lexer = parser->lexer;
    TOKEN_T token = lexer_next(lexer);
    if (token == type)
        printf("MATCH '%s'\n", lexer->token->data);
    else {
        printf("DISMATCH '%s'\n", lexer->token->data);
        exit(EXIT_FAILURE);
    }
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
        parser_match(parser, TOKEN_ID);
    else if (token == TOKEN_LP)
        list(parser);
    else
        exit(EXIT_FAILURE);
}

static void
list(parser_t *parser)
{
    parser_match(parser, TOKEN_LP);
    expr(parser);
    expr_list(parser);
    parser_match(parser, TOKEN_RP);
}

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
