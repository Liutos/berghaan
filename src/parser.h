/**
 * 提供语法分析的功能
 */
#pragma once

#include "ast.h"
#include "lexer.h"

typedef struct {
    lexer_t *lexer;
} parser_t;

extern parser_t *parser_new(lexer_t *lexer);
extern ast_t *program(parser_t *parser);
