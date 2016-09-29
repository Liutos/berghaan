/**
 * 提供字节码虚拟机的定义及其相关操作
 */
#pragma once

#include "base/string.h"
#include "base/vector.h"
#include "store.h"

#include <stdbool.h>

typedef struct {
    char *launcher;
    string_t *message;
} error_t;

typedef struct {
    bool erroneous;
    error_t *error;
    store_t *env;
    vector_t *data_stack;
    vector_t *frame_stack;
} vm_t;

extern vector_t *toplevel_vec;

extern error_t *error_new(const char *, const char *);
extern vm_t *vm_new(void);
extern void vm_execute(vm_t *, vector_t *);
