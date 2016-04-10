#include "ast.h"
#include "base/utils.h"
#include "base/vector.h"
#include "compiler.h"
#include "env.h"
#include "object.h"
#include "op.h"

#include <assert.h>
#include <stdlib.h>

static void compiler_compile_any(compiler_t *, ast_t *);

static env_t *toplevel_env = NULL;

static void
compiler_emit(compiler_t *c, op_t *op)
{
    vector_push_back(c->code, op);
}

static void
compile_set(compiler_t *c, ast_t *args)
{
    assert(args->type == AST_CONS);
    ast_t *var = AST_CONS_1ST(args);
    ast_t *expr = AST_CONS_2ND(args);
    compiler_compile_any(c, expr);
    char *name = AST_ID_NAME(var);
    env_push_back(toplevel_env, name, NULL);
    compiler_emit(c, OP_NEW1(OP_GSET, name));
}

static void
compiler_compile_cons(compiler_t *c, ast_t *x)
{
    assert(x->type == AST_CONS);
    ast_t *op = AST_CONS_CAR(x);
    assert(op->type == AST_ID);
    char *name = AST_ID_NAME(op);
    if (utils_str_equal(name, "set")) {
        compile_set(c, AST_CONS_CDR(x));
    }
}

static void
compiler_compile_id(compiler_t *c, ast_t *id)
{
    assert(id->type == AST_ID);
    int x, y;
    bool is_found = env_position(toplevel_env, AST_ID_NAME(id), &x, &y);
    assert(is_found == true);
    compiler_emit(c, OP_NEW2(OP_REF, x, y));
}

static void
compiler_compile_prog(compiler_t *c, ast_t *prog)
{
    assert(prog->type == AST_PROG);
    ast_t *exprs = AST_PROG_EXPRS(prog);
    while (exprs != NULL) {
        compiler_compile_any(c, AST_CONS_CAR(exprs));
        exprs = AST_CONS_CDR(exprs);
        if (exprs != NULL)
            compiler_emit(c, OP_NEW0(OP_POP));
    }
    compiler_emit(c, OP_NEW0(OP_PRINT));
    compiler_emit(c, OP_NEW0(OP_HALT));
}

static void
compiler_compile_any(compiler_t *c, ast_t *x)
{
    switch (x->type) {
        case AST_CONS:
            compiler_compile_cons(c, x);
            break;
        case AST_ID:
            compiler_compile_id(c, x);
            break;
        case AST_INT:
            compiler_emit(c, OP_NEW1(OP_PUSH, object_int_new(AST_INT_VALUE(x))));
            break;
        case AST_PROG:
            compiler_compile_prog(c, x);
            break;
        default :
            exit(EXIT_FAILURE);
    }
}

compiler_t *
compiler_new(void)
{
    compiler_t *c = calloc(1, sizeof(compiler_t));
    c->code = vector_new();
    return c;
}

void
compiler_compile(compiler_t *c, ast_t *prog)
{
    assert(prog->type == AST_PROG);
    compiler_compile_any(c, prog);
}

void
compiler_init(void)
{
    toplevel_env = env_new(NULL);
}
