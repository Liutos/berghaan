/**
 * 定义日志工具的接口
 */
#pragma once

extern void log_close(void);
extern void log_lprintf(int, const char *, ...);
extern void log_open(void);
