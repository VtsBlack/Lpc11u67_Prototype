#ifndef __DEBUG_H
#define __DEBUG_H
#include <stdarg.h>
void debug_uart_rb_init(void);
void Debug(const char *Str, ...);
void DebugHex(const uint8_t *dat, int sz);
#endif
