#include "82c54.h"

uint8_t pic82c54_ticks;

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

void pic82c54_clock_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	pic82c54_ticks++;
	//@todo only a sample
	if ( pic82c54_ticks == 0 )
	{
		debug_print("tick()\n");
	}
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_EOI));
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_EOI));

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");		// black magic
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

	pic82c54_ticks = 0;
	__asm__ __volatile__("cli");
	install_idt_handler(32, (uint32_t)pic82c54_clock_handler);
	__asm__ __volatile__("sti");

	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS timer82c54_deinit(void)
{
	debug_print("timer82c54_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}

void sleep(uint32_t seconds)
{
	uint32_t currentTicks;
	uint32_t newTicks;
	uint32_t r;

	for(r=0;r<seconds;r++)
	{
		__asm__ __volatile__("cli");
		currentTicks = pic82c54_ticks;
		__asm__ __volatile__("sti");
		while (1)
		{
			__asm__ __volatile__("cli");
			newTicks = pic82c54_ticks;
			__asm__ __volatile__("sti");
			if ( currentTicks != newTicks )
			{
				break;
			}
		}
	}
}
