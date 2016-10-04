#include "ast.h"
#include "base/list.h"
#include "base/utils.h"
#include "base/utf8.h"
#include "base/vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static assoc_list_t *sf = NULL;

static void
ast_bool_dfs(ast_t *b)
{
    assert(b->type == AST_BOOL);
    printf("%s", AST_BOOL_VALUE(b) ? "true" : "false");
}

static void
ast_bool_print(ast_t *b, FILE *out)
{
    assert(b->type == AST_BOOL);
    fprintf(out, "%s", AST_BOOL_VALUE(b) ? "true" : "false");
}

static void
ast_char_print(ast_t *c, FILE *out)
{
    assert(c->type == AST_CHAR);
    fprintf(out, "'");
    utf8_fprintf(out, AST_CHAR_VALUE(c));
    fprintf(out, "'");
}

static void
ast_char_dfs(ast_t *c)
{
    ast_char_print(c, stdout);
}

static void
ast_cons_dfs(ast_t *cons)
{
    assert(cons->type == AST_CONS);
    ast_t *car = AST_CONS_CAR(cons);
    if (car->type == AST_ID && utils_str_equal("defun", AST_ID_NAME(car))) {
        vector_t *v = assoc_list_search(sf, AST_ID_NAME(car));
        if (v == NULL) {
            v = vector_new();
            assoc_list_push(sf, AST_ID_NAME(car), v);
        }
        vector_push_back(v, cons);
    }
    ast_dfs(car);
    if (AST_CONS_CDR(cons) != NULL)
        ast_dfs(AST_CONS_CDR(cons));
}

static void
ast_cons_print(ast_t *cons, FILE *out)
{
    assert(cons->type == AST_CONS);
    fprintf(out, "(");
    while (cons != NULL) {
        ast_print(AST_CONS_CAR(cons), out);
        cons = AST_CONS_CDR(cons);
        if (cons != NULL)
            fprintf(out, " ");
    }
    fprintf(out, ")");
}

static void
ast_id_dfs(ast_t *id)
{
    assert(id->type == AST_ID);
    printf("%s", AST_ID_NAME(id));
}

static void
ast_id_print(ast_t *id, FILE *out)
{
    assert(id->type == AST_ID);
    fprintf(out, "%s", AST_ID_NAME(id));
}

static void
ast_int_dfs(ast_t *n)
{
    assert(n->type == AST_INT);
    printf("%d", AST_INT_VALUE(n));
}

static void
ast_int_print(ast_t *n, FILE *out)
{
    assert(n->type == AST_INT);
    fprintf(out, "%d", AST_INT_VALUE(n));
}

static void
ast_prog_dfs(ast_t *p)
{
    assert(p->type == AST_PROG);
    ast_dfs(AST_PROG_EXPRS(p));
}

static void
ast_prog_print(ast_t *p, FILE *out)
{
    assert(p->type == AST_PROG);
    ast_print(AST_PROG_EXPRS(p), out);
}

static void
ast_symbol_print(ast_t *s, FILE *out)
{
    assert(s->type == AST_SYMBOL);
    fprintf(out, ":%s", AST_SYMBOL_NAME(s));
}

static void
ast_symbol_dfs(ast_t *s)
{
    ast_symbol_print(s, stdout);
}

static ast_t *
ast_new(AST_TYPE_T type)
{
    ast_t *a = calloc(1, sizeof(ast_t));
    a->type = type;
    return a;
}

ast_t *
ast_bool_new(bool value)
{
    ast_t *b = calloc(1, sizeof(ast_t));
    b->type = AST_BOOL;
    AST_BOOL_VALUE(b) = value;
    return b;
}

ast_t *
ast_char_new(uint32_t value)
{
    ast_t *c = ast_new(AST_CHAR);
    AST_CHAR_VALUE(c) = value;
    return c;
}

ast_t *
ast_cons_map1(ast_t *cons)
{
    if (cons == NULL) {
        return NULL;
    } else {
        assert(cons->type == AST_CONS);
        ast_t *car = AST_CONS_CAR(cons);
        assert(car->type == AST_CONS);
        return ast_cons_new(AST_CONS_1ST(car), ast_cons_map1(AST_CONS_CDR(cons)));
    }
}

ast_t *
ast_cons_map2(ast_t *cons)
{
    if (cons == NULL) {
        return NULL;
    } else {
        assert(cons->type == AST_CONS);
        ast_t *car = AST_CONS_CAR(cons);
        assert(car->type == AST_CONS);
        return ast_cons_new(AST_CONS_2ND(car), ast_cons_map2(AST_CONS_CDR(cons)));
    }
}

ast_t *
ast_cons_new(ast_t *car, ast_t *cdr)
{
    ast_t *cons = calloc(1, sizeof(ast_t));
    cons->type = AST_CONS;
    AST_CONS_CAR(cons) = car;
    AST_CONS_CDR(cons) = cdr;
    return cons;
}

ast_t *
ast_id_new(const char *name)
{
    ast_t *id = calloc(1, sizeof(ast_t));
    id->type = AST_ID;
    AST_ID_NAME(id) = strdup(name);
    return id;
}

ast_t *
ast_int_new(int value)
{
    ast_t *n = calloc(1, sizeof(ast_t));
    n->type = AST_INT;
    AST_INT_VALUE(n) = value;
    return n;
}

ast_t *
ast_prog_new(ast_t *exprs)
{
    ast_t *p = calloc(1, sizeof(ast_t));
    p->type = AST_PROG;
    AST_PROG_EXPRS(p) = exprs;
    return p;
}

ast_t *
ast_symbol_new(const char *name)
{
    ast_t *s = ast_new(AST_SYMBOL);
    AST_SYMBOL_NAME(s) = strdup(name);
    return s;
}

int
ast_cons_length(ast_t *cons)
{
    assert(cons->type == AST_CONS);
    int length = 0;
    while (cons != NULL) {
        length += 1;
        cons = AST_CONS_CDR(cons);
    }
    return length;
}

void
ast_dfs(ast_t *x)
{
    if (sf == NULL)
        sf = assoc_list_new();
    switch (x->type) {
        case AST_BOOL:
            ast_bool_dfs(x);
            break;
        case AST_CHAR:
            ast_char_dfs(x);
            break;
        case AST_CONS:
            ast_cons_dfs(x);
            break;
        case AST_ID:
            ast_id_dfs(x);
            break;
        case AST_INT:
            ast_int_dfs(x);
            break;
        case AST_PROG:
            ast_prog_dfs(x);
            break;
        case AST_SYMBOL:
            ast_symbol_dfs(x);
            break;
    }
}

void
ast_print(ast_t *x, FILE *out)
{
    switch (x->type) {
        case AST_BOOL:
            ast_bool_print(x, out);
            break;
        case AST_CHAR:
            ast_char_print(x, out);
            break;
        case AST_CONS:
            ast_cons_print(x, out);
            break;
        case AST_ID:
            ast_id_print(x, out);
            break;
        case AST_INT:
            ast_int_print(x, out);
            break;
        case AST_PROG:
            ast_prog_print(x, out);
            break;
        case AST_SYMBOL:
            ast_symbol_print(x, out);
            break;
    }
}

vector_t *
ast_find_cons(const char *name)
{
    assert(sf != NULL);
    return (vector_t *)assoc_list_search(sf, name);
}
