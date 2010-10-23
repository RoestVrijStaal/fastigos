#include "kernel.h"

//@todo ramsizekb is currently ignored
__attribute__((noreturn)) void _start(uint32_t ramsizekb)
{
	// never return at this point, the mmu modify the stack and call kernel_main
	mmu_init();
}

__attribute__((noreturn)) void kernel_die(char *message)
{
	debug_print("Kernel_die(\"");
	debug_print(message);
	debug_print("\")\n");
	__asm__ __volatile__("cli");
	while(1)
	{
		__asm__("hlt");
	}
}

// the real kernel main with mmu working
__attribute__((noreturn)) void kernel_main()
{
	// initialize drivers
	null_init();
	vga_init();
	// normal kernel work loop
	vga_seek(1997);
	vga_write('h');
	vga_write('e');
	vga_write('l');
	vga_write('l');
	vga_write('o');

	// end of kernel (the death!)
	vga_deinit();
	null_deinit();
	kernel_die("End of code");
}
