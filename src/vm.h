/**
 * 提供字节码虚拟机的定义及其相关操作
 */
#pragma once

#include "base/vector.h"
#include "env.h"

typedef struct {
    env_t *env;
    vector_t *data_stack;
} vm_t;

extern vm_t *vm_new(void);
extern void vm_execute(vm_t *, vector_t *);
