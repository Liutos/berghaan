/**
 * 提供内置函数的定义及初始化操作
 */
#pragma once

#include "env.h"
#include "base/vector.h"

typedef object_t *(*bif_0_t)(void);
typedef object_t *(*bif_1_t)(object_t *);
typedef object_t *(*bif_2_t)(object_t *, object_t *);

extern void bif_init(env_t **, vector_t **);
