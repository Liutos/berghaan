/**
 * 提供字节码虚拟机的定义及其相关操作
 */
#pragma once

#include "base/vector.h"
#include "store.h"

typedef struct {
    store_t *env;
    vector_t *data_stack;
    vector_t *frame_stack;
} vm_t;

extern vector_t *toplevel_vec;

extern vm_t *vm_new(void);
extern void vm_execute(vm_t *, vector_t *);
