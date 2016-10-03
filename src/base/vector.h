/**
 * 定义向量类型及其操作接口
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t capacity;
    size_t length;
    void **data;
} vector_t;

extern bool vector_is_empty(vector_t *);
extern vector_t *vector_new(void);
extern vector_t *vector_concat(vector_t *, vector_t *);
extern void *vector_at(vector_t *, size_t);
extern void *vector_back(vector_t *);
extern void *vector_set(vector_t *, size_t, void *);
extern void *vector_unsafe_at(vector_t *, size_t);
extern void vector_pop_back(vector_t *);
extern void vector_push_back(vector_t *, void *);
extern void vector_reserve(vector_t *, size_t);
