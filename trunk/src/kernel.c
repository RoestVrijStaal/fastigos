#include "kernel.h"

void _start(uint32_t ramsizekb)
{
	uint32_t ramsize = ramsizekb *1024;

	debug_init();

	debug_write_string("fastigOS is here (obokaman)\n");

	gdt_init();
	idt_init();

	mem_init(ramsize);

	video_init();

	syswait();
}

