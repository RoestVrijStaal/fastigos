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
   SPECIFY =                    0x3,
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

static __inline__ void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

static __inline__ uint8_t inb(uint16_t port)
{
   unsigned char ret;
   asm volatile ("inb %0, %1":"=a"(ret):"Nd"(port));
   return ret;
}

void fdc_skel()
{
	// DMA/non-DMA in SPECIFY command
	// FIFO threshold in CONFIGURE command


	// examine RQM=1 and DIO=0 bits of MSR
	// RQM bit 7
	// DIO bit 6

	// Command phase

	// examine RQM=0 of MSR

	// Execution phase

	// Result phase
	// int recived
	// examine RQM=1 and DIO=1 bits of MSR
	// read data from FIFO
	// examine RQM=1 and DIO=0 bits of MSR
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

int8_t fdc_readyforcommand()
{
	uint8_t status;
	uint8_t timeout;

	for (timeout=0; timeout <= 3; timeout++)
	{
		status = inb(MAIN_STATUS_REGISTER);
		status = status >> 6;
		if ( status == 0x2 )
		{
			//RQM=1
			//DIO=0
			// yeah, fdc ready for command!
			return FDC_OK;
		}
		timer_wait(200);		// wait 200ms (arbitrary wait)
	}
	// other case... fdc not ready
	video_printstring(7,"DEBUG: fdc ready for command timeout!\n");
	return FDC_ERROR;
}

int8_t fdc_specify(uint8_t srt, uint8_t hut, uint8_t hlt)
{
	//fdc_specify(0xd, 0xf, 0x1);

	uint8_t ready;
	uint8_t srthut_b;
	uint8_t hlt_b;

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc specify commnand error\n");
		return FDC_ERROR;
	}
	// Command phase
	outb(FDC_DATA, FDC_COMMAND_SPECIFY);
	/*
	SRT = 4bit
	HUT = 4bit
	*/
	srthut_b = srt << 4;
	srthut_b = srthut_b + hut;
	outb(FDC_DATA, srthut_b);
	/*
	HLT = 7bit
	*/
	hlt_b = hlt << 1;
	outb(FDC_DATA, hlt_b);

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc specify commnand error\n");
		return FDC_ERROR;
	}

	return FDC_OK;
}

int8_t fdc_version()
{
	uint8_t ready;

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc version commnand error\n");
		return FDC_ERROR;
	}
	// Command phase
	outb(FDC_DATA, FDC_COMMAND_VERSION);
	fdc.type = inb(FDC_DATA);
	// Result phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc version commnand error\n");
		return FDC_ERROR;
	}

	return fdc.type;
}

int8_t fdc_configure(uint8_t eis, uint8_t efifo, uint8_t poll, uint8_t fifothr, uint8_t pretrk)
{
	uint8_t ready;
	uint8_t cfg_b;

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc configure commnand error\n");
		return FDC_ERROR;
	}

	fifothr = fifothr & 0x0f;
	poll = (poll & 0x01) << 4;
	efifo = (efifo & 0x01) << 5;
	eis = (eis & 0x01) << 6;
	cfg_b = eis + efifo + poll + fifothr;
	// Command phase
	outb(FDC_DATA, FDC_COMMAND_CONFIGURE);
	outb(FDC_DATA, 0x0);
	outb(FDC_DATA, cfg_b);
	outb(FDC_DATA, pretrk);
	// Result phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc configure commnand error\n");
		return FDC_ERROR;
	}

	return FDC_OK;
}

int8_t fdc_recalibrate(uint8_t drive)
{
	int8_t ready;
	int8_t drive_b;
	uint8_t timeout;

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc recalibrate commnand error\n");
		return FDC_ERROR;
	}
	drive_b = drive & 0x03;
	fdc.wait_irq = FALSE;
	install_idt_handler(IRQ_FDC, (uint32_t)irq_fdc);

	// Command phase
	outb(FDC_DATA, FDC_COMMAND_RECALIBRATE);
	outb(FDC_DATA, drive_b);

	// Execution phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc recalibrate commnand error\n");
		return FDC_ERROR;
	}
	timeout = 0;
	while (!fdc.wait_irq)
	{
		timer_wait(100);
		if ( timeout == 10)
		{
			video_printstring(7,"DEBUG: fdc recalibrate commnand timeout\n");
			return FDC_ERROR;
		}
		timeout++;
	}
	// Result phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc recalibrate commnand error\n");
		return FDC_ERROR;
	}
	return FDC_OK;
}

int8_t fdc_seek(uint8_t drive, uint8_t cylinder)
{
	int8_t ready;
	int8_t drive_b;

	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc seek commnand error\n");
		return FDC_ERROR;
	}
	drive_b = drive & 0x03;
	fdc.wait_irq = FALSE;
	install_idt_handler(IRQ_FDC, (uint32_t)irq_fdc);

	// Command phase
	outb(FDC_DATA, FDC_COMMAND_SEEK);
	outb(FDC_DATA, drive_b);
	outb(FDC_DATA, cylinder);

	// Execution phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc seek commnand error\n");
		return FDC_ERROR;
	}
	// Result phase
	fdc_sense_interrupt_status();
	video_print_uint8(7, fdc.pcn);
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc seek commnand error\n");
		return FDC_ERROR;
	}
	return FDC_OK;
}

int8_t fdc_sense_interrupt_status(void)
{
	int8_t ready;

	// pag 30
	// Result phase
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc sense interrupt status commnand error\n");
		return FDC_ERROR;
	}
	outb(FDC_DATA, FDC_COMMAND_SENSE_INTERRUPT_STATUS);
	fdc.st0 = inb(FDC_DATA);
	fdc.pcn = inb(FDC_DATA);
	ready = fdc_readyforcommand();
	if (ready != FDC_OK)
	{
		video_printstring(7,"DEBUG: fdc sense interrupt status commnand error\n");
		return FDC_ERROR;
	}
	return FDC_OK;
}















/*
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
	outb(FDC_DATA, 0xdf); // SRT = 3ms, HUT = 240ms
	outb(FDC_DATA, 0x2); // HLT = 16ms, ND = 0
	inb(FDC_DATA);
	return FDC_OK;
}

uint8_t fdc_identify(void)
{
	outb(FDC_DATA, FDC_COMMAND_VERSION);
	fdc.type = inb(FDC_DATA);
	return fdc.type;
}
*/
