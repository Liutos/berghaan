#include "base/string.h"
#include "base/utf8.h"
#include "base/vector.h"
#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static hash_table_t *string_symbol_map = NULL;

static object_t type_objects[] = {
    { .type = OBJECT_TYPE, .u = { .type = { .name = "boolean", .type_id = OBJECT_BOOL, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "character", .type_id = OBJECT_CHAR, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "function", .type_id = OBJECT_FUN, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "integer", .type_id = OBJECT_INT, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "map", .type_id = OBJECT_MAP, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "nil", .type_id = OBJECT_NIL, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "string", .type_id = OBJECT_STRING, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "symbol", .type_id = OBJECT_SYMBOL, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "type", .type_id = OBJECT_TYPE, } } },
    { .type = OBJECT_TYPE, .u = { .type = { .name = "vector", .type_id = OBJECT_VECTOR, } } },
};

static void
object_bool_print(object_t *b)
{
    assert(b->type == OBJECT_BOOL);
    printf("%s", OBJECT_BOOL_VALUE(b) ? "true" : "false");
}

static void
object_char_print(object_t *c)
{
    assert(c->type == OBJECT_CHAR);
    printf("'");
    utf8_fprintf(stdout, OBJECT_CHAR_VALUE(c));
    printf("'");
}

static void
object_fun_print(object_t *f)
{
    assert(f->type == OBJECT_FUN);
    printf("#<FUN ");
    if (OBJECT_FUN_TYPE(f) == FUN_NATIVE)
        printf("native");
    else
        printf("udf");
    printf("%p>", f);
}

static void
object_int_print(object_t *n)
{
    assert(n->type == OBJECT_INT);
    printf("%d", OBJECT_INT_VALUE(n));
}

static void
object_map_print(object_t *m)
{
    assert(m->type == OBJECT_MAP);
    printf("{}");
}

static void
object_nil_print(object_t *n)
{
    assert(n->type == OBJECT_NIL);
    printf("nil");
}

static void
object_string_print(const object_t *s)
{
    assert(s->type == OBJECT_STRING);
    printf("\"");
    vector_t *content = OBJECT_STRING_CONTENT(s);
    for (int i = 0; i < content->length; i++) {
        object_t *c = vector_at(content, i);
        utf8_fprintf(stdout, OBJECT_CHAR_VALUE(c));
    }
    printf("\"");
}

static void
object_symbol_print(object_t *s)
{
    assert(s->type == OBJECT_SYMBOL);
    printf(":%s", OBJECT_SYMBOL_NAME(s));
}

static void
object_type_print(object_t *t)
{
    assert(t->type == OBJECT_TYPE);
    printf("#<TYPE %d %s>", OBJECT_TYPE_ID(t), OBJECT_TYPE_NAME(t));
}

static void
object_vector_print(object_t *v)
{
    assert(v->type == OBJECT_VECTOR);
    printf("[");
    vector_t *vector = v->u.vector;
    for (int i = 0; i < vector->length; i++) {
        object_print(vector_at(vector, i));
        if (i != vector->length - 1) {
            printf(", ");
        }
    }
    printf("]");
}

static object_t *
object_new(OBJECT_T type)
{
    object_t *o = calloc(1, sizeof(object_t));
    o->type = type;
    return o;
}

object_t *
object_bool_new(bool value)
{
    object_t *b = calloc(1, sizeof(object_t));
    b->type = OBJECT_BOOL;
    OBJECT_BOOL_VALUE(b) = value;
    return b;
}

object_t *
object_char_new(uint32_t value)
{
    object_t *c = calloc(1, sizeof(object_t));
    c->type = OBJECT_CHAR;
    OBJECT_CHAR_VALUE(c) = value;
    return c;
}

object_t *
object_fun_native_new(int arity, void *impl)
{
    object_t *f = object_new(OBJECT_FUN);
    OBJECT_FUN_TYPE(f) = FUN_NATIVE;
    OBJECT_FUN_NATIVE_ARITY(f) = arity;
    OBJECT_FUN_NATIVE_IMPL(f) = impl;
    return f;
}

object_t *
object_fun_udf_new(int entry)
{
    object_t *f = object_new(OBJECT_FUN);
    OBJECT_FUN_TYPE(f) = FUN_UDF;
    OBJECT_FUN_UDF_ENTRY(f) = entry;
    return f;
}

object_t *
object_int_new(int value)
{
    object_t *n = calloc(1, sizeof(object_t));
    n->type = OBJECT_INT;
    OBJECT_INT_VALUE(n) = value;
    return n;
}

object_t *
object_map_new(void)
{
    object_t *m = object_new(OBJECT_MAP);
    m->u.map = hash_table_new(hash_pointer_equal, hash_pointer_hash);
    return m;
}

object_t *
object_nil_new(void)
{
    object_t *n = calloc(1, sizeof(object_t));
    n->type = OBJECT_NIL;
    return n;
}

object_t *
object_string_new(vector_t *content)
{
    object_t *s = object_new(OBJECT_STRING);
    OBJECT_STRING_CONTENT(s) = content;
    return s;
}

object_t *
object_symbol_new(const char *name)
{
    object_t *s = object_new(OBJECT_SYMBOL);
    OBJECT_SYMBOL_NAME(s) = strdup(name);
    return s;
}

object_t *
object_symbol_intern(const char *name)
{
    if (string_symbol_map == NULL) {
        string_symbol_map = hash_table_new(hash_string_equal, hash_string_hash);
    }
    object_t *symbol = hash_table_get(string_symbol_map, name);
    if (symbol == NULL) {
        symbol = object_symbol_new(name);
        hash_table_set(string_symbol_map, name, symbol);
    }
    return symbol;
}

object_t *
object_vector_new(void)
{
    object_t *v = object_new(OBJECT_VECTOR);
    v->u.vector = vector_new();
    return v;
}

void
object_print(object_t *x)
{
    switch (x->type) {
        case OBJECT_BOOL:
            object_bool_print(x);
            break;
        case OBJECT_CHAR:
            object_char_print(x);
            break;
        case OBJECT_FUN:
            object_fun_print(x);
            break;
        case OBJECT_INT:
            object_int_print(x);
            break;
        case OBJECT_MAP:
            object_map_print(x);
            break;
        case OBJECT_NIL:
            object_nil_print(x);
            break;
        case OBJECT_STRING:
            object_string_print(x);
            break;
        case OBJECT_SYMBOL:
            object_symbol_print(x);
            break;
        case OBJECT_TYPE:
            object_type_print(x);
            break;
        case OBJECT_VECTOR:
            object_vector_print(x);
            break;
    }
}

void
object_vector_reserve(object_t *v, size_t length)
{
    assert(v->type == OBJECT_VECTOR);
    vector_reserve(v->u.vector, length);
}

void
object_vector_set(object_t *v, size_t index, object_t *element)
{
    assert(v->type == OBJECT_VECTOR);
    vector_set(v->u.vector, index, (void *)element);
}

char *
object_string_to_chars(object_t *string)
{
    assert(string->type == OBJECT_STRING);
    vector_t *content = OBJECT_STRING_CONTENT(string);
    size_t length = content->length;
    string_t *cs = string_new();
    for (size_t i = 0; i < length; i++) {
        object_t *c = vector_at(content, i);
        uint8_t bytes[8] = { 0 };
        size_t length;
        utf8_code_to_bytes(OBJECT_CHAR_VALUE(c), bytes, &length);
        for (size_t i = 0; i < length; i++) {
            string_push_back(cs, bytes[i]);
        }
    }
    return cs->data;
}

object_t *
object_get_type(OBJECT_T type_id)
{
    return &type_objects[type_id];
}