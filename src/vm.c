#include "object.h"
#include "op.h"
#include "vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void
vm_pop_data(vm_t *vm)
{
    vector_pop_back(vm->data_stack);
}

static void
vm_push_data(vm_t *vm, object_t *obj)
{
    vector_push_back(vm->data_stack, obj);
}

static object_t *
vm_last_data(vm_t *vm)
{
    return (object_t *)vector_back(vm->data_stack);
}

vm_t *
vm_new(void)
{
    vm_t *vm = calloc(1, sizeof(vm_t));
    vm->data_stack = vector_new();
    return vm;
}

void
vm_execute(vm_t *vm, vector_t *code)
{
    size_t pc = 0, prev_pc;
    object_t *obj;
    while (true) {
        assert(pc < code->length);
        prev_pc = pc;
        op_t *op = vector_at(code, pc);
        switch (op->type) {
            case OP_HALT:
                exit(EXIT_SUCCESS);
                break;
            case OP_POP:
                vm_pop_data(vm);
                pc++;
                break;
            case OP_PRINT:
                obj = vm_last_data(vm);
                object_print(obj);
                puts("");
                pc++;
                break;
            case OP_PUSH:
                vm_push_data(vm, (object_t *)OP_PUSH_TARGET(op));
                pc++;
                break;
        }
        assert(pc != prev_pc);
    }
    assert(vm->data_stack->length == 1);
}
