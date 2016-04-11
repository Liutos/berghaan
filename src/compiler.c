#include "ast.h"
#include "base/utils.h"
#include "base/vector.h"
#include "compiler.h"
#include "env.h"
#include "object.h"
#include "op.h"

#include <assert.h>
#include <stdlib.h>

#define code_new vector_new

static void compiler_compile_any(code_t *, ast_t *, env_t *);

static env_t *toplevel_env = NULL;
static code_t *toplevel_code = NULL;
static code_t *toplevel_defs = NULL;

static void
emit(code_t *s, op_t *op)
{
    vector_push_back(s, op);
}

static void
compile_sequence(code_t *s, ast_t *body, env_t *env)
{
    assert(body->type == AST_CONS);
    while (body != NULL) {
        compiler_compile_any(s, AST_CONS_CAR(body), env);
        body = AST_CONS_CDR(body);
        if (body != NULL)
            emit(s, OP_NEW0(OP_POP));
    }
}

static void
compile_defun(code_t *s, ast_t *args, env_t *env)
{
    ast_t *fun = AST_CONS_1ST(args);
    ast_t *parameters = AST_CONS_2ND(args);
    ast_t *body = AST_CONS_CDR(AST_CONS_CDR(args));
    // 写入标号
    char *name = AST_ID_NAME(fun);
    emit(toplevel_defs, OP_NEW1(OP_LABEL, name));
    // 写入传参指令
    int arity = ast_cons_length(parameters);
    emit(toplevel_defs, OP_NEW1(OP_ARG, arity));
    // 扩展环境
    env_t *nenv = env_new(env);
    while (parameters != NULL) {
        ast_t *par = AST_CONS_CAR(parameters);
        env_bind(nenv, AST_ID_NAME(par), NULL);
        parameters = AST_CONS_CDR(parameters);
    }
    // 编译函数体
    compile_sequence(toplevel_defs, body, nenv);
    // 写入返回指令
    emit(toplevel_defs, OP_NEW0(OP_RET));
    // 创建函数对象
    emit(s, OP_NEW1(OP_FUN, name));
    // 绑定到顶层环境
    emit(s, OP_NEW1(OP_GSET, name));
    emit(s, OP_NEW0(OP_POP));
    emit(s, OP_NEW0(OP_NIL));
}

static void
compile_set(code_t *s, ast_t *args, env_t *env)
{
    assert(args->type == AST_CONS);
    ast_t *var = AST_CONS_1ST(args);
    ast_t *expr = AST_CONS_2ND(args);
    compiler_compile_any(s, expr, env);
    char *name = AST_ID_NAME(var);
    env_push_back(toplevel_env, name, NULL);
    emit(s, OP_NEW1(OP_GSET, name));
}

static void
compiler_compile_cons(code_t *s, ast_t *x, env_t *env)
{
    assert(x->type == AST_CONS);
    ast_t *op = AST_CONS_CAR(x);
    assert(op->type == AST_ID);
    char *name = AST_ID_NAME(op);
    if (utils_str_equal(name, "set")) {
        compile_set(s, AST_CONS_CDR(x), env);
    } else if (utils_str_equal(name, "defun")) {
        compile_defun(s, AST_CONS_CDR(x), env);
    }
}

static void
compiler_compile_id(code_t *s, ast_t *id, env_t *env)
{
    assert(id->type == AST_ID);
    int x, y;
    bool is_found = env_position(env, AST_ID_NAME(id), &x, &y);
    assert(is_found == true);
    emit(s, OP_NEW2(OP_REF, x, y));
}

static void
compiler_compile_prog(ast_t *prog, env_t *env)
{
    assert(prog->type == AST_PROG);
    ast_t *exprs = AST_PROG_EXPRS(prog);
    compile_sequence(toplevel_code, exprs, env);
    emit(toplevel_code, OP_NEW0(OP_PRINT));
    emit(toplevel_code, OP_NEW0(OP_HALT));
}

static void
compiler_compile_any(code_t *s, ast_t *x, env_t *env)
{
    switch (x->type) {
        case AST_CONS:
            compiler_compile_cons(s, x, env);
            break;
        case AST_ID:
            compiler_compile_id(s, x, env);
            break;
        case AST_INT:
            emit(s, OP_NEW1(OP_PUSH, object_int_new(AST_INT_VALUE(x))));
            break;
        case AST_PROG:
            compiler_compile_prog(x, env);
            break;
        default :
            exit(EXIT_FAILURE);
    }
}

void
compiler_compile(ast_t *prog)
{
    assert(prog->type == AST_PROG);
    compiler_compile_prog(prog, toplevel_env);
}

void
compiler_init(void)
{
    toplevel_code = code_new();
    toplevel_defs = code_new();
    toplevel_env = env_new(NULL);
}

code_t *
compiler_done(void)
{
    // 将函数定义追加到运行代码中
    for (size_t i = 0; i < toplevel_defs->length; i++) {
        emit(toplevel_code, (op_t *)vector_at(toplevel_defs, i));
    }
    for (size_t i = 0; i < toplevel_code->length; i++) {
        op_print((op_t *)vector_at(toplevel_code, i), stdout);
    }
    return toplevel_code;
}
