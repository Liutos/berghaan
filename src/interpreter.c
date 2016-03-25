#include "ast.h"
#include "interpreter.h"

#include <assert.h>
#include <stdlib.h>

static object_t *
interpret_cons(ast_t *x)
{
    assert(x->type == AST_CONS);
    object_t *result;
    while (x != NULL) {
        result = interpret(AST_CONS_CAR(x));
        x = AST_CONS_CDR(x);
    }
    return result;
}

static object_t *
interpret_int(ast_t *x)
{
    assert(x->type == AST_INT);
    return object_int_new(AST_INT_VALUE(x));
}

object_t *
interpret(ast_t *x)
{
    switch (x->type) {
        case AST_CONS:
            return interpret_cons(x);
        case AST_INT:
            return interpret_int(x);
        default :
            exit(EXIT_FAILURE);
    }
}
