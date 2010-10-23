#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "../common/typedefs.h"

#define BREAKPOINT __asm__ __volatile__("xchg %bx,%bx");

inline void debug_print(char *what);
void debug_print_uint8(uint8_t number);
void debug_print_uint16(uint16_t number);
void debug_print_uint32(uint32_t number);
#endif
