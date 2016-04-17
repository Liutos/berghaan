/**
 * 提供环境类型定义及操作接口
 */
#pragma once

#include "base/list.h"
#include "object.h"

#include <stdbool.h>

typedef struct _env_t {
    assoc_list_t *bindings;
    struct _env_t *outer;
} env_t;

extern bool env_position(env_t *, const char *, int *, int *);
extern env_t *env_new(env_t *);
extern env_t *env_bind(env_t *, const char *, object_t *);
extern env_t *env_push_back(env_t *, const char *, object_t *);
extern object_t *env_at(env_t *, int, int);
extern object_t *env_reference(env_t *, const char *);
extern env_t *env_set(env_t *, int, int, object_t *);
