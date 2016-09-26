/**
 * 提供读取及输出UTF-8编码的字节序列的功能
 */
#pragma once

#include <stdint.h>
#include <stdio.h>

extern uint32_t utf8_fread(FILE *, unsigned int *);
extern uint32_t utf8_sread(const char *, unsigned int *);
extern void utf8_fprintf(FILE *, uint32_t);
