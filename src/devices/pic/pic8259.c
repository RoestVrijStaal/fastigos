#include "pic8259.h"
// http://wiki.osdev.org/PIC
// http://www.osdever.net/tutorials/view/programming-the-pic
// http://www.osdever.net/tutorials/view/irqs
#define ICW1 0x11
#define ICW4 0x01

#define MASTER_PIC_COMMAND 0x20
#define MASTER_PIC_DATA 0x21
#define SLAVE_PIC_COMMAND 0xA0
#define SLAVE_PIC_DATA 0xA1

void pic8259_outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

uint8_t pic8259_inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

void pic8259_remap(uint16_t pic1, uint16_t pic2)
{
	// send icw1
	pic8259_outb(MASTER_PIC_COMMAND, ICW1);
	pic8259_outb(SLAVE_PIC_COMMAND, ICW1);

	// remap pics (icw2)
	pic8259_outb(MASTER_PIC_DATA, pic1);
	pic8259_outb(SLAVE_PIC_DATA, pic2);

	// icw3
	pic8259_outb(MASTER_PIC_DATA, 4);
	pic8259_outb(SLAVE_PIC_DATA, 2);

	// icw4
	pic8259_outb(MASTER_PIC_DATA, ICW4);
	pic8259_outb(SLAVE_PIC_DATA, ICW4);

	pic8259_outb(SLAVE_PIC_DATA, 0xff);

}

__FOS_DEVICE_STATUS pic8259_init(void)
{
	debug_print("pic8259_init()\n");
	pic8259_remap(0x20, 0x28);
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS pic8259_deinit(void)
{
	debug_print("pic8259_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}

