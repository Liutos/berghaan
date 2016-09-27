#include "bif.h"
#include "object.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    char *name;
    void *impl;
    int arity;
} bif_t;

static object_t *
bif_add(vm_t *vm, object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) + OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_sub(vm_t *vm, object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) - OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_mul(vm_t *vm, object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) * OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_div(vm_t *vm, object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) / OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_equal(vm_t *vm, object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_bool_new(OBJECT_INT_VALUE(lhs) == OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_char2code(vm_t *vm, object_t *c)
{
    assert(c->type == OBJECT_CHAR);
    return object_int_new(OBJECT_CHAR_VALUE(c));
}

static object_t *
bif_code2char(vm_t *vm, object_t *num)
{
    assert(num->type == OBJECT_INT);
    return object_char_new(OBJECT_INT_VALUE(num));
}

static object_t *
bif_eq(vm_t *vm, object_t *x, object_t *y)
{
    return object_bool_new(x == y);
}

static object_t *
bif_make_map(vm_t *vm)
{
    return object_map_new();
}

static object_t *
bif_make_vector(vm_t *vm)
{
    return object_vector_new();
}

static object_t *
bif_map_get(vm_t *vm, object_t *map, object_t *key)
{
    assert(map->type == OBJECT_MAP);
    return hash_table_get(map->u.map, key);
}

static object_t *
bif_map_set(vm_t *vm, object_t *map, object_t *key, object_t *value)
{
    assert(map->type == OBJECT_MAP);
    hash_table_set(map->u.map, key, value);
    return value;
}

static object_t *
bif_vector_pop_back(vm_t *vm, object_t *vector)
{
    assert(vector->type == OBJECT_VECTOR);
    object_t *top = vector_back(vector->u.vector);
    vector_pop_back(vector->u.vector);
    return top;
}

static object_t *
bif_vector_push_back(vm_t *vm, object_t *vector, object_t *value)
{
    assert(vector->type == OBJECT_VECTOR);
    vector_push_back(vector->u.vector, value);
    return vector;
}

void
bif_init(env_t **env, vector_t **vec)
{
    bif_t bif[] = {
            { .name = "+", .impl = (void *)bif_add, .arity = 2 },
            { .name = "-", .impl = (void *)bif_sub, .arity = 2 },
            { .name = "*", .impl = (void *)bif_mul, .arity = 2 },
            { .name = "/", .impl = (void *)bif_div, .arity = 2 },
            { .name = "=", .impl = (void *)bif_equal, .arity = 2 },
            { .name = "char-code", .impl = (void *)bif_char2code, .arity = 1 },
            { .name = "code-char", .impl = (void *)bif_code2char, .arity = 1 },
            { .name = "eq", .impl = (void *)bif_eq, .arity = 2 },
            { .name = "make-map", .impl = (void *)bif_make_map, .arity = 0 },
            { .name = "make-vector", .impl = (void *)bif_make_vector, .arity = 0 },
            { .name = "map-get", .impl = (void *)bif_map_get, .arity = 2 },
            { .name = "map-set", .impl = (void *)bif_map_set, .arity = 3 },
            { .name = "vector-pop", .impl = (void *)bif_vector_pop_back, .arity = 1 },
            { .name = "vector-push", .impl = (void *)bif_vector_push_back, .arity = 2 },
    };
    int len = sizeof(bif) / sizeof(*bif);
    // 初始化编译器环境
    env_t *_env = env_new(NULL);
    for (int i = 0; i < len; i++) {
        const char *name = bif[i].name;
        env_push_back(_env, name, NULL);
    }
    // 初始化虚拟机存储
    vector_t *_vec = vector_new();
    vector_reserve(_vec, len);
    for (int i = 0; i < len; i++) {
        object_t *f = object_fun_native_new(bif[i].arity, bif[i].impl);
        vector_push_back(_vec, f);
    }
    *env = _env;
    *vec = _vec;
}
