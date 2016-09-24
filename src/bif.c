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
bif_add(object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) + OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_sub(object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) - OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_mul(object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) * OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_div(object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_int_new(OBJECT_INT_VALUE(lhs) / OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_equal(object_t *lhs, object_t *rhs)
{
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_bool_new(OBJECT_INT_VALUE(lhs) == OBJECT_INT_VALUE(rhs));
}

static object_t *
bif_code2char(object_t *num)
{
    assert(num->type == OBJECT_INT);
    return object_char_new(OBJECT_INT_VALUE(num));
}

static object_t *
bif_eq(object_t *x, object_t *y)
{
    return object_bool_new(x == y);
}

static object_t *
bif_make_map(void)
{
    return object_map_new();
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
            { .name = "code-char", .impl = (void *)bif_code2char, .arity = 1 },
            { .name = "eq", .impl = (void *)bif_eq, .arity = 2 },
            { .name = "make-map", .impl = (void *)bif_make_map, .arity = 0 },
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
