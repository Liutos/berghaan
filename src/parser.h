/**
 * 提供语法分析的功能
 */
#pragma once

#include "ast.h"
#include "lexer.h"

typedef enum {
    FAIL,
    SUCCEED,
} PARSE_RESULT;

typedef struct {
    lexer_t *lexer;
} parser_t;

extern parser_t *parser_new(lexer_t *lexer);
extern PARSE_RESULT program(parser_t *parser, ast_t **);
