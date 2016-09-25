#include "base/utf8.h"
#include "object.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static hash_table_t *string_symbol_map = NULL;

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
//    printf("'%c'", OBJECT_CHAR_VALUE(c));
    utf8_fprintf(stdout, OBJECT_CHAR_VALUE(c));
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
object_symbol_print(object_t *s)
{
    assert(s->type == OBJECT_SYMBOL);
    printf(":%s", OBJECT_SYMBOL_NAME(s));
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
        case OBJECT_SYMBOL:
            object_symbol_print(x);
            break;
    }
}
