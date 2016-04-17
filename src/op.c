#include "op.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

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

op_t *
op_next_label(const char *prefix)
{
    static int counter = 0;
    char label[32] = {0};
    snprintf(label, sizeof(label), "%s_%d", prefix, counter);
    counter++;
    return OP_NEW1(OP_LABEL, strdup(label));
}

void
op_print(op_t *op, FILE *out)
{
    if (op->type != OP_LABEL)
        fprintf(out, "  %s", op_name(op));
    switch (op->type) {
        case OP_ARG:
            fprintf(out, " %"PRIiPTR, OP_ARG_ARITY(op));
            break;
        case OP_FUN:
            fprintf(out, " %"PRIiPTR, (intptr_t)OP_FUN_NAME(op));
            break;
        case OP_GENV:
            fprintf(out, " %"PRIiPTR, OP_ARG0(op));
            break;
        case OP_GREF:
            fprintf(out, " %"PRIiPTR" %"PRIiPTR, OP_GREF_X(op), OP_GREF_Y(op));
            break;
        case OP_GSET:
            fprintf(out, " %"PRIiPTR" %"PRIiPTR, OP_GSET_X(op), OP_GSET_Y(op));
            break;
        case OP_JUMP:
            fprintf(out, " %"PRIiPTR, (intptr_t)OP_JUMP_LABEL(op));
            break;
        case OP_LABEL:
            fprintf(out, "%s:", OP_LABEL_NAME(op));
            break;
        case OP_REF:
            fprintf(out, " %"PRIiPTR" %"PRIiPTR, OP_REF_X(op), OP_REF_Y(op));
            break;
        case OP_TJUMP:
            fprintf(out, " %"PRIiPTR, (intptr_t)OP_TJUMP_LABEL(op));
            break;
        default :;
    }
    fprintf(out, "\n");
}

void
code_print(code_t *s, FILE *out)
{
    for (size_t i = 0; i < s->length; i++) {
        fprintf(out, "%lu\t", i);
        op_print(code_at(s, i), out);
    }
}

void
emit(code_t *s, op_t *op)
{
    vector_push_back(s, op);
}

code_t *
code_concat(code_t *dest, code_t *code)
{
    return vector_concat(dest, code);
}

op_t *
code_at(code_t *code, size_t index)
{
    return (op_t *)vector_at(code, index);
}
