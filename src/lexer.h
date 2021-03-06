/**
 * 提供词法分析的功能
 */
#pragma once

#include "base/string.h"

typedef enum {
    TOKEN_BOOL,
    TOKEN_CHAR,
    TOKEN_END,
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_LP,
    TOKEN_RP,
    TOKEN_SHARP_V,
    TOKEN_STRING,
    TOKEN_SYMBOL,
} TOKEN_T;

typedef struct {
    char *code;
    int index;
    string_t *token;
} lexer_t;

extern lexer_t *lexer_new(const char *string);
extern TOKEN_T lexer_next(lexer_t *lexer);
extern TOKEN_T lexer_peek(lexer_t *lexer);
