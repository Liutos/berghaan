#include "base/log.h"
#include "base/vector.h"
#include "bif.h"
#include "env.h"
#include "object.h"
#include "op.h"
#include "store.h"
#include "vm.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

typedef struct {
    int pc;
} frame_t;

vector_t *toplevel_vec = NULL;

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

static void
vm_call_native(vm_t *vm, object_t *f)
{
    assert(f->type == OBJECT_FUN && OBJECT_FUN_TYPE(f) == FUN_NATIVE);
    object_t *result;
    void *impl = OBJECT_FUN_NATIVE_IMPL(f);
    object_t *arg0, *arg1, *arg2;
    switch (OBJECT_FUN_NATIVE_ARITY(f)) {
        case 0:
            result = ((bif_0_t)impl)(vm);
            break;
        case 1:
            arg0 = vm_pop_data(vm);
            result = ((bif_1_t)impl)(vm, arg0);
            break;
        case 2:
            arg1 = vm_pop_data(vm);
            arg0 = vm_pop_data(vm);
            result = ((bif_2_t)impl)(vm, arg0, arg1);
            break;
        case 3:
            arg2 = vm_pop_data(vm);
            arg1 = vm_pop_data(vm);
            arg0 = vm_pop_data(vm);
            result = ((bif_3_t)impl)(vm, arg0, arg1, arg2);
            break;
        default :
            exit(EXIT_FAILURE);
    }
    vm_push_data(vm, result);
}

error_t *
error_new(const char *launcher, const char *message)
{
    error_t *e = calloc(1, sizeof(error_t));
    e->launcher = launcher;
    e->message = string_new();
    string_append(e->message, message);
    return e;
}

vm_t *
vm_new(void)
{
    vm_t *vm = calloc(1, sizeof(vm_t));
    vm->erroneous = false;
    vm->error = NULL;
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
    object_t *obj;
    while (true) {
        assert(pc < code->length);
        prev_pc = pc;
        op_t *op = vector_at(code, pc);

        log_lprintf(LOG_DEBUG, "VM:EXECUTE:OPCODE %s at %d", op_name(op), pc);

        switch (op->type) {
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
                obj = vm_pop_data(vm);
                if (OBJECT_FUN_TYPE(obj) == FUN_NATIVE) {
                    vm_call_native(vm, obj);
                    NEXT;
                }
                assert(OBJECT_FUN_TYPE(obj) == FUN_UDF);
                // 保存现场信息
                frame = frame_new(pc);
                vm_push_frame(vm, frame);
                // 跳转到函数体
                pc = OBJECT_FUN_UDF_ENTRY(obj);
                break;
            case OP_FUN:
                obj = object_fun_udf_new(OP_ARG0(op));
                vm_push_data(vm, obj);
                NEXT;
            case OP_GENV:
                vector_reserve(toplevel_vec, toplevel_vec->length + OP_ARG0(op));
                NEXT;
            case OP_GREF:
                obj = vector_unsafe_at(toplevel_vec, OP_GREF_Y(op));
                vm_push_data(vm, obj);
                NEXT;
            case OP_GSET:
                obj = vm_last_data(vm);
                vector_set(toplevel_vec, OP_GSET_Y(op), obj);
                NEXT;
            case OP_HALT:
                exit(EXIT_SUCCESS);
                break;
            case OP_JUMP:
                pc = OP_ARG0(op);
                break;
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
            case OP_TJUMP:
                obj = vm_last_data(vm);
                vm_pop_data(vm);
                assert(obj->type == OBJECT_BOOL);
                if (OBJECT_BOOL_VALUE(obj) == true)
                    pc = OP_ARG0(op);
                else
                    pc++;
                break;
            case OP_TUNWIND:
                if (vm->erroneous) {
                    if (vm->frame_stack->length > 0) {
                        printf("展开栈\n");
                        frame = vm_pop_frame(vm);
                        pc = frame->pc;
                    } else {
                        fprintf(stderr, "%s: %s\n", vm->error->launcher, vm->error->message->data);
                        exit(EXIT_FAILURE);
                    }
                }
                NEXT;
            default :
                printf("Undefined instruction: %s\n", op_name(op));
                exit(EXIT_FAILURE);
        }
        assert(pc != prev_pc);
    }
    assert(vm->data_stack->length == 1);
#undef NEXT
}
