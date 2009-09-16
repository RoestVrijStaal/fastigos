#include "typedefs.h"
#include "config.h"
#include "segments.h"
#include "pit.h"
#include "video.h"
#include "fdc.h"
#include "pic.h"

enum FloppyRegisters
{
   STATUS_REGISTER_A                = 0x3F0, // read-only
   STATUS_REGISTER_B                = 0x3F1, // read-only
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4, // read-only
   DATA_RATE_SELECT_REGISTER        = 0x3F4, // write-only
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7, // read-only
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7  // write-only
};

enum FloppyCommands
{
   READ_TRACK =                 2,
   SPECIFY =                    3,
   SENSE_DRIVE_STATUS =         4,
   WRITE_DATA =                 5,
   READ_DATA =                  6,
   RECALIBRATE =                7,
   SENSE_INTERRUPT =            8,
   WRITE_DELETED_DATA =         9,
   READ_ID =                    10,
   READ_DELETED_DATA =          12,
   FORMAT_TRACK =               13,
   SEEK =                       15,
   VERSION =                    16,
   SCAN_EQUAL =                 17,
   PERPENDICULAR_MODE =         18,
   CONFIGURE =                  19,
   VERIFY =                     22,
   SCAN_LOW_OR_EQUAL =          25,
   SCAN_HIGH_OR_EQUAL =         29
};

/*
 * fdc status structure
 */
struct fdc_s
{
	uint8_t		wait_irq;
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

uint8_t fdc_motoron(uint8_t drive)
{
	uint8_t sendbyte;

	if (drive == 0)
	{
		sendbyte = 0x10;
	}
	else if (drive == 1)
	{
		sendbyte = 0x20;
	}
	else if (drive == 2)
	{
		sendbyte = 0x40;
	}
	else if (drive == 3)
	{
		sendbyte = 0x80;
	}
	else
	{
		return FDC_ERROR;
	}
	outb(FDC_DOR, sendbyte);
	timer_wait(50);
	return FDC_OK;
}

uint8_t fdc_motoroff()
{
	// all motors off
	//@todo find better way to do this!
	uint8_t sendbyte = 0x0;

	outb(FDC_DOR, sendbyte);
	return FDC_OK;
}

void irq_fdc(void)
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	fdc.wait_irq = TRUE;

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

	fdc.wait_irq = FALSE;
	install_idt_handler(IRQ_FDC, (uint32_t)irq_fdc);
	outb(FDC_DOR, 0xc);	// fdc on
	while (!fdc.wait_irq);
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
