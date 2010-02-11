#ifndef __IO_H__
#define __IO_H__

#include "typedefs.h"

#define com1 0x3f8
#define com2 0x2f8
#define com3 0x3e8
#define com4 0x2e8

#define PIC1_CMD 0x20
#define PIC_ICW1 0x11
#define PIC2_CMD 0xa0
#define PIC1_DATA 0x21
#define PIC_ICW4 0x01
#define PIC2_DATA 0xa1
#define PIC_EOI 0x20

uint8_t inb(uint16_t port);

void outb(uint16_t port, uint8_t data);

/* serial port related */

void serial_init(uint16_t port);

uint8_t serial_transmit_empty(uint16_t port);

void serial_write(uint16_t port, char b);

#endif
