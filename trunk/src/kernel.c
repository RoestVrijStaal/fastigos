#include "kernel.h"

void _start(uint32_t ramsizekb)
{
	uint32_t ramsize = ramsizekb *1024;

	debug_init();

	debug_write_string("fastigOS is here (obokaman)\n");

	gdt_init();
	idt_init();
	/*debug_write_dump(0x0, 5);*/

	mem_init(ramsize);

	video_init();

	fdc_init();

	int8_t byte;
	null_init();
	null_write(&byte);
	null_read(&byte);
	null_deinit();

	syswait();
}

