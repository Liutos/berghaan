#include "base/vector.h"
#include "env.h"
#include "object.h"
#include "op.h"
#include "store.h"
#include "vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pc;
} frame_t;

static vector_t *toplevel_env = NULL;

static frame_t *
frame_new(int pc)
{
    frame_t *f = calloc(1, sizeof(frame_t));
    f->pc = pc;
    return f;
}

static object_t *
vm_last_data(vm_t *vm)
{
    return (object_t *)vector_back(vm->data_stack);
}

static object_t *
vm_pop_data(vm_t *vm)
{
    object_t *o = vm_last_data(vm);
    vector_pop_back(vm->data_stack);
    return o;
}

static frame_t *
vm_pop_frame(vm_t *vm)
{
    frame_t *f = (frame_t *)vector_back(vm->frame_stack);
    vector_pop_back(vm->frame_stack);
    return f;
}

static void
vm_push_data(vm_t *vm, object_t *obj)
{
    vector_push_back(vm->data_stack, obj);
}

static void
vm_push_frame(vm_t *vm, frame_t *frame)
{
    vector_push_back(vm->frame_stack, frame);
}

static object_t *
vm_reference(vm_t *vm, int x, int y)
{
    return store_get(vm->env, x, y);
}

vm_t *
vm_new(void)
{
    vm_t *vm = calloc(1, sizeof(vm_t));
    vm->env = NULL;
    vm->data_stack = vector_new();
    vm->frame_stack = vector_new();
    return vm;
}

void
vm_execute(vm_t *vm, vector_t *code)
{
#define NEXT pc++; break

    size_t pc = 0, prev_pc;
    store_t *env;
    frame_t *frame;
    object_t *lhs, *obj, *rhs;
    while (true) {
        assert(pc < code->length);
        prev_pc = pc;
        op_t *op = vector_at(code, pc);
        switch (op->type) {
            case OP_ADD:
                rhs = vm_pop_data(vm);
                lhs = vm_pop_data(vm);
                vm_push_data(vm, object_int_new(OBJECT_INT_VALUE(lhs) + OBJECT_INT_VALUE(rhs)));
                NEXT;
            case OP_ARG:
                // 创建存储空间
                env = store_new(OP_ARG0(op), vm->env);
                // 填充存储空间
                for (int i = 0; i < OP_ARG0(op); i++) {
                    obj = vm_last_data(vm);
                    vm_pop_data(vm);
                    int index = OP_ARG0(op) - i - 1;
                    store_set(env, 0, index, obj);
                }
                // 替换当前环境
                vm->env = env;
                NEXT;
            case OP_CALL:
                // 保存现场信息
                frame = frame_new(pc);
                vm_push_frame(vm, frame);
                // 跳转到函数体
                obj = vm_last_data(vm);
                vm_pop_data(vm);
                pc = OBJECT_FUN_UDF_ENTRY(obj);
                break;
            case OP_DIV:
                rhs = vm_pop_data(vm);
                lhs = vm_pop_data(vm);
                vm_push_data(vm, object_int_new(OBJECT_INT_VALUE(lhs) / OBJECT_INT_VALUE(rhs)));
                NEXT;
            case OP_FUN:
                obj = object_fun_udf_new(OP_ARG0(op));
                vm_push_data(vm, obj);
                NEXT;
            case OP_GENV:
                toplevel_env = vector_new();
                vector_reserve(toplevel_env, OP_ARG0(op));
                NEXT;
            case OP_GREF:
                obj = vector_unsafe_at(toplevel_env, OP_GREF_Y(op));
                vm_push_data(vm, obj);
                NEXT;
            case OP_GSET:
                obj = vm_last_data(vm);
                vector_set(toplevel_env, OP_GSET_Y(op), obj);
                NEXT;
            case OP_HALT:
                exit(EXIT_SUCCESS);
                break;
            case OP_JUMP:
                pc = OP_ARG0(op);
                break;
            case OP_MUL:
                rhs = vm_pop_data(vm);
                lhs = vm_pop_data(vm);
                vm_push_data(vm, object_int_new(OBJECT_INT_VALUE(lhs) * OBJECT_INT_VALUE(rhs)));
                NEXT;
            case OP_NIL:
                vm_push_data(vm, object_nil_new());
                NEXT;
            case OP_POP:
                vm_pop_data(vm);
                NEXT;
            case OP_PRINT:
                obj = vm_last_data(vm);
                object_print(obj);
                puts("");
                NEXT;
            case OP_PUSH:
                vm_push_data(vm, (object_t *)OP_PUSH_TARGET(op));
                NEXT;
            case OP_REF:
                obj = vm_reference(vm, OP_REF_X(op), OP_REF_Y(op));
                vm_push_data(vm, obj);
                NEXT;
            case OP_RET:
                frame = vm_pop_frame(vm);
                pc = frame->pc;
                NEXT;
            case OP_SUB:
                rhs = vm_pop_data(vm);
                lhs = vm_pop_data(vm);
                vm_push_data(vm, object_int_new(OBJECT_INT_VALUE(lhs) - OBJECT_INT_VALUE(rhs)));
                NEXT;
            case OP_TJUMP:
                obj = vm_last_data(vm);
                vm_pop_data(vm);
                assert(obj->type == OBJECT_BOOL);
                if (OBJECT_BOOL_VALUE(obj) == true)
                    pc = OP_ARG0(op);
                else
                    pc++;
                break;
            default :
                printf("Undefined instruction: %s\n", op_name(op));
                exit(EXIT_FAILURE);
        }
        assert(pc != prev_pc);
    }
    assert(vm->data_stack->length == 1);
#undef NEXT
}
