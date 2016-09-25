/**
 * 定义目标语言中的一等对象的实现
 */
#pragma once

#include "base/hash.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    OBJECT_BOOL,
    OBJECT_CHAR,
    OBJECT_FUN,
    OBJECT_INT,
    OBJECT_MAP,
    OBJECT_NIL,
    OBJECT_SYMBOL,
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
    } u;
} object_t;

#define OBJECT_BOOL_VALUE(x) ((x)->u.bool_value)
#define OBJECT_CHAR_VALUE(x) ((x)->u.char_value)
#define OBJECT_FUN_TYPE(x) ((x)->u.fun.type)
#define OBJECT_FUN_NATIVE_ARITY(x) ((x)->u.fun.u.native.arity)
#define OBJECT_FUN_NATIVE_IMPL(x) ((x)->u.fun.u.native.impl)
#define OBJECT_FUN_UDF_ENTRY(x) ((x)->u.fun.u.udf.entry)
#define OBJECT_INT_VALUE(x) ((x)->u.integer)
#define OBJECT_SYMBOL_NAME(x) ((x)->u.symbol)

extern object_t *object_bool_new(bool);
extern object_t *object_char_new(uint32_t);
extern object_t *object_fun_native_new(int, void *);
extern object_t *object_fun_udf_new(int);
extern object_t *object_int_new(int);
extern object_t *object_map_new(void);
extern object_t *object_nil_new(void);
extern object_t *object_symbol_new(const char *);
extern object_t *object_symbol_intern(const char *);
extern void object_print(object_t *);
