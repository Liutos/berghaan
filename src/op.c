#include "op.h"

#include <stdlib.h>

op_t *
op_new0(OP_T type)
{
    op_t *op = calloc(1, sizeof(op_t));
    op->type = type;
    return op;
}

op_t *
op_new1(OP_T type, intptr_t arg0)
{
    op_t *op = op_new0(type);
    op->arg0 = arg0;
    return op;
}

op_t *
op_new2(OP_T type, intptr_t arg0, intptr_t arg1)
{
    op_t *op = op_new1(type, arg0);
    op->arg1 = arg1;
    return op;
}
