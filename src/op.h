/**
 * 提供字节码指令集的定义及相关操作
 */
#pragma once

#include <stdio.h>
#include <stdint.h>

#define IDENTITY(x) x
#define OPS(op) op(OP_ARG), \
    op(OP_CALL), \
    op(OP_FUN), \
    op(OP_GENV), \
    op(OP_GREF), \
    op(OP_GSET), \
    op(OP_HALT), \
    op(OP_JUMP), \
    op(OP_LABEL), \
    op(OP_NIL), \
    op(OP_POP), \
    op(OP_PRINT), \
    op(OP_PUSH), \
    op(OP_REF), \
    op(OP_RET), \
    op(OP_TJUMP),
#define STRINGIFY(x) #x

typedef enum {
    OPS(IDENTITY)
} OP_T;

typedef struct {
    OP_T type;
    intptr_t arg0;
    intptr_t arg1;
} op_t;

extern const char *op_name(op_t *);
extern op_t *op_new0(OP_T);
extern op_t *op_new1(OP_T, intptr_t);
extern op_t *op_new2(OP_T, intptr_t, intptr_t);
extern op_t *op_next_label(const char *);
extern void op_print(op_t *, FILE *);

#define OP_NEW0(t) op_new0(t)
#define OP_NEW1(t, a0) op_new1(t, (intptr_t)a0)
#define OP_NEW2(t, a0, a1) op_new2(t, (intptr_t)a0, (intptr_t)a1)
#define OP_ARG0(x) ((x)->arg0)
#define OP_ARG1(x) ((x)->arg1)
#define OP_ARG_ARITY(x) OP_ARG0(x)
#define OP_FUN_NAME(x) ((const char *)OP_ARG0(x))
#define OP_GREF_X(x) OP_ARG0(x)
#define OP_GREF_Y(x) OP_ARG1(x)
#define OP_GSET_X(x) OP_ARG0(x)
#define OP_GSET_Y(x) OP_ARG1(x)
#define OP_JUMP_LABEL(x) ((const char *)OP_ARG0(x))
#define OP_LABEL_NAME(x) ((const char *)OP_ARG0(x))
#define OP_PUSH_TARGET(x) OP_ARG0(x)
#define OP_REF_X(x) OP_ARG0(x)
#define OP_REF_Y(x) OP_ARG1(x)
#define OP_TJUMP_LABEL(x) ((const char *)OP_ARG0(x))
