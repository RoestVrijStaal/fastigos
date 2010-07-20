#include "devel/system.h"

__attribute__((noreturn)) void syswait(void)
{
	debug_write_string("syswait()");
	while(1)
	{
		__asm__("hlt");
	}
}

void *memset(void *s, uint8_t c, uint32_t size)
{
    char *p = (char *) s;
    while (size-- > 0)
        *p++ = c;
    return s;
}

void default_idt_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	outb(PIC1_CMD, PIC_EOI);
	outb(PIC2_CMD, PIC_EOI);

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");
}

void gdt_init(void)
{
	debug_write_string("gdt_init()\n");

	static struct gdtInfo gdtdesc;

	gdtdesc.size = sizeof(gdt) -1;
	gdtdesc.addr = (uint32_t) gdt;

	__asm__ __volatile__("lgdt %0;"
		"ljmp $0x8, $pmode;"
		"pmode: mov $0x18, %%eax;"
		"movl %%eax, %%ds;"
		"mov $0x10, %%eax;"
		"movl %%eax, %%ss;"
		:"=m"(gdtdesc));

	debug_write_string("GDT: ");
	debug_write_uint32(gdtdesc.addr);
	debug_write_string(":");
	debug_write_uint16(gdtdesc.size);
	debug_write_string("\n");

	debug_write_string("gdt_init() return\n");
}

void install_idt_handler(uint8_t index, uint32_t handler)
{
	debug_write_string("install_idt_handler() begin\n");

	if ( handler == 0x0 )
	{
		debug_write_string("cleaning all idt...");
		memset(idt + index, 0, sizeof(struct idt_descriptor));
		debug_write_string("ok\n");
	}
	else
	{
		idt[index] = BUILD_IDT_DESCRIPTOR(handler);
	}


	debug_write_string("install_idt_handler(");
	debug_write_uint8(index);
	debug_write_string(", ");
	debug_write_uint32(handler);
	debug_write_string(")\n");

	debug_write_string("install_idt_handler() return\n");
}

void idt_init(void)
{
	debug_write_string("idt_init()\n");

	static struct idtInfo idtdesc;
	uint8_t i;

	memset(idt, 0, sizeof(idt));

	idtdesc.size = sizeof(idt) -1;
	idtdesc.addr = (uint32_t) idt;

	__asm__ __volatile__("cli");
	outb(PIC1_CMD, PIC_ICW1);
	outb(PIC2_CMD, PIC_ICW1);
	__asm__ __volatile__("lidt %0;":"=m"(idtdesc));
	outb(PIC1_DATA, PIC_ICW4);
	outb(PIC2_DATA, PIC_ICW4);

	/* 32 intel default interrupts */
	for(i=0;i<=31;i++)
	{
		install_idt_handler(i, (uint32_t)default_idt_handler);
	}
	/* + 16 IRQ's */
	for(i=31;i<=48;i++)
	{
		install_idt_handler(i, (uint32_t)default_idt_handler);
	}

	__asm__ __volatile__("sti");
	debug_write_string("idt_init() return\n");
}

