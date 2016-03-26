#include "ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
ast_call_dfs(ast_t *call)
{
    assert(call->type == AST_CALL);
    ast_dfs(AST_CALL_OPERATOR(call));
    if (AST_CALL_ARGS(call) != NULL)
        ast_dfs(AST_CALL_ARGS(call));
}

static void
ast_call_print(ast_t *call, FILE *out)
{
    assert(call->type == AST_CALL);
    fprintf(out, "(");
    ast_print(AST_CALL_OPERATOR(call), out);
    if (AST_CALL_ARGS(call) != NULL) {
        fprintf(out, " . ");
        ast_print(AST_CALL_ARGS(call), out);
    }
    fprintf(out, ")");
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

ast_t *
ast_call_new(ast_t *operator, ast_t *args)
{
    ast_t *call = calloc(1, sizeof(ast_t));
    call->type = AST_CALL;
    AST_CALL_OPERATOR(call) = operator;
    AST_CALL_ARGS(call) = args;
    return call;
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
        case AST_CALL:
            ast_call_dfs(x);
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
    }
}

void
ast_print(ast_t *x, FILE *out)
{
    switch (x->type) {
        case AST_CALL:
            ast_call_print(x, out);
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
    }
}
