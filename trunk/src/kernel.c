#include "devel/config.h"
#include "devel/typedefs.h"
#include "devel/segments.h"
#include "devel/pic.h"
#include "devel/pit.h"
#include "devel/kernel.h"
#include "devel/video.h"
#include "devel/fdc.h"


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

void _start(void)
{
	video_clear();
	video_printstring(9, "f");
	video_printstring(3, "astig");
	video_printstring(11, "OS");
	video_printstring(7, " is here!\n");


	//printk("fastigOS is here!");
	
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
	video_print_uint32(9, (uint32_t)&pic_init);
	video_printstring(7, ") Init PIC...");
	pic_init();

	video_printstring(7, "Init FDC...\n");

	video_printstring(7, "Reset FDC...");
	//fdc_reset();
	video_printstring(7, "OK\n");

	uint8_t fdc_type = fdc_identify();
	if ( fdc_type == FDC_VERSION_IS_ENHANCED )
	{
		video_printstring(7, "fdc > Model 82077AA detected\n");
	}
	else if ( fdc_type == FDC_VERSION_IS_STANDARD )
	{
		video_printstring(7, "fdc > Model 8272A/765A detected (not supported)\n");
		kernel_crash();
	}
	else
	{
		video_printstring(7, "fdc > Unknown floppy disk controller! (");
		video_print_uint8(14, fdc_type);
		video_printstring(7, ")\n");
		kernel_crash();
	}


	/*
	int i;
	unsigned char cmosdata;

	video_printstring(7, "CMOS dump: ");
	for (i=0;i<=0xff;i++)
	{
		outb(0x70, i);
		cmosdata = inb(0x71);
		video_print_uint8(7, cmosdata);
		video_printstring(7, ",");
	}
	video_printstring(7, "\n");
	*/
	printk("System up and running... (mainloop)");

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


