/**
 * 定义目标语言中的一等对象的实现
 */
#pragma once

#include "base/hash.h"
#include "base/vector.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    OBJECT_BOOL,
    OBJECT_CHAR,
    OBJECT_FUN,
    OBJECT_INT,
    OBJECT_MAP,
    OBJECT_NIL,
    OBJECT_STRING,
    OBJECT_SYMBOL,
    OBJECT_TYPE,
    OBJECT_VECTOR,
} OBJECT_T;

typedef enum {
    FUN_NATIVE,
    FUN_UDF,
} FUN_T;

typedef struct {
    OBJECT_T type;
    union {
        bool bool_value;
        int integer;
        uint32_t char_value;
        struct {
            FUN_T type;
            union {
                struct {
                    int arity;
                    void *impl;
                } native;
                struct {
                    int entry;
                } udf;
            } u;
        } fun;
        hash_table_t *map;
        char *symbol;
        vector_t *vector;
        vector_t *content;
        struct {
            OBJECT_T type_id;
            char *name;
        } type;
    } u;
} object_t;

#define OBJECT_BOOL_VALUE(x) ((x)->u.bool_value)
#define OBJECT_CHAR_VALUE(x) ((x)->u.char_value)
#define OBJECT_FUN_TYPE(x) ((x)->u.fun.type)
#define OBJECT_FUN_NATIVE_ARITY(x) ((x)->u.fun.u.native.arity)
#define OBJECT_FUN_NATIVE_IMPL(x) ((x)->u.fun.u.native.impl)
#define OBJECT_FUN_UDF_ENTRY(x) ((x)->u.fun.u.udf.entry)
#define OBJECT_INT_VALUE(x) ((x)->u.integer)
#define OBJECT_STRING_CONTENT(x) ((x)->u.content)
#define OBJECT_SYMBOL_NAME(x) ((x)->u.symbol)
#define OBJECT_TYPE_ID(x) ((x)->u.type.type_id)
#define OBJECT_TYPE_NAME(x) ((x)->u.type.name)

extern object_t *object_bool_new(bool);
extern object_t *object_char_new(uint32_t);
extern object_t *object_fun_native_new(int, void *);
extern object_t *object_fun_udf_new(int);
extern object_t *object_int_new(int);
extern object_t *object_map_new(void);
extern object_t *object_nil_new(void);
extern object_t *object_string_new(vector_t *);
extern object_t *object_symbol_new(const char *);
extern object_t *object_symbol_intern(const char *);
extern object_t *object_type_new(const OBJECT_T, const char *);
extern object_t *object_vector_new(void);
extern void object_print(object_t *);
extern void object_vector_reserve(object_t *, size_t);
extern void object_vector_set(object_t *, size_t, object_t *);
extern char *object_string_to_chars(object_t *);
extern object_t *object_get_type(OBJECT_T type_id);
