/**
 * 提供字节码指令集的定义及相关操作
 */
#pragma once

#include <stdint.h>

typedef enum {
    OP_GSET,
    OP_HALT,
    OP_POP,
    OP_PRINT,
    OP_PUSH,
    OP_REF,
} OP_T;

typedef struct {
    OP_T type;
    intptr_t arg0;
    intptr_t arg1;
} op_t;

extern op_t *op_new0(OP_T);
extern op_t *op_new1(OP_T, intptr_t);
extern op_t *op_new2(OP_T, intptr_t, intptr_t);

#define OP_NEW0(t) op_new0(t)
#define OP_NEW1(t, a0) op_new1(t, (intptr_t)a0)
#define OP_NEW2(t, a0, a1) op_new2(t, (intptr_t)a0, (intptr_t)a1)
#define OP_ARG0(x) ((x)->arg0)
#define OP_ARG1(x) ((x)->arg1)
#define OP_GSET_NAME(x) ((char *)OP_ARG0(x))
#define OP_PUSH_TARGET(x) OP_ARG0(x)
#define OP_REF_X(x) OP_ARG0(x)
#define OP_REF_Y(x) OP_ARG1(x)
