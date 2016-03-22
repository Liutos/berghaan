#include "ast.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    const char *code = "(set a 1) (+ a a)";
    lexer_t *lexer = lexer_new(code);
    parser_t *parser = parser_new(lexer);
    ast_t *prog = program(parser);
    ast_print(prog, stdout);
    return 0;
}
