#include "ast.h"
#include "compiler.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "object.h"
#include "vm.h"

#include <stdio.h>

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    interpreter_init();

    const char *code = "(defun equal? (x y) (= x y)) (set foobar 233) (if (equal? foobar foobar) (yes) 666)";
    lexer_t *lexer = lexer_new(code);
    parser_t *parser = parser_new(lexer);
    ast_t *prog = program(parser);
    ast_print(prog, stdout);
    printf("\n");
    ast_dfs(prog);
    printf("\n");
    object_t *obj = interpret(prog);
    object_print(obj);
    printf("\n");

    code = "(defun foo (x) (if x true false)) (defun bar (y) (foo y)) (bar 233)";
    lexer = lexer_new(code);
    parser = parser_new(lexer);
    prog = program(parser);
    ast_print(prog, stdout);
    puts("");
    compiler_init();
    compiler_compile(prog);
    compiler_done();
    return 0;
}
