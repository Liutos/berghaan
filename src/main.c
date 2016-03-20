#include "lexer.h"
#include "parser.h"

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    const char *code = "(set a 1) (+ a a)";
    lexer_t *lexer = lexer_new(code);
    parser_t *parser = parser_new(lexer);
    program(parser);
    return 0;
}
