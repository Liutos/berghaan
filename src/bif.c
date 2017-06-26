#include "base/hash.h"
#include "bif.h"
#include "object.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    char *name;
    char *owner;
    void *impl;
    int arity;
} bif_t;

#define EXPECT_TYPE(vm, x, target) \
    do { \
        assert(x != NULL); \
        if (x->type != target) { \
            vm->erroneous = true; \
            char message[256] = {0}; \
            snprintf(message, sizeof(message), "类型不匹配。期望`%d`(%s)，但收到了`%d`", target, #x, x->type); \
            vm->error = error_new(__func__, message); \
            return NULL; \
        } \
    } while (0)

static hash_table_t *servants = NULL;

static object_t *
bif_add(vm_t *vm, object_t *lhs, object_t *rhs)
{
    EXPECT_TYPE(vm, lhs, OBJECT_INT);
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
    if (OBJECT_INT_VALUE(rhs) == 0) {
        vm->erroneous = true;
        vm->error = error_new(__func__, "除数为0");
        return NULL;
    }
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
bif_error(vm_t *vm, object_t *msg)
{
    EXPECT_TYPE(vm, msg, OBJECT_STRING);
    vm->erroneous = true;
    vm->error = error_new(__func__, object_string_to_chars(msg));
    return NULL;
}

static object_t *
bif_make_map(vm_t *vm)
{
    return object_map_new();
}

static object_t *
bif_make_string(vm_t *vm, object_t *content)
{
    EXPECT_TYPE(vm, content, OBJECT_VECTOR);
    vector_t *dest = vector_new();
    vector_t *src = content->u.vector;
    for (size_t i = 0; i < src->length; i++) {
        vector_push_back(dest, vector_at(src, i));
    }
    return object_string_new(dest);
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
bif_oblate(vm_t *vm, object_t *name, object_t *servant)
{
    EXPECT_TYPE(vm, name, OBJECT_SYMBOL);
    EXPECT_TYPE(vm, servant, OBJECT_MAP);
    hash_table_set(servants, (void *)name, (void *)servant);
    return servant;
}

static object_t *
bif_summon(vm_t *vm, object_t *name)
{
    EXPECT_TYPE(vm, name, OBJECT_SYMBOL);
    object_t *servant = (object_t *)hash_table_get(servants, (void *)name);
    if (servant)
        return servant;
    return object_nil_new();
}

static object_t *
bif_type_of(vm_t *vm, object_t *any)
{
    OBJECT_T type_id = any->type;
    return object_get_type(type_id);
}

static object_t *
bif_vector_is_empty(vm_t *vm, object_t *vector)
{
    EXPECT_TYPE(vm, vector, OBJECT_VECTOR);
    return object_bool_new(vector_is_empty(vector->u.vector));
}

static object_t *
bif_vector_pop_back(vm_t *vm, object_t *vector)
{
    EXPECT_TYPE(vm, vector, OBJECT_VECTOR);
    object_t *top = vector_back(vector->u.vector);
    vector_pop_back(vector->u.vector);
    return top;
}

static object_t *
bif_vector_push_back(vm_t *vm, object_t *vector, object_t *value)
{
    EXPECT_TYPE(vm, vector, OBJECT_VECTOR);
    vector_push_back(vector->u.vector, value);
    return vector;
}

static object_t *
servant_ensure_exist(const char *name)
{
    object_t *servant_name = object_symbol_intern(name);
    object_t *servant = hash_table_get(servants, (void *)servant_name);
    if (servant == NULL) {
        servant = object_map_new();
        hash_table_set(servants, (void *)servant_name, servant);
    }
    return servant;
}

static void
servant_init(void)
{
    servants = hash_table_new(hash_pointer_equal, hash_pointer_hash);
}

static void
servant_learn(object_t *servant, const char *name, object_t *skill)
{
    assert(servant->type == OBJECT_MAP);
    hash_table_set(servant->u.map, (void *)object_symbol_intern(name), (void *)skill);
}

void
bif_init(env_t **env, vector_t **vec)
{
    servant_init();
    bif_t bif[] = {
            { .name = "+", .owner = NULL, .impl = (void *)bif_add, .arity = 2 },
            { .name = "-", .owner = NULL, .impl = (void *)bif_sub, .arity = 2 },
            { .name = "*", .owner = NULL, .impl = (void *)bif_mul, .arity = 2 },
            { .name = "/", .owner = NULL, .impl = (void *)bif_div, .arity = 2 },
            { .name = "=", .owner = NULL, .impl = (void *)bif_equal, .arity = 2 },
            { .name = "char-code", .owner = "char", .impl = (void *)bif_char2code, .arity = 1 },
            { .name = "code-char", .owner = "char", .impl = (void *)bif_code2char, .arity = 1 },
            { .name = "eq", .owner = NULL, .impl = (void *)bif_eq, .arity = 2 },
            { .name = "error", .owner = NULL, .impl = (void *)bif_error, .arity = 1 },
            { .name = "make-map", .owner = "map", .impl = (void *)bif_make_map, .arity = 0 },
            { .name = "make-string", .owner = "string", .impl = (void *)bif_make_string, .arity = 1 },
            { .name = "make-vector", .owner = "vector", .impl = (void *)bif_make_vector, .arity = 0 },
            { .name = "map-get", .owner = "map", .impl = (void *)bif_map_get, .arity = 2 },
            { .name = "map-set", .owner = "map", .impl = (void *)bif_map_set, .arity = 3 },
            { .name = "oblate", .owner = NULL, .impl = (void *)bif_oblate, .arity = 2 },
            { .name = "summon", .owner = NULL, .impl = (void *)bif_summon, .arity = 1 },
            { .name = "type-of", .owner = NULL, .impl = (void *)bif_type_of, .arity = 1 },
            { .name = "vector-is-empty?", .owner = "vector", .impl = (void *)bif_vector_is_empty, .arity = 1 },
            { .name = "vector-pop", .owner = "vector", .impl = (void *)bif_vector_pop_back, .arity = 1 },
            { .name = "vector-push", .owner = "vector", .impl = (void *)bif_vector_push_back, .arity = 2 },
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
    vector_reserve(_vec, len > 256 ? len : 256);
    for (int i = 0; i < len; i++) {
        object_t *f = object_fun_native_new(bif[i].arity, bif[i].impl);
        vector_push_back(_vec, f);
        if (bif[i].owner != NULL) {
            object_t *servant = servant_ensure_exist(bif[i].owner);
            servant_learn(servant, bif[i].name, f);
        }
    }
    *env = _env;
    *vec = _vec;
}
