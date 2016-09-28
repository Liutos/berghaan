#include "assembler.h"
#include "base/list.h"

static assoc_list_t *
scan_labels(code_t *code)
{
    assoc_list_t *map = assoc_list_new();
    int offset = 0;
    for (size_t i = 0; i < code->length; i++) {
        op_t *op = code_at(code, i);
        if (op->type == OP_LABEL) {
            assoc_list_push(map, OP_LABEL_NAME(op), (void *)offset);
        } else {
            offset++;
        }
    }
    return map;
}

code_t *relocate(code_t *code)
{
    // 第一个阶段负责扫描字节码中所有的标号的偏移
    assoc_list_t *map = scan_labels(code);
    // 第二个阶段将所有标号替换为数值偏移
    code_t *ins = code_new();
    int offset;
    const char *name;
    for (size_t i = 0; i < code->length; i++) {
        op_t *op = code_at(code, i);
        if (op->type == OP_LABEL)
            continue;
        switch (op->type) {
            case OP_FUN:
            case OP_JUMP:
            case OP_TJUMP:
                name = (const char *)OP_ARG0(op);
                offset = (int)assoc_list_search(map, name);
                OP_ARG0(op) = (intptr_t)offset;
                OP_ARG1(op) = (intptr_t)name;
                emit(ins, op);
                break;
            default :
                emit(ins, op);
        }
    }
    return ins;
}
