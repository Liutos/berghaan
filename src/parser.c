#include "ast.h"
#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ast_t *expr_list(parser_t *);
static ast_t *expr(parser_t *);
static ast_t *list(parser_t *);

static bool
is_atom(TOKEN_T token)
{
    return token == TOKEN_BOOL || token == TOKEN_ID || token == TOKEN_INT;
}

static char *
parser_match(parser_t *parser, TOKEN_T type)
{
    lexer_t *lexer = parser->lexer;
    TOKEN_T token = lexer_next(lexer);
    if (token == type) {
        printf("MATCH '%s'\n", lexer->token->data);
        return lexer->token->data;
    } else {
        printf("DISMATCH '%s'\n", lexer->token->data);
        exit(EXIT_FAILURE);
    }
}

static ast_t *
expr_list(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (is_atom(token)
        || token == TOKEN_LP) {
        ast_t *e = expr(parser);
        ast_t *el = expr_list(parser);
        return ast_cons_new(e, el);
    } else if (token == TOKEN_RP
               || token == TOKEN_END)
        return NULL;
    else
        exit(EXIT_FAILURE);
}

static ast_t *
expr(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (token == TOKEN_BOOL) {
        char *text = parser_match(parser, TOKEN_BOOL);
        return ast_bool_new(strcmp(text, "true") == 0);
    } if (token == TOKEN_ID) {
        char *name = parser_match(parser, TOKEN_ID);
        return ast_id_new(name);
    } else if (token == TOKEN_INT) {
        char *n = parser_match(parser, TOKEN_INT);
        int value = atoi(n);
        return ast_int_new(value);
    } else if (token == TOKEN_LP)
        return list(parser);
    else
        exit(EXIT_FAILURE);
}

static ast_t *
list(parser_t *parser)
{
    parser_match(parser, TOKEN_LP);
    ast_t *e = expr(parser);
    ast_t *el = expr_list(parser);
    parser_match(parser, TOKEN_RP);
    return ast_cons_new(e, el);
}

parser_t *
parser_new(lexer_t *lexer)
{
    parser_t *parser = calloc(1, sizeof(parser_t));
    parser->lexer = lexer;
    return parser;
}

ast_t *
program(parser_t *parser)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (is_atom(token)
        || token == TOKEN_LP
        || token == TOKEN_END)
        return ast_prog_new(expr_list(parser));
    else
        exit(EXIT_FAILURE);
}
