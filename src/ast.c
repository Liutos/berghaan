#include "ast.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
ast_id_print(ast_t *id, FILE *out)
{
    assert(id->type == AST_ID);
    fprintf(out, "%s", AST_ID_NAME(id));
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

void
ast_print(ast_t *x, FILE *out)
{
    switch (x->type) {
        case AST_CONS:
            ast_cons_print(x, out);
            break;
        case AST_ID:
            ast_id_print(x, out);
            break;
    }
}
