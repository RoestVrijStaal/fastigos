#include "debug.h"

void debug_init()
{
	serial_init(com1);
}

void debug_write_c(char b)
{
	serial_write(com1, b);
}

void debug_write_string(char *s)
{
	while(*s!=0)
	{
		debug_write_c((char)*s);
		s++;
	}
}

void debug_write_uint32(uint32_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;

	debug_write_string("0x");
	d = 28;
	for ( i=0; i<=7;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		debug_write_c(*b);
		d-=4;
	}
}

void debug_write_uint16(uint16_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;
	debug_write_string("0x");
	d = 12;
	for ( i=0; i<=3;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		debug_write_c(*b);
		d-=4;
	}
}

void debug_write_uint8(uint8_t number)
{
	uint8_t i;
	uint8_t c;
	uint8_t d;
	char *b;

	debug_write_string("0x");
	d = 4;
	for ( i=0; i<=1;i++)
	{
		c = number >> d & 0x0f;
		b = hextable+c;
		debug_write_c(*b);
		d-=4;
	}
}

