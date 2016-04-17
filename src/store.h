/**
 * 提供存储空间定义及相关接口
 */
#pragma once

#include "base/vector.h"
#include "object.h"

typedef struct _store_t {
    vector_t *pool;
    struct _store_t *outer;
} store_t;

extern store_t *store_new(int, store_t *);
extern void store_set(store_t *, size_t, size_t, object_t *);
extern object_t *store_get(store_t *, int, int);
