#include "op.h"

#include <inttypes.h>
#include <stdlib.h>

static const char *op_names[] = {
        OPS(STRINGIFY)
};

const char *
op_name(op_t *op)
{
    return op_names[op->type];
}

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

void
op_print(op_t *op, FILE *out)
{
    if (op->type != OP_LABEL)
        fprintf(out, "%s", op_name(op));
    switch (op->type) {
        case OP_ARG:
            fprintf(out, " %"PRIiPTR, OP_ARG_ARITY(op));
            break;
        case OP_FUN:
            fprintf(out, " %s", OP_FUN_NAME(op));
            break;
        case OP_GSET:
            fprintf(out, " %s", OP_GSET_NAME(op));
            break;
        case OP_LABEL:
            fprintf(out, "%s:", OP_LABEL_NAME(op));
            break;
        case OP_REF:
            fprintf(out, " %"PRIiPTR" %"PRIiPTR, OP_REF_X(op), OP_REF_Y(op));
            break;
        default :;
    }
    fprintf(out, "\n");
}
