/**
 * 定义目标语言中的一等对象的实现
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    OBJECT_BOOL,
    OBJECT_CHAR,
    OBJECT_INT,
    OBJECT_NIL,
} OBJECT_T;

typedef struct {
    OBJECT_T type;
    union {
        bool bool_value;
        int integer;
        uint32_t char_value;
    } u;
} object_t;

#define OBJECT_BOOL_VALUE(x) ((x)->u.bool_value)
#define OBJECT_CHAR_VALUE(x) ((x)->u.char_value)
#define OBJECT_INT_VALUE(x) ((x)->u.integer)

extern object_t *object_bool_new(bool);
extern object_t *object_char_new(uint32_t);
extern object_t *object_int_new(int);
extern object_t *object_nil_new(void);
extern void object_print(object_t *);
