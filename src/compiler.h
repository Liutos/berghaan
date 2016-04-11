/**
 * 提供编译器定义及操作接口
 */
#pragma once

#include "ast.h"
#include "base/vector.h"

typedef vector_t code_t;

extern void compiler_compile(ast_t *);
extern void compiler_init(void);
extern code_t *compiler_done(void);
