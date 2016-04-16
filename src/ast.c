#include "ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
ast_cons_dfs(ast_t *cons)
{
    assert(cons->type == AST_CONS);
    ast_dfs(AST_CONS_CAR(cons));
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

ast_t *
ast_bool_new(bool value)
{
    ast_t *b = calloc(1, sizeof(ast_t));
    b->type = AST_BOOL;
    AST_BOOL_VALUE(b) = value;
    return b;
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
    switch (x->type) {
        case AST_BOOL:
            ast_bool_dfs(x);
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
    }
}

void
ast_print(ast_t *x, FILE *out)
{
    switch (x->type) {
        case AST_BOOL:
            ast_bool_print(x, out);
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
    }
}
