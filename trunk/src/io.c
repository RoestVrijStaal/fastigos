#include "devel/io.h"

void outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

uint8_t inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

void serial_init(uint16_t port)
{
	outb(port + 1, 0x00);    /* Disable all interrupts */
	outb(port + 3, 0x80);    /* Enable DLAB (set baud rate divisor) */
	outb(port + 0, 0x03);    /* Set divisor to 3 (lo byte) 38400 baud */
	outb(port + 1, 0x00);    /*                  (hi byte) */
	outb(port + 3, 0x03);    /* 8 bits, no parity, one stop bit */
	outb(port + 2, 0xC7);    /* Enable FIFO, clear them, with 14-byte threshold */
	outb(port + 4, 0x0B);    /* IRQs enabled, RTS/DSR set */
}

uint8_t serial_transmit_empty(uint16_t port)
{
	return inb(port + 5) & 0x20;
}

void serial_write(uint16_t port, char b)
{
	while (serial_transmit_empty(port) == 0);
	outb(port, b);
}
