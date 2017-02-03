#include "ast.h"
#include "base/utils.h"
#include "base/vector.h"
#include "compiler.h"
#include "env.h"
#include "object.h"
#include "op.h"

#include <assert.h>
#include <stdlib.h>

static void compiler_compile_any(code_t *, ast_t *, env_t *);

env_t *toplevel_env = NULL;

static int toplevel_var_count = 0;
static code_t *toplevel_code = NULL;
static code_t *toplevel_defs = NULL;

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
compile_global_function(const char *name, ast_t *parameters, ast_t *body, env_t *env)
{
    // 写入标号
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
}

static void
compile_defun(code_t *s, ast_t *args, env_t *env)
{
    ast_t *fun = AST_CONS_1ST(args);
    ast_t *parameters = AST_CONS_2ND(args);
    ast_t *body = AST_CONS_CDR(AST_CONS_CDR(args));
    char *name = AST_ID_NAME(fun);
    int x, y;
    assert(env_position(toplevel_env, name, &x, &y) == true);
    compile_global_function(name, parameters, body, env);
    // 创建函数对象
    emit(s, OP_NEW1(OP_FUN, name));
    // 绑定到顶层环境
    emit(s, OP_NEW2(OP_GSET, x, y));
    toplevel_var_count++;
    emit(s, OP_NEW0(OP_POP));
    emit(s, OP_NEW0(OP_NIL));
}

static void
compile_fun(code_t *s, ast_t *x, env_t *env)
{
    static int counter = 0;
    // 生成不重复的函数名
    char text[256] = {0};
    snprintf(text, sizeof(text), "(fun_%d)", counter);
    counter++;
    const char *name = strdup(text);
    // 声明函数名
    env_push_back(toplevel_env, name, NULL);
    ast_t *parameters = AST_CONS_1ST(x);
    ast_t *body = AST_CONS_CDR(x);
    compile_global_function(name, parameters, body, env);
    // 创建函数对象
    emit(s, OP_NEW1(OP_FUN, name));

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
    emit(s, OP_NEW0(OP_TUNWIND));
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
compile_let(code_t *s, ast_t *args, env_t *env)
{
    ast_t *bindings = AST_CONS_1ST(args);
    ast_t *parameters = ast_cons_map1(bindings);
    ast_t *exprs = ast_cons_map2(bindings);

    ast_t *body = AST_CONS_CDR(args);
    // 编译表达式列表
    compile_args(s, exprs, env);
    // 写入传参指令
    int arity = ast_cons_length(parameters);
    emit(s, OP_NEW1(OP_ARG, arity));
    // 扩展环境
    env_t *nenv = env_new(env);
    while (parameters != NULL) {
        ast_t *par = AST_CONS_CAR(parameters);
        env_bind(nenv, AST_ID_NAME(par), NULL);
        parameters = AST_CONS_CDR(parameters);
    }
    // 编译绑定
    compile_sequence(s, body, nenv);
    // 恢复环境
    emit(s, OP_NEW0(OP_RENV));
}

static void
compile_progn(code_t *s, ast_t *args, env_t *env)
{
    compile_sequence(s, args, env);
}

static void
compile_return(code_t *s, ast_t *args, env_t *env)
{
    ast_t *expr = AST_CONS_1ST(args);
    compiler_compile_any(s, expr, env);
    emit(s, OP_NEW0(OP_RET));
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
    if (op->type == AST_ID) {
        assert(op->type == AST_ID);
        char *name = AST_ID_NAME(op);
        if (utils_str_equal(name, "set")) {
            compile_set(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "defun")) {
            compile_defun(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "fun")) {
            compile_fun(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "if")) {
            compile_if(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "let")) {
            compile_let(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "progn")) {
            compile_progn(s, AST_CONS_CDR(x), env);
        } else if (utils_str_equal(name, "return")) {
            compile_return(s, AST_CONS_CDR(x), env);
        } else {
            compile_funcall(s, x, env);
        }
    } else {
        compile_funcall(s, x, env);
    }
}

static void
compiler_compile_id(code_t *s, ast_t *id, env_t *env)
{
    assert(id->type == AST_ID);
    int x, y;
    const char *name = AST_ID_NAME(id);
    bool is_found = env_position(env, name, &x, &y);
    if (is_found) {
        emit(s, OP_NEW3(OP_REF, x, y, name));
    } else {
        assert(env_position(toplevel_env, name, &x, &y) == true);
        emit(s, OP_NEW3(OP_GREF, x, y, name));
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
compiler_compile_sharp_v(code_t *s, ast_t *sv, env_t *env)
{
    assert(sv->type == AST_SHARP_V);
    // 从左到右编译向量中的元素
    const ast_t *elements = AST_SHARP_V_ELEMENTS(sv);
    compile_args(s, elements, env);
    // 写入用于创建向量的指令
    int length = ast_cons_length(elements);
    emit(s, OP_NEW1(OP_MKVEC, length));
}

static void
compiler_compile_any(code_t *s, ast_t *x, env_t *env)
{
    switch (x->type) {
        case AST_BOOL:
            emit(s, OP_NEW1(OP_PUSH, object_bool_new(AST_BOOL_VALUE(x))));
            break;
        case AST_CHAR:
            emit(s, OP_NEW1(OP_PUSH, object_char_new(AST_CHAR_VALUE(x))));
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
        case AST_SHARP_V:
            compiler_compile_sharp_v(s, x, env);
            break;
        case AST_SYMBOL:
            emit(s, OP_NEW1(OP_PUSH, object_symbol_intern(AST_SYMBOL_NAME(x))));
            break;
        default :
            fprintf(stderr, "%d:未定义的语法类型\n", x->type);
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
