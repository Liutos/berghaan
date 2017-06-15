#include "ast.h"
#include "base/log.h"
#include "base/utf8.h"
#include "lexer.h"
#include "parser.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

static PARSE_RESULT expr_list(parser_t *, ast_t **);
static PARSE_RESULT expr(parser_t *, ast_t **);
static PARSE_RESULT list(parser_t *, ast_t **);
static PARSE_RESULT sharp_v(parser_t *, ast_t **);

static bool
is_atom(TOKEN_T token)
{
    return token == TOKEN_BOOL || token == TOKEN_CHAR || token == TOKEN_ID || token == TOKEN_INT || token == TOKEN_STRING || token == TOKEN_SYMBOL;
}

static char *
parser_match(parser_t *parser, TOKEN_T type)
{
    lexer_t *lexer = parser->lexer;
    TOKEN_T token = lexer_next(lexer);
    if (token == type) {
        log_lprintf(LOG_DEBUG, "MATCH '%s'\n", lexer->token->data);
        return lexer->token->data;
    } else {
        log_lprintf(LOG_DEBUG, "DISMATCH '%s'\n", lexer->token->data);
        exit(EXIT_FAILURE);
    }
}

static PARSE_RESULT
parser_string(parser_t *parser, ast_t **result)
{
    char *content = parser_match(parser, TOKEN_STRING);
    *result = ast_string_new(content);
    return SUCCEED;
}

static PARSE_RESULT
expr_list(parser_t *parser, ast_t **exprs)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (is_atom(token)
        || token == TOKEN_LP
        || token == TOKEN_SHARP_V) {
        ast_t *e, *el;
        if (expr(parser, &e) == FAIL)
            return FAIL;
        if (expr_list(parser, &el) == FAIL)
            return FAIL;
        *exprs = ast_cons_new(e, el);
        return SUCCEED;
    } else if (token == TOKEN_RP
               || token == TOKEN_END) {
        *exprs = NULL;
        return SUCCEED;
    } else
        return FAIL;
}

static PARSE_RESULT
expr(parser_t *parser, ast_t **result)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (token == TOKEN_BOOL) {
        char *text = parser_match(parser, TOKEN_BOOL);
        *result = ast_bool_new(strcmp(text, "true") == 0);
        return SUCCEED;
    } if (token == TOKEN_CHAR) {
        char *text = parser_match(parser, TOKEN_CHAR);
        uint32_t value = utf8_sread(&text[1], NULL);
        *result = ast_char_new(value);
        return SUCCEED;
    } if (token == TOKEN_ID) {
        char *name = parser_match(parser, TOKEN_ID);
        *result = ast_id_new(name);
        return SUCCEED;
    } else if (token == TOKEN_INT) {
        char *n = parser_match(parser, TOKEN_INT);
        int value = atoi(n);
        *result = ast_int_new(value);
        return SUCCEED;
    } else if (token == TOKEN_LP)
        return list(parser, result);
    else if (token == TOKEN_SHARP_V)
        return sharp_v(parser, result);
    else if (token == TOKEN_STRING)
        return parser_string(parser, result);
    else if (token == TOKEN_SYMBOL) {
        char *n = parser_match(parser, TOKEN_SYMBOL);
        *result = ast_symbol_new(n + 1);
        return SUCCEED;
    }
    return FAIL;
}

static PARSE_RESULT
list(parser_t *parser, ast_t **result)
{
    parser_match(parser, TOKEN_LP);
    ast_t *e, *el;
    if (expr(parser, &e) == FAIL)
        return FAIL;
    if (expr_list(parser, &el) == FAIL)
        return FAIL;
    parser_match(parser, TOKEN_RP);
    *result = ast_cons_new(e, el);
    return SUCCEED;
}

parser_t *
parser_new(lexer_t *lexer)
{
    parser_t *parser = calloc(1, sizeof(parser_t));
    parser->lexer = lexer;
    return parser;
}

PARSE_RESULT
program(parser_t *parser, ast_t **result)
{
    TOKEN_T token = lexer_peek(parser->lexer);
    if (is_atom(token)
        || token == TOKEN_LP
        || token == TOKEN_END
        || token == TOKEN_SHARP_V) {
        ast_t *exprs;
        PARSE_RESULT status = expr_list(parser, &exprs);
        if (status == FAIL)
            return status;
        *result = ast_prog_new(exprs);
        return SUCCEED;
    } else
        return FAIL;
}

PARSE_RESULT
sharp_v(parser_t *parser, ast_t **result)
{
    parser_match(parser, TOKEN_SHARP_V);
    ast_t *elements;
    if (list(parser, &elements) == FAIL)
        return FAIL;
    *result = ast_sharp_v_new(elements);
    return SUCCEED;
}
