#include "ast.h"
#include "base/utils.h"
#include "base/vector.h"
#include "compiler.h"
#include "env.h"
#include "object.h"
#include "op.h"

#include <assert.h>
#include <stdlib.h>

typedef struct {
    const char *name;
    OP_T op;
} bif_t;

static void compiler_compile_any(code_t *, ast_t *, env_t *);

static bif_t bif[] = {
        { .name = "+", .op = OP_ADD },
        { .name = "-", .op = OP_SUB },
        { .name = "*", .op = OP_MUL },
        { .name = "/", .op = OP_DIV },
        { .name = "=", .op = OP_EQL },
        { .name = "code-char", .op = OP_CODE_CHAR },
};
static int toplevel_var_count = 0;
static env_t *toplevel_env = NULL;
static code_t *toplevel_code = NULL;
static code_t *toplevel_defs = NULL;

static bool
is_bif(const char *name)
{
    for (size_t i = 0; i < sizeof(bif) / sizeof(*bif); i++) {
        if (utils_str_equal(name, bif[i].name))
            return true;
    }
    return false;
}

static void
compile_args(code_t *s, ast_t *args, env_t *env)
{
    while (args != NULL) {
        ast_t *expr = AST_CONS_CAR(args);
        compiler_compile_any(s, expr, env);
        args = AST_CONS_CDR(args);
    }
}

static void
compile_bif(code_t *s, const char *name, ast_t *args, env_t *env)
{
    assert(args->type == AST_CONS);
    // 编译实参列表
    compile_args(s, args, env);
    for (size_t i = 0; i < sizeof(bif) / sizeof(*bif); i++) {
        if (utils_str_equal(name, bif[i].name)) {
            emit(s, OP_NEW0(bif[i].op));
            return;
        }
    }
    assert(false);
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
    int x, y;
    assert(env_position(toplevel_env, name, &x, &y) == true);
    // 编译函数体
    compile_sequence(toplevel_defs, body, nenv);
    // 写入返回指令
    emit(toplevel_defs, OP_NEW0(OP_RET));
    // 创建函数对象
    emit(s, OP_NEW1(OP_FUN, name));
    // 绑定到顶层环境
    emit(s, OP_NEW2(OP_GSET, x, y));
    toplevel_var_count++;
    emit(s, OP_NEW0(OP_POP));
    emit(s, OP_NEW0(OP_NIL));
}

static void
compile_funcall(code_t *s, ast_t *x, env_t *env)
{
    ast_t *fun = AST_CONS_CAR(x);
    ast_t *args = AST_CONS_CDR(x);
    // 编译实参列表
    compile_args(s, args, env);
    // 编译函数
    compiler_compile_any(s, fun, env);
    emit(s, OP_NEW0(OP_CALL));
}

static void
compile_if(code_t *s, ast_t *args, env_t *env)
{
    ast_t *expr_test = AST_CONS_1ST(args);
    ast_t *expr_then = AST_CONS_2ND(args);
    ast_t *expr_else = AST_CONS_3ND(args);
    compiler_compile_any(s, expr_test, env);
    op_t *label_end = op_next_label("end");
    op_t *label_then = op_next_label("then");
    emit(s, OP_NEW1(OP_TJUMP, OP_LABEL_NAME(label_then)));
    compiler_compile_any(s, expr_else, env);
    emit(s, OP_NEW1(OP_JUMP, OP_LABEL_NAME(label_end)));
    emit(s, label_then);
    compiler_compile_any(s, expr_then, env);
    emit(s, label_end);
}

static void
compile_set(code_t *s, ast_t *args, env_t *env)
{
    assert(args->type == AST_CONS);
    ast_t *var = AST_CONS_1ST(args);
    ast_t *expr = AST_CONS_2ND(args);
    compiler_compile_any(s, expr, env);
    char *name = AST_ID_NAME(var);
    int x, y;
    if (env_position(toplevel_env, name, &x, &y) == false) {
        env_push_back(toplevel_env, name, NULL);
        assert(env_position(toplevel_env, name, &x, &y) == true);
    }
    emit(s, OP_NEW2(OP_GSET, x, y));
    toplevel_var_count++;
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
    } else if (utils_str_equal(name, "if")) {
        compile_if(s, AST_CONS_CDR(x), env);
    } else if (is_bif(name)) {
        compile_bif(s, name, AST_CONS_CDR(x), env);
    } else {
        compile_funcall(s, x, env);
    }
}

static void
compiler_compile_id(code_t *s, ast_t *id, env_t *env)
{
    assert(id->type == AST_ID);
    int x, y;
    bool is_found = env_position(env, AST_ID_NAME(id), &x, &y);
    if (is_found) {
        emit(s, OP_NEW2(OP_REF, x, y));
    } else {
        assert(env_position(toplevel_env, AST_ID_NAME(id), &x, &y) == true);
        emit(s, OP_NEW2(OP_GREF, x, y));
    }
}

static void
compiler_compile_prog(ast_t *prog, env_t *env)
{
    assert(prog->type == AST_PROG);
    ast_t *exprs = AST_PROG_EXPRS(prog);
    emit(toplevel_code, OP_NEW1(OP_LABEL, "_start"));
    compile_sequence(toplevel_code, exprs, env);
    emit(toplevel_code, OP_NEW0(OP_PRINT));
    emit(toplevel_code, OP_NEW0(OP_HALT));
}

static void
compiler_compile_any(code_t *s, ast_t *x, env_t *env)
{
    switch (x->type) {
        case AST_BOOL:
            emit(s, OP_NEW1(OP_PUSH, object_bool_new(AST_BOOL_VALUE(x))));
            break;
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
    ast_dfs(prog);
    puts("");
    // 声明所有的defun中的函数名，支持向前引用
    vector_t *v = ast_find_cons("defun");
    if (v != NULL) {
        for (size_t i = 0; i < v->length; i++) {
            ast_t *d = vector_at(v, i);
            ast_t *fun = AST_CONS_2ND(d);
            assert(fun->type == AST_ID);
            const char *name = AST_ID_NAME(fun);
            env_push_back(toplevel_env, name, NULL);
        }
    }
    compiler_compile_prog(prog, NULL);
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
    vector_t *program = code_new();
    emit(program, OP_NEW1(OP_GENV, toplevel_var_count));
    program = code_concat(program, toplevel_code);
    program = code_concat(program, toplevel_defs);
    return program;
}
