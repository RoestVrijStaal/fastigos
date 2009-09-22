#include "config.h"
#include "typedefs.h"
#include "segments.h"
#include "pic.h"
#include "pit.h"
#include "kernel.h"
#include "video.h"
#include "fdc.h"
#include "memory.h"
#include "rtc.h"
#include "mutex.h"
#include "util.h"

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

void _start(void)
{
	video_init();
	video_printstring(14,"fastigOS is here! (Caramel Man No 4: Obokaman)\n");
	//cmosdump();
	/*
	char cpuid[13];
	char *cpuid0;
	char *cpuid1;
	char *cpuid2;
	video_printstring(7,"CPU type: ");
	BREAKPOINT_INTEL;
	cpuid0 = cpuid;
	cpuid1 = cpuid+4;
	cpuid2 = cpuid+8;

	__asm__ __volatile__(
			"cpuid;"
			:"=b"(cpuid0),"=c"(cpuid1),"=d"(cpuid2):"al"(0x0)
			);
	cpuid[13] = 0;
	BREAKPOINT_INTEL;
	video_printstring(7, cpuid);
	video_printstring(7, "\n");
	*/

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&gdt_init);
	video_printstring(7, ") Init GDT...");
	gdt_init();

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&idt_init);
	video_printstring(7, ") Init IDT...");
	idt_init();

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&pit_init);
	video_printstring(7, ") Init PIT...");
	pit_init();

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&rtc_init);
	video_printstring(7, ") Init RTC...");
	rtc_init();

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&pic_init);
	video_printstring(7, ") Init PIC...");
	pic_init();

	video_printstring(7, "(");
	video_print_uint32(9, (uint32_t)&mm_init);
	video_printstring(7, ") Init MM...");
	mm_init();
	video_printstring(7, "OK\n");
	/*
	video_printstring(7, "Init FDC...\n");

	video_printstring(7, "Reset FDC...");
	fdc_reset();
	video_printstring(7, "TODO\n");
	*/


	int8_t fdc_type = fdc_version();
	if ( fdc_type != FDC_ERROR )
	{
		if ( (uint8_t)fdc_type == FDC_VERSION_IS_ENHANCED )
		{
			video_printstring(7, "fdc > Model 82077AA detected\n");
		}
		else
		{
			video_printstring(7, "fdc > Unknown floppy disk controller! (");
			video_print_uint8(14, fdc_type);
			video_printstring(7, ")\n");
			kernel_crash();
		}
	}
	/*
	fdc_recalibrate(0X0);
	fdc_seek(0x0, 0x2);
	fdc_read(0x0, 0x0, 0x0);
	*/

	// test for obtain the cursor offset
	uint8_t x, y;
	outb(0x3d4, 0x0e);		/* Get cursor low byte position	*/
	x = inb(0x3d5);
	outb(0x3d4, 0x0f);		/* And add cursor high byte position	*/
	y= inb(0x3d5);
	video_print_uint8(7, x);
	video_print_uint8(7, y);

	video_printstring(7, "System up and running... (mainloop)");
	while(1)
	{
		__asm__("hlt");
	}
}

void kernel_crash(void)
{
	__asm__ __volatile__("cli");
	video_printstring(4, "FATAL ERROR, SYSTEM HALTED.");
	while(1)
	{
		__asm__("hlt");
	}
}


