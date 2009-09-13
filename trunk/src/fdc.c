#include "typedefs.h"
#include "fdc.h"
#include "config.h"
#include "segments.h"
#include "video.h"
#include "pit.h"

struct fdc_s
{
	uint8_t		type;
	uint8_t		st0;
	uint8_t		pcn;
} fdc;

static inline void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

static __inline__ unsigned char inb(unsigned short port)
{
   unsigned char ret;
   asm volatile ("inb %0,%1":"=a"(ret):"Nd"(port));
   return ret;
}

unsigned char irq_fdc_status;

void irq_fdc(void)
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	irq_fdc_status = TRUE;

	outb(PIC1_CMD, PIC_EOI);
	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret"); /* BLACK MAGIC! */
}

int8_t fdc_sendbyte(uint8_t byte)
{
	uint8_t msr;
	uint8_t timeout;
	for (timeout = 0;timeout < 128;timeout++)
	{
		msr = inb(FDC_MSR);
		if ((msr & 0xc0) == 0x80)
		{
			outb(FDC_DATA, byte);
			return FDC_OK;
		}
		inb(0x80);
	}
	return FDC_TIMEOUT;
}

int8_t fdc_getbyte()
{
	uint8_t msr;
	uint8_t timeout;
	for (timeout = 0;timeout < 128;timeout++)
	{
		msr = inb(FDC_MSR);
		if ((msr & 0xd0) == 0xd0)
		{
			return inb(FDC_DATA);
		}
		inb(0x80);
	}
	return FDC_TIMEOUT;
}


uint8_t fdc_reset(void)
{
	uint8_t i;
	outb(FDC_DOR, 0);	// fdc off
	outb(FDC_DSR, 0);	// data rate 500kbs

	irq_fdc_status = FALSE;
	install_idt_handler(IRQ_FDC, (uint32_t)irq_fdc);
	outb(FDC_DOR, 0xc);	// fdc on
	while (!irq_fdc_status);
	for (i= 0; i<=4;i++)
	{
		// get status
		outb(FDC_DATA, FDC_COMMAND_SENSE_INTERRUPT_STATUS);
		fdc.st0 = inb(FDC_DATA);
		fdc.pcn = inb(FDC_DATA);
	}
	outb(FDC_DATA, FDC_COMMAND_SPECIFY);
	outb(FDC_DATA, 0xdf); /* SRT = 3ms, HUT = 240ms */
	outb(FDC_DATA, 0x2); /* HLT = 16ms, ND = 0 */
	inb(FDC_DATA);
	return FDC_OK;
}

uint8_t fdc_identify(void)
{
	outb(FDC_DATA, FDC_COMMAND_VERSION);
	fdc.type = inb(FDC_DATA);
	return fdc.type;
}
