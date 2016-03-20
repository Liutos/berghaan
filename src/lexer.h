/**
 * 提供词法分析的功能
 */
#pragma once

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

extern lexer_t *lexer_new(const char *string);
extern TOKEN_T lexer_next(lexer_t *lexer);
extern TOKEN_T lexer_peek(lexer_t *lexer);
