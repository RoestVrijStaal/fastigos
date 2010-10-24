#include "mmu.h"

static struct idt_descriptor idt[255];

static struct segment_descriptor gdt[] =
{
	NULL_DESCRIPTOR,
	BUILD_4K_CS_RE_SEG_DESC(0x0,0xffffffff),
	BUILD_4K_SS_RW_SEG_DESC(0x0,0xffffffff),
	BUILD_4K_DS_RW_SEG_DESC(0x0,0xffffffff),
};

uint8_t  * memorybitmap;
uint32_t * memorybitmaplist;

// this is default interrupt handler (do nothing)
void default_idt_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	debug_print("default_idt_handler()\n");
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_EOI));
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_EOI));

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");		// black magic
}

void default_irq_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	debug_print("default_irq_handler()\n");
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_EOI));
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_EOI));

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");		// black magic
}

void install_idt_handler(uint8_t index, uint32_t handler)
{
	debug_print("install_idt_handler() begin\n");

	if ( handler == 0x0 )
	{
		debug_print("cleaning all idt...");
		//@todo memset(idt + index, 0, sizeof(struct idt_descriptor));
		debug_print("ok\n");
	}
	else
	{
		idt[index] = BUILD_IDT_DESCRIPTOR(handler);
	}


	debug_print("install_idt_handler(");
	debug_print_uint8(index);
	debug_print(", ");
	debug_print_uint32(handler);
	debug_print(")\n");

	debug_print("install_idt_handler() return\n");
}


void idt_init(void)
{
	debug_print("idt_init()\n");

	static struct idtInfo idtdesc;
	uint8_t i;

	//memset(idt, 0, sizeof(idt));

	idtdesc.size = sizeof(idt) -1;
	idtdesc.addr = (uint32_t) idt;

	__asm__ __volatile__("cli");

	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_ICW1));
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_ICW1));
	__asm__ __volatile__("lidt %0;":"=m"(idtdesc));
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC1_DATA),"a"((uint8_t)PIC_ICW4));
	__asm__ __volatile__("outb %b1, %0"::"d"((uint16_t)PIC2_DATA),"a"((uint8_t)PIC_ICW4));

	/* 31 intel default interrupts */
	for(i=0;i<31;i++)
	{
		install_idt_handler(i, (uint32_t)default_idt_handler);
	}

	/* + 15 IRQ's */
	for(i=31;i<=48;i++)
	{
		install_idt_handler(i, (uint32_t)default_irq_handler);
	}

	__asm__ __volatile__("sti");
	debug_print("idt_init() return\n");
}

void mmu_memorymanager(void)
{
	uint32_t c;

	debug_print("mmu_memorymanager()\n");
	memorybitmap = (uint8_t *)MEMORY_MAP_OFFSET;	// memory bitmap map is up the first megabyte
	memorybitmaplist = (uint32_t *)(memorybitmap + (MEMORY_FREE_SIZE/MEMORY_BLOCK_SIZE)); // memory map list is after the memory bitmap
	debug_print("memory bitmap: ");
	debug_print_uint32((uint32_t)memorybitmap);
	debug_print("\n");

	debug_print("memory bitmap list: ");
	debug_print_uint32((uint32_t)memorybitmaplist);

	debug_print("\n");

	for(c=0;c<MEMORY_FREE_SIZE/MEMORY_BLOCK_SIZE;c++)
	{
		memorybitmap[c] = 0x0;		// mark as free
		memorybitmaplist[c] = 0x0;	// reserved in this block is 0 size
	}
}

__attribute__((noreturn)) void mmu_init_step2(void)
{
	idt_init();
	mmu_memorymanager();
	kernel_main();
}

__attribute__((noreturn))void gdt_init(void)
{
	static struct gdtInfo gdtdesc;
	gdtdesc.size = sizeof(gdt) -1;
	gdtdesc.addr = (uint32_t) gdt;
	__asm__ __volatile__("cli");
	__asm__ __volatile__("lgdt %0;"
		"ljmp $0x8, $pmode;"
		"pmode: mov $0x18, %%eax;"
		"movl %%eax, %%ds;"
		"mov $0x10, %%eax;"
		"movl %%eax, %%ss;"
		:"=m"(gdtdesc));

	debug_print("GDT: ");
	debug_print_uint32(gdtdesc.addr);
	debug_print(":");
	debug_print_uint16(gdtdesc.size);
	debug_print("\n");

	//@todo the next step causes "stack corruption" (solution: simply jump to other function in asm ignoring the stack)
	__asm__ __volatile__("movl $0x400000, %eax;"
		"movl %eax, %esp");
	__asm__ __volatile__("sti");
	// jump to mmu step 2 (set idt and so) this step never return
	mmu_init_step2();
}
__attribute__((noreturn)) void mmu_init(void)
{
	gdt_init();
}

void * kmalloc(uint32_t size)
{
	uint32_t c, c1;
	uint32_t blocks_to_search = (size/MEMORY_BLOCK_SIZE) +1; // always waste at least 1 block
	uint32_t contiguous_blocks = 0;
	uint32_t hole_begin = 0;

	debug_print("kmalloc(");
	debug_print_uint32(size);
	debug_print(")\n");
	debug_print("contiguous blocks to search: ");
	debug_print_uint32(blocks_to_search);
	debug_print("\n");

	// walk all bitmap searching for a free space of contiguous blocks
	for(c=1;c<MEMORY_FREE_SIZE/MEMORY_BLOCK_SIZE;c++)
	{
		if ( memorybitmap[c] == 0x0 )
		{
			if ( contiguous_blocks == 0 )
			{
				hole_begin = c;
				debug_print("hole begin at: ");
				debug_print_uint32(hole_begin);
				debug_print("\n");
			}
			if (contiguous_blocks == blocks_to_search)
			{
				debug_print("yeah, found hole at ");
				debug_print_uint32(hole_begin);
				debug_print("\n");
				for(c1=hole_begin;c1<(hole_begin+blocks_to_search);c1++)
				{
					debug_print("Marking hole ");
					debug_print_uint32(c1);
					debug_print("\n");
					memorybitmap[c1] = 0xff;		// mark as in use
				}
				memorybitmaplist[hole_begin] = size;	// save the size reserved
				return (void *)((hole_begin * MEMORY_BLOCK_SIZE)+MEMORY_FREE_BEGIN);
			}
			contiguous_blocks++;	// if is a free space, try to search another
		}
		else
		{
			contiguous_blocks=0;	// end of lucky, search more!
			hole_begin = 0;
		}
	}
	return 0x0;
}

void dump_memory_map()
{
	uint32_t c;
	for(c=0;c<MEMORY_FREE_SIZE/MEMORY_BLOCK_SIZE;c++)
	{
		debug_print_uint8(memorybitmap[c]);
		debug_print(",");

	}
	debug_print("\n");
}

void kfree(void * addr)
{
	uint32_t memory_block = (uint32_t)(( (uint32_t)addr - MEMORY_FREE_BEGIN)/ MEMORY_BLOCK_SIZE);
	uint32_t memory_size_to_free;
	uint32_t memory_blocks_to_free;
	uint32_t c;

	debug_print("kfree(");
	debug_print_uint32((uint32_t)addr);
	debug_print(")\n");
	debug_print("memory block to free: ");
	debug_print_uint32(memory_block);
	debug_print("\n");
	if ( memory_block > ( MEMORY_FREE_SIZE / MEMORY_BLOCK_SIZE ) )
	{
		debug_print("ERROR: memory block out of bounds\n");
		return;
	}
	memory_size_to_free = memorybitmaplist[memory_block];
	debug_print("memory size to free: ");
	debug_print_uint32(memory_size_to_free);
	debug_print("\n");
	if ( memory_size_to_free == 0x0 )
	{
		debug_print("ERROR: memory already freed\n");
		return;
	}
	memory_blocks_to_free = (memory_size_to_free / MEMORY_BLOCK_SIZE);
	for(c=memory_block;c<=memory_block+memory_blocks_to_free;c++)
	{
		memorybitmap[c] = 0x0;
		/*
		debug_print("Marking hole ");
		debug_print_uint32(c);
		debug_print("\n");
		*/
	}
	memorybitmaplist[memory_block] = 0x0;
}

