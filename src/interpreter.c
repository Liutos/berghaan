#include "ast.h"
#include "interpreter.h"
#include "base/list.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _env_t {
    assoc_list_t *bindings;
    struct _env_t *outer;
} env_t;

static env_t *toplevel_env = NULL;

static env_t *
env_new(void)
{
    env_t *env = calloc(1, sizeof(env_t));
    env->bindings = assoc_list_new();
    env->outer = NULL;
    return env;
}

static env_t *
env_bind(env_t *env, const char *variable, object_t *value)
{
    env->bindings = assoc_list_push(env->bindings, variable, value);
    return env;
}

static object_t *
env_reference(env_t *env, const char *variable)
{
    while (env != NULL) {
        object_t *value = assoc_list_search(env->bindings, variable);
        if (value != NULL)
            return value;
        env = env->outer;
    }
    return NULL;
}

static object_t *
interpret_equal(ast_t *x)
{
    int length = ast_cons_length(x);
    assert(length == 2);
    ast_t *left = AST_CONS_1ST(x);
    ast_t *right = AST_CONS_2ND(x);
    object_t *lhs = interpret(left);
    object_t *rhs = interpret(right);
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_bool_new(OBJECT_INT_VALUE(lhs) == OBJECT_INT_VALUE(rhs));
}

static object_t *
interpret_if(ast_t *x)
{
    int length = ast_cons_length(x);
    assert(length == 3);
    ast_t *predicate = AST_CONS_1ST(x);
    ast_t *expr_then = AST_CONS_2ND(x);
    ast_t *expr_else = AST_CONS_3ND(x);
    object_t *result = interpret(predicate);
    assert(result->type == OBJECT_BOOL);
    if (OBJECT_BOOL_VALUE(result) == true) {
        return interpret(expr_then);
    } else {
        return interpret(expr_else);
    }
}

static object_t *
interpret_set(ast_t *x)
{
    int length = ast_cons_length(x);
    assert(length == 2);
    ast_t *variable = AST_CONS_1ST(x);
    ast_t *expr = AST_CONS_2ND(x);
    assert(variable->type == AST_ID);
    object_t *value = interpret(expr);
    env_bind(toplevel_env, AST_ID_NAME(variable), value);
    return value;
}

static object_t *
interpret_bool(ast_t *x)
{
    assert(x->type == AST_BOOL);
    return object_bool_new(AST_BOOL_VALUE(x));
}

static object_t *
interpret_call(ast_t *x)
{
    assert(x->type == AST_CALL);
    ast_t *operator = AST_CALL_OPERATOR(x);
    assert(operator->type == AST_ID);
    char *name = AST_ID_NAME(operator);
    if (strcmp(name, "=") == 0)
        return interpret_equal(AST_CALL_ARGS(x));
    if (strcmp(name, "if") == 0)
        return interpret_if(AST_CALL_ARGS(x));
    if (strcmp(name, "set") == 0)
        return interpret_set(AST_CALL_ARGS(x));
    exit(EXIT_FAILURE);
}

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
interpret_id(ast_t *x)
{
    assert(x->type == AST_ID);
    return env_reference(toplevel_env, AST_ID_NAME(x));
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
        case AST_BOOL:
            return interpret_bool(x);
        case AST_CALL:
            return interpret_call(x);
        case AST_CONS:
            return interpret_cons(x);
        case AST_ID:
            return interpret_id(x);
        case AST_INT:
            return interpret_int(x);
        default :
            exit(EXIT_FAILURE);
    }
}

void
interpreter_init(void)
{
    toplevel_env = env_new();
}
