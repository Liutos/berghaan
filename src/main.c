#include "assembler.h"
#include "ast.h"
#include "base/log.h"
#include "base/string.h"
#include "bif.h"
#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "object.h"
#include "vm.h"

#include <stdio.h>
#include <syslog.h>

int
main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused)))
{
    log_open();
    // 读取标准输入
    string_t *s = string_new();
    char part[256] = {0};
    while (fgets(part, sizeof(part), stdin) != NULL) {
        string_append(s, part);
    }
    const char *code = s->data;

    lexer_t *lexer = lexer_new(code);
    parser_t *parser = parser_new(lexer);
    ast_t *prog;
    if (program(parser, &prog) == FAIL) {
        printf("Compilation fail\n");
        return 1;
    }
    ast_print(prog, stdout);
    puts("");
    bif_init(&toplevel_env, &toplevel_vec);
    // 进行编译
    compiler_init();
    compiler_compile(prog);
    code_t *ins = compiler_done();
    // 进行重定位
    ins = relocate(ins);
    code_print(ins, stdout);
    // 交给虚拟机执行
    vm_t *vm = vm_new();
    vm_execute(vm, ins);
    log_close();
    return 0;
}
