#include "ast.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "object.h"

#include <stdio.h>

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    interpreter_init();

    const char *code = "(define identity x x) (set foobar 233) (if (= foobar foobar) (identity 666) 250)";
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
    return 0;
}
