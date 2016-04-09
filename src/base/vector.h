/**
 * 定义向量类型及其操作接口
 */
#pragma once

#include <stddef.h>

typedef struct {
    size_t capacity;
    size_t length;
    void **data;
} vector_t;

extern vector_t *vector_new(void);
extern void *vector_at(vector_t *, size_t);
extern void *vector_back(vector_t *);
extern void vector_pop_back(vector_t *);
extern void vector_push_back(vector_t *, void *);
extern void vector_reverse(vector_t *, size_t);
