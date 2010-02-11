#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "io.h"

#define BREAKPOINT __asm__ __volatile__("xchg %bx,%bx");

static char hextable[] = "0123456789abcdef";

void debug_init();

void debug_write_c(char b);

void debug_write_string(char *string);

void debug_write_uint32(uint32_t number);

void debug_write_uint16(uint16_t number);

void debug_write_uint8(uint8_t number);

#endif
