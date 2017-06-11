/**
 * 提供读取及输出UTF-8编码的字节序列的功能
 */
#pragma once

#include <stdint.h>
#include <stdio.h>

extern size_t utf8_msread(const char *, uint32_t *);
extern uint32_t utf8_fread(FILE *, unsigned int *);
extern uint32_t utf8_sread(const char *, unsigned int *);
extern void utf8_code_to_bytes(uint32_t, uint8_t *, size_t *);
extern void utf8_fprintf(FILE *, uint32_t);
extern void utf8_mfprintf(FILE *, uint32_t *, size_t);
extern uint32_t *utf8_from_chars(const char *, size_t, size_t *);