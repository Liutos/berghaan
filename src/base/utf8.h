/**
 * 提供读取及输出UTF-8编码的字节序列的功能
 */
#pragma once

#include <stdint.h>
#include <stdio.h>

extern void utf8_fprintf(FILE *, uint32_t);
