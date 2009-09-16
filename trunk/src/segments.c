#include "typedefs.h"
#include "config.h"
#include "string.h"
#include "video.h"
#include "pic.h"

static inline void outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %b1, %0"::"d"(port),"a"(data));
}

/*
; dw Limit L0-L15(16)
; dw Base B0-B15(16)
; db Base B16-B23(8)
; db P(1)/DPL(2)/S(1=codi/dades)/E(1)/X(1)/RW(1)/0
; db G(1)/OpSize(1)/0/0/Limit L16-L19(4)
; db Base B24-B31
*/

void install_idt_handler(uint8_t index, uint32_t handler);

void default_idt_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");
	//video_printstring(7, "Interrupts has me! ");
	outb(PIC1_CMD, PIC_EOI);
	outb(PIC2_CMD, PIC_EOI);
	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret"); /* BLACK MAGIC! */
}

struct segment_descriptor
{
	uint16_t	limit_15_0;
	uint16_t	base_addr_15_0;
	uint8_t		base_addr_23_16;
	uint8_t		type:4;			// 0000 Reserved
						// 0001 16bit TTS (Available)
						// 0010 LDT
						// 0011 16bit TTS (Busy)
						// 0100 16bit call gate
						// 0101 Task gate
						// 0110 16bit Interrupt Gate
						// 0111 16bit Trap gate
						// 1000 Reserved
						// 1001 32bit TSS (Available)
						// 1010 Reserved
						// 1011 32bit TSS (Busy)
						// 1100 32bit Call Gate
						// 1101 Reserved
						// 1110 32bit Interrupt Gate
						// 1111 32bit Trap Gate
	uint8_t		s:1;			// Type of descriptor (1=data/0=sys)
	uint8_t		dpl:2;			// Privilege level
	uint8_t		p:1;			// Segment is present
	uint8_t		limit_19_16:4;
	uint8_t		avl:1;			// Available
	uint8_t		l:1;			// 64bit segment
	uint8_t		db:1;			// Operation size
	uint8_t		g:1;			// Granularity
	uint8_t		base_addr_31_24;
} __attribute__((__packed__));

struct idt_descriptor
{
	uint16_t	offset_0_15;
	uint16_t	segment_selector;	// 0x8 normaly
	uint8_t		reserved;		// 00000000b
	uint8_t		reserved2;		// 01110001b
	uint16_t	offset_16_31;
} __attribute__((__packed__));

static struct segment_descriptor gdt[] =
{
	NULL_DESCRIPTOR,
	BUILD_4K_CS_RE_SEG_DESC(0,0xffffffff),
	BUILD_4K_SS_RW_SEG_DESC(0x100000,0xffff),
	BUILD_4K_DS_RW_SEG_DESC(0,0xffffffff),
};

struct gdtInfo
{
	uint16_t	size;
	uint32_t	addr;
} __attribute__((__packed__));

void gdt_init(void)
{
	struct gdtInfo gdtdesc;

	gdtdesc.size = sizeof(gdt) -1;
	gdtdesc.addr = (uint32_t) gdt;

	__asm__ __volatile__("lgdt %0;"
		"ljmp $0x8, $pmode;"
		"pmode: mov $0x18, %%eax;"
		"movl %%eax, %%ds;"
		"mov $0x10, %%eax;"
		"movl %%eax, %%ss;"
		:"=m"(gdtdesc));

	video_printstring(7, "GDT(0 = ");
	video_print_uint32(7, gdtdesc.addr);
	video_printstring(7, ":");
	video_print_uint16(7, gdtdesc.size);
	video_printstring(7, ")\n");

}

static struct idt_descriptor idt[255];

struct idtInfo
{
	uint16_t	size;
	uint32_t	addr;
} __attribute__((__packed__));

void idt_init(void)
{
	struct idtInfo idtdesc;
	uint8_t i;

	memset(idt, 0, sizeof(idt));

	idtdesc.size = sizeof(idt) -1;
	idtdesc.addr = (uint32_t) idt;

	__asm__ __volatile("cli");
	outb(PIC1_CMD, PIC_ICW1);
	outb(PIC2_CMD, PIC_ICW1);
	__asm__ __volatile__("lidt %0;":"=m"(idtdesc));
	outb(PIC1_DATA, PIC_ICW4);
	outb(PIC2_DATA, PIC_ICW4);
	__asm__ __volatile("sti");

	video_printstring(7, "IDT(0 = ");
	video_print_uint32(7, idtdesc.addr);
	video_printstring(7, ":");
	video_print_uint16(7, idtdesc.size);
	video_printstring(7, ")\n");

	// 32 intel default interrupts 
	// + 16 IRQ's
	for(i=0;i<=48;i++)
	{
		install_idt_handler(i, (uint32_t)default_idt_handler);
	}

	__asm__("sti");
}

void install_idt_handler(uint8_t index, uint32_t handler)
{
	__asm__ __volatile("cli");

	if ( handler == 0x0 )
	{
		memset(idt + index, 0, sizeof(struct idt_descriptor));
	}
	else
	{
		idt[index] = BUILD_IDT_DESCRIPTOR(handler);
	}

	__asm__ __volatile("sti");
}

