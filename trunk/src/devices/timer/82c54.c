#include "82c54.h"

void timer82c54_outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

uint8_t timer82c54_inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

void timer82c54_idt_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	//debug_print("timer82c54_idt_handler()\n");
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_EOI));
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_EOI));

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");		// black magic
}

__FOS_DEVICE_STATUS timer82c54_init(void)
{
	debug_print("timer82c54_init()\n");
	//install_idt_handler(0, (uint32_t)timer82c54_idt_handler);

	// initialize counter 0
	// initialize counter 1
	// initialize counter 2
	/*
	__asm__ __volatile__("cli");
	timer82c54_outb(0x43, 0x34);
	timer82c54_outb(0x40, 0x0);
	timer82c54_outb(0x40, 0x0);
	__asm__ __volatile__("sti");
	*/
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS timer82c54_deinit(void)
{
	debug_print("timer82c54_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}

