/**
 * 提供语法分析的功能
 */
#pragma once

#include "lexer.h"

typedef struct {
    lexer_t *lexer;
} parser_t;

extern parser_t *parser_new(lexer_t *lexer);
extern void program(parser_t *parser);
