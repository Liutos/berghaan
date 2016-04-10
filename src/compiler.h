/**
 * 提供编译器定义及操作接口
 */
#pragma once

#include "ast.h"
#include "base/vector.h"

typedef struct {
    vector_t *code;
} compiler_t;

extern compiler_t *compiler_new(void);
extern void compiler_compile(compiler_t *, ast_t *);
extern void compiler_init(void);
