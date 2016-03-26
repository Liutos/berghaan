/**
 * 定义目标语言中的一等对象的实现
 */
#pragma once

#include <stdbool.h>

typedef enum {
    OBJECT_BOOL,
    OBJECT_INT,
} OBJECT_T;

typedef struct {
    OBJECT_T type;
    union {
        bool bool_value;
        int integer;
    } u;
} object_t;

#define OBJECT_BOOL_VALUE(x) ((x)->u.bool_value)
#define OBJECT_INT_VALUE(x) ((x)->u.integer)

extern object_t *object_bool_new(bool);
extern object_t *object_int_new(int);
extern void object_print(object_t *);
