#include "ast.h"
#include "interpreter.h"
#include "base/hash.h"
#include "base/list.h"
#include "base/utils.h"
#include "env.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef object_t *(*native_0_t)(void);

typedef enum {
    FUNCTION_NATIVE,
    FUNCTION_UDF,
} FUNCTION_T;

typedef struct {
    FUNCTION_T type;
    union {
        struct {
            int arity;
            void *impl;
        } native;
        struct {
            ast_t *arguments;
            ast_t *body;
        } udf;
    } u;
} function_t;

#define FUNCTION_NATIVE_ARITY(x) ((x)->u.native.arity)
#define FUNCTION_NATIVE_IMPL(x) ((x)->u.native.impl)
#define FUNCTION_UDF_ARGUMENTS(x) ((x)->u.udf.arguments)
#define FUNCTION_UDF_BODY(x) ((x)->u.udf.body)

static env_t *toplevel_env = NULL;
static hash_table_t *toplevel_udf = NULL;

static object_t *interpret_any(ast_t *, env_t *);

static object_t *
interpret_code_char(ast_t *x, env_t *env)
{
    int length = ast_cons_length(x);
    assert(length == 1);
    ast_t *expr_n = AST_CONS_1ST(x);
    object_t *n = interpret_any(expr_n, env);
    uint32_t value = OBJECT_INT_VALUE(n);
    return object_char_new(value);
}

static object_t *
interpret_defun(ast_t *x)
{
    int length = ast_cons_length(x);
    assert(length >= 3);
    ast_t *name = AST_CONS_1ST(x);
    assert(name->type == AST_ID);
    ast_t *args = AST_CONS_2ND(x);
    assert(args->type == AST_CONS);
    ast_t *body = AST_CONS_CDR(AST_CONS_CDR(x));
    assert(body->type == AST_CONS);
    function_t *f = calloc(1, sizeof(function_t));
    f->type = FUNCTION_UDF;
    FUNCTION_UDF_ARGUMENTS(f) = args;
    FUNCTION_UDF_BODY(f) = body;
    hash_table_set(toplevel_udf, AST_ID_NAME(name), f);
    return object_nil_new();
}

static object_t *
interpret_equal(ast_t *x, env_t *env)
{
    int length = ast_cons_length(x);
    assert(length == 2);
    ast_t *left = AST_CONS_1ST(x);
    ast_t *right = AST_CONS_2ND(x);
    object_t *lhs = interpret_any(left, env);
    object_t *rhs = interpret_any(right, env);
    assert(lhs->type == OBJECT_INT);
    assert(rhs->type == OBJECT_INT);
    return object_bool_new(OBJECT_INT_VALUE(lhs) == OBJECT_INT_VALUE(rhs));
}

static object_t *
interpret_if(ast_t *x, env_t *env)
{
    int length = ast_cons_length(x);
    assert(length == 3);
    ast_t *predicate = AST_CONS_1ST(x);
    ast_t *expr_then = AST_CONS_2ND(x);
    ast_t *expr_else = AST_CONS_3ND(x);
    object_t *result = interpret_any(predicate, env);
    assert(result->type == OBJECT_BOOL);
    if (OBJECT_BOOL_VALUE(result) == true) {
        return interpret_any(expr_then, env);
    } else {
        return interpret_any(expr_else, env);
    }
}

static object_t *
interpret_native(function_t *f, ast_t *x __attribute__ ((unused)), env_t *env __attribute__ ((unused)))
{
    object_t *value = NULL;
    switch (FUNCTION_NATIVE_ARITY(f)) {
        case 0:
            value = ((native_0_t)FUNCTION_NATIVE_IMPL(f))();
            break;
        default :
            exit(EXIT_FAILURE);
    }
    return value;
}

static object_t *
interpret_sequence(ast_t *x, env_t *env)
{
    assert(x->type == AST_CONS);
    object_t *result = NULL;
    while (x != NULL) {
        result = interpret_any(AST_CONS_CAR(x), env);
        x = AST_CONS_CDR(x);
    }
    return result;
}

static object_t *
interpret_set(ast_t *x, env_t *env)
{
    int length = ast_cons_length(x);
    assert(length == 2);
    ast_t *variable = AST_CONS_1ST(x);
    ast_t *expr = AST_CONS_2ND(x);
    assert(variable->type == AST_ID);
    object_t *value = interpret_any(expr, env);
    env_bind(toplevel_env, AST_ID_NAME(variable), value);
    return value;
}

static object_t *
interpret_udf(function_t *f, ast_t *x, env_t *env)
{
    ast_t *args = FUNCTION_UDF_ARGUMENTS(f);
    int arity = ast_cons_length(args);
    int length = ast_cons_length(x);
    assert(length == arity);
    env_t *e = env_new(toplevel_env);
    while (args != NULL) {
        ast_t *expr = AST_CONS_1ST(x);
        object_t *v = interpret_any(expr, env);
        ast_t *arg = AST_CONS_CAR(args);
        assert(arg->type == AST_ID);
        e = env_bind(e, AST_ID_NAME(arg), v);
        args = AST_CONS_CDR(args);
        x = AST_CONS_CDR(x);
    }
    object_t *result = interpret_sequence(FUNCTION_UDF_BODY(f), e);
    return result;
}

static object_t *
interpret_bool(ast_t *x)
{
    assert(x->type == AST_BOOL);
    return object_bool_new(AST_BOOL_VALUE(x));
}

static object_t *
interpret_cons(ast_t *x, env_t *env)
{
    assert(x->type == AST_CONS);
    ast_t *operator = AST_CONS_1ST(x);
    assert(operator->type == AST_ID);
    char *name = AST_ID_NAME(operator);
    if (utils_str_equal(name, "="))
        return interpret_equal(AST_CONS_CDR(x), env);
    if (utils_str_equal(name, "code-char"))
        return interpret_code_char(AST_CONS_CDR(x), env);
    if (utils_str_equal(name, "defun"))
        return interpret_defun(AST_CONS_CDR(x));
    if (utils_str_equal(name, "if"))
        return interpret_if(AST_CONS_CDR(x), env);
    if (utils_str_equal(name, "set"))
        return interpret_set(AST_CONS_CDR(x), env);
    function_t *f = hash_table_get(toplevel_udf, name);
    if (f == NULL)
        exit(EXIT_FAILURE);
    if (f->type == FUNCTION_NATIVE)
        return interpret_native(f, AST_CONS_CDR(x), env);
    if (f->type == FUNCTION_UDF)
        return interpret_udf(f, AST_CONS_CDR(x), env);
    exit(EXIT_FAILURE);
}

static object_t *
interpret_id(ast_t *x, env_t *env)
{
    assert(x->type == AST_ID);
    return env_reference(env, AST_ID_NAME(x));
}

static object_t *
interpret_int(ast_t *x)
{
    assert(x->type == AST_INT);
    return object_int_new(AST_INT_VALUE(x));
}

static object_t *
interpret_prog(ast_t *x, env_t *env)
{
    assert(x->type == AST_PROG);
    ast_t *exprs = AST_PROG_EXPRS(x);
    object_t *result = interpret_sequence(exprs, env);
    return result;
}

static object_t *
interpret_any(ast_t *x, env_t *env)
{
    switch (x->type) {
        case AST_BOOL:
            return interpret_bool(x);
        case AST_CONS:
            return interpret_cons(x, env);
        case AST_ID:
            return interpret_id(x, env);
        case AST_INT:
            return interpret_int(x);
        case AST_PROG:
            return interpret_prog(x, env);
        default :
            exit(EXIT_FAILURE);
    }
}

static object_t *
native_yes(void)
{
    puts("yes");
    return object_nil_new();
}

object_t *
interpret(ast_t *x)
{
    return interpret_any(x, toplevel_env);
}

void
interpreter_init(void)
{
    toplevel_env = env_new(NULL);
    toplevel_udf = hash_table_new(hash_string_equal, hash_string_hash);
    function_t *f = calloc(1, sizeof(function_t));
    f->type = FUNCTION_NATIVE;
    FUNCTION_NATIVE_ARITY(f) = 0;
    FUNCTION_NATIVE_IMPL(f) = (void *)native_yes;
    hash_table_set(toplevel_udf, "yes", f);
}
