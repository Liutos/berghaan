/**
 * 定义抽象语法树类型
 */
#pragma once

#include <stdio.h>

typedef enum {
    AST_CONS,
    AST_ID,
} AST_TYPE_T;

typedef struct _ast_t {
    AST_TYPE_T type;
    union {
        char *id;
        struct {
            struct _ast_t *car;
            struct _ast_t *cdr;
        } cons;
    } u;
} ast_t;

#define AST_CONS_CAR(x) ((x)->u.cons.car)
#define AST_CONS_CDR(x) ((x)->u.cons.cdr)
#define AST_ID_NAME(x) ((x)->u.id)

extern ast_t *ast_cons_new(ast_t *, ast_t *);
extern ast_t *ast_id_new(const char *);
extern void ast_print(ast_t *, FILE *);
