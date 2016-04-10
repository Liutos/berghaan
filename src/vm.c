#include "env.h"
#include "object.h"
#include "op.h"
#include "vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static env_t *toplevel_env = NULL;

static void
vm_gset(vm_t *vm __attribute__ ((unused)), const char *name, object_t *val)
{
    env_push_back(toplevel_env, name, val);
}

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

static object_t *
vm_reference(vm_t *vm, int x, int y)
{
    return (object_t *)env_at(vm->env, x, y);
}

vm_t *
vm_new(void)
{
    vm_t *vm = calloc(1, sizeof(vm_t));
    vm->env = toplevel_env;
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
            case OP_REF:
                obj = vm_reference(vm, OP_REF_X(op), OP_REF_Y(op));
                vm_push_data(vm, obj);
                pc++;
                break;
            case OP_GSET:
                obj = vm_last_data(vm);
                vm_gset(vm, OP_GSET_NAME(op), obj);
                pc++;
                break;
        }
        assert(pc != prev_pc);
    }
    assert(vm->data_stack->length == 1);
}

void
vm_init(void)
{
    toplevel_env = env_new(NULL);
}
