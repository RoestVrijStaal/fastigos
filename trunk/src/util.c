#include "typedefs.h"

static inline void outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %w1, %0"::"a"(data),"d"(port));
}
