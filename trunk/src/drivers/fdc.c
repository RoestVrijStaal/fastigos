#include "fdc.h"
/*
	Inspired in:
	http://gaztek.sourceforge.net/osdev/hardware/fdc.c
*/

struct fdcStatus_s
{
	uint8_t type;
	uint8_t interrupt;
};
struct fdcStatus_s fdcStatus;

void fdc_init()
{
	debug_write_string("fdc_init() begin\n");

	/* get type of fdc */
	fdc_send(VERSION);
	fdcStatus.type = fdc_get();
	debug_write_string("fdc_init() controller type: ");
	debug_write_uint8(fdcStatus.type);
	debug_write_string("\n");
	debug_write_string("fdc_init() ");
	if ( fdcStatus.type == 0x80 )
	{
		debug_write_string("NEC765");
	}
	else if ( fdcStatus.type == 0x90 )
	{
		debug_write_string("Enhanced");
	}
	else
	{
		debug_write_string("unknown");
	}
	debug_write_string(" controller found.\n");

	/* install isr for fdc */
	debug_write_string("fdc_init() installing isr...\n");
	__asm__ __volatile__("cli");
	install_idt_handler(FDC_INT, (uint32_t)fdc_isr);
	__asm__ __volatile__("sti");


	fdc_motorOn();
	fdc_motorOff();
	debug_write_string("fdc_init() return\n");
}

/* sendbyte() routine from intel manual */
void fdc_send(int byte)
{
   volatile int msr;
   int tmo;
   
   for (tmo = 0;tmo < 128;tmo++) {
      msr = inb(MAIN_STATUS_REGISTER);
      if ((msr & 0xc0) == 0x80) {
	 outb(DATA_FIFO,byte);
	 return;
      }
      inb(0x80);   /* delay */
   }
}

/* getbyte() routine from intel manual */
int fdc_get()
{
   volatile int msr;
   int tmo;
   
   for (tmo = 0;tmo < 128;tmo++) {
      msr = inb(MAIN_STATUS_REGISTER);
      if ((msr & 0xd0) == 0xd0) {
	 return inb(DATA_FIFO);
      }
      inb(0x80);   /* delay */
   }
   return -1;   /* read timeout */
}

void fdc_isr()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	fdcStatus.interrupt = 0x0;
	debug_write_string("fdc_interrupt()\n");
	outb(PIC1_CMD, PIC_EOI);
	outb(PIC2_CMD, PIC_EOI);

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");

}

void fdc_motorOn()
{
	debug_write_string("fdc_motorOn()\n");
	outb(DIGITAL_OUTPUT_REGISTER,0x1c);
}

void fdc_motorOff()
{
	debug_write_string("fdc_motorOff()\n");
	outb(DIGITAL_OUTPUT_REGISTER,0x0c);
}

/* review pending */
void reset_fdc()
{
	debug_write_string("reset_fdc() begin\n");
	debug_write_string("reset_fdc() waiting int...\n");
	outb(DIGITAL_OUTPUT_REGISTER, 0x00); /*disable controller*/
  	outb(DIGITAL_OUTPUT_REGISTER, 0x0c); /*enable controller*/
	while(fdcStatus.interrupt);
	debug_write_string("reset_fdc() int completed\n");
	debug_write_string("reset_fdc() return\n");
}
