/**
 * 提供对抽象语法树进行解释执行的功能
 */
#pragma once

#include "ast.h"
#include "object.h"

extern object_t *interpret(ast_t *);
