/**
 * 定义抽象语法树类型
 */
#pragma once

#include <stdio.h>

typedef enum {
    AST_CALL,
    AST_CONS,
    AST_ID,
    AST_INT,
} AST_TYPE_T;

typedef struct _ast_t {
    AST_TYPE_T type;
    union {
        char *id;
        int integer;
        struct {
            struct _ast_t *operator;
            struct _ast_t *args;
        } call;
        struct {
            struct _ast_t *car;
            struct _ast_t *cdr;
        } cons;
    } u;
} ast_t;

#define AST_CALL_OPERATOR(x) ((x)->u.call.operator)
#define AST_CALL_ARGS(x) ((x)->u.call.args)
#define AST_CONS_CAR(x) ((x)->u.cons.car)
#define AST_CONS_CDR(x) ((x)->u.cons.cdr)
#define AST_ID_NAME(x) ((x)->u.id)
#define AST_INT_VALUE(x) ((x)->u.integer)

extern ast_t *ast_call_new(ast_t *, ast_t *);
extern ast_t *ast_cons_new(ast_t *, ast_t *);
extern ast_t *ast_id_new(const char *);
extern ast_t *ast_int_new(int);
extern void ast_dfs(ast_t *);
extern void ast_print(ast_t *, FILE *);
