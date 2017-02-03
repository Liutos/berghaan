/**
 * 定义抽象语法树类型
 */
#pragma once

#include "base/vector.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
    AST_BOOL,
    AST_CHAR,
    AST_CONS,
    AST_ID,
    AST_INT,
    AST_PROG,
    AST_SHARP_V,
    AST_SYMBOL,
} AST_TYPE_T;

typedef struct _ast_t {
    AST_TYPE_T type;
    union {
        char *id;
        int integer;
        bool bool_value;
        struct {
            struct _ast_t *operator;
            struct _ast_t *args;
        } call;
        struct {
            struct _ast_t *car;
            struct _ast_t *cdr;
        } cons;
        struct {
            struct _ast_t *exprs;
        } prog;
        char *symbol;
        uint32_t char_value;
        struct {
            struct _ast_t *elements;
        } sharp_v;
    } u;
} ast_t;

#define AST_BOOL_VALUE(x) ((x)->u.bool_value)
#define AST_CHAR_VALUE(x) ((x)->u.char_value)
#define AST_CONS_1ST(x) AST_CONS_CAR(x)
#define AST_CONS_2ND(x) AST_CONS_CAR(AST_CONS_CDR(x))
#define AST_CONS_3ND(x) AST_CONS_CAR(AST_CONS_CDR(AST_CONS_CDR(x)))
#define AST_CONS_CAR(x) ((x)->u.cons.car)
#define AST_CONS_CDR(x) ((x)->u.cons.cdr)
#define AST_ID_NAME(x) ((x)->u.id)
#define AST_INT_VALUE(x) ((x)->u.integer)
#define AST_PROG_EXPRS(x) ((x)->u.prog.exprs)
#define AST_SHARP_V_ELEMENTS(x) ((x)->u.sharp_v.elements)
#define AST_SYMBOL_NAME(x) ((x)->u.symbol)

extern ast_t *ast_bool_new(bool);
extern ast_t *ast_char_new(uint32_t);
extern ast_t *ast_cons_map1(ast_t *);
extern ast_t *ast_cons_map2(ast_t *);
extern ast_t *ast_cons_new(ast_t *, ast_t *);
extern ast_t *ast_id_new(const char *);
extern ast_t *ast_int_new(int);
extern ast_t *ast_prog_new(ast_t *);
extern ast_t *ast_sharp_v_new(ast_t *);
extern ast_t *ast_symbol_new(const char *);
extern int ast_cons_length(ast_t *);
extern void ast_dfs(ast_t *);
extern void ast_print(ast_t *, FILE *);
extern vector_t *ast_find_cons(const char *);
