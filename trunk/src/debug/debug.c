#include "debug.h"

static char hextable[] = "0123456789abcdef";

inline void debug_print(char *what)
{
	#ifdef USE_BOCHS_PORT_E9_HACK
	uint16_t port = 0xe9;
	while (*what != 0 )
	{
		__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(*what));
		what++;
	}
	#endif
}

void debug_print_uint32(uint32_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;
	uint16_t port = 0xe9;

	debug_print("0x");
	d = 28;
	for ( i=0; i<=7;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(*b));
		d-=4;
	}
}

void debug_print_uint16(uint16_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;
	uint16_t port = 0xe9;

	debug_print("0x");
	d = 12;
	for ( i=0; i<=3;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(*b));
		d-=4;
	}
}

void debug_print_uint8(uint8_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;
	uint16_t port = 0xe9;

	debug_print("0x");
	d = 4;
	for ( i=0; i<=1;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(*b));
		d-=4;
	}
}

