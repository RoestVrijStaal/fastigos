#ifndef __MMU_H__
#define __MMU_H__

#include "../common/typedefs.h"
#include "../debug/debug.h"
#include "../kernel/kernel.h"

#define MEMORY_BLOCK_SIZE 4096
#define MEMORY_FREE_BEGIN 0x400000
#define MEMORY_FREE_SIZE  0x400000
#define MEMORY_MAP_OFFSET 0x100000

#define PIC1_CMD 0x20
#define PIC_ICW1 0x11
#define PIC2_CMD 0xa0
#define PIC1_DATA 0x21
#define PIC_ICW4 0x01
#define PIC2_DATA 0xa1
#define PIC_EOI 0x20

struct segment_descriptor
{
	uint16_t	limit_15_0;
	uint16_t	base_addr_15_0;
	uint8_t		base_addr_23_16;
	uint8_t		type:4;			/* 0000 Reserved
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
						// 1111 32bit Trap Gate */
	uint8_t		s:1;			/* Type of descriptor (1=data/0=sys) */
	uint8_t		dpl:2;			/* Privilege level */
	uint8_t		p:1;			/* Segment is present */
	uint8_t		limit_19_16:4;
	uint8_t		avl:1;			/* Available */
	uint8_t		l:1;			/* 64bit segment */
	uint8_t		db:1;			/* Operation size */
	uint8_t		g:1;			/* Granularity */
	uint8_t		base_addr_31_24;
} __attribute__((__packed__));

#define NULL_DESCRIPTOR						\
	((struct segment_descriptor)				\
	{							\
		.limit_15_0	= 0,				\
		.limit_19_16	= 0,				\
		.base_addr_15_0	= 0,				\
		.base_addr_23_16= 0,				\
		.base_addr_31_24= 0,				\
		.type 		= 0,		 		\
		.s 		= 0, 				\
		.dpl 		= 0,				\
		.p 		= 0,				\
		.avl 		= 0,				\
		.l 		= 0,				\
		.db 		= 0,				\
		.g 		= 0				\
	})

#define BUILD_4K_CS_RE_SEG_DESC(base_addr, limit)		\
	((struct segment_descriptor)				\
	{							\
		.limit_15_0	= (limit) & 0xffff,		\
		.limit_19_16	= ((limit) >> 16 ) & 0xf,	\
		.base_addr_15_0	= (base_addr) & 0xffff,		\
		.base_addr_23_16= ((base_addr) >> 16 ) & 0xff,	\
		.base_addr_31_24= ((base_addr) >> 24 ) & 0xff,	\
		.type 		= 0x9,		 		\
		.s 		= 1, 				\
		.dpl 		= 0,				\
		.p 		= 1,				\
		.avl 		= 0,				\
		.l 		= 0,				\
		.db 		= 1,				\
		.g 		= 1				\
	})

#define BUILD_4K_SS_RW_SEG_DESC(base_addr, limit)		\
	((struct segment_descriptor)				\
	{							\
		.limit_15_0	= (limit) & 0xffff,		\
		.limit_19_16	= ((limit) >> 16 ) & 0xf,	\
		.base_addr_15_0	= (base_addr) & 0xffff,		\
		.base_addr_23_16= ((base_addr) >> 16 ) & 0xff,	\
		.base_addr_31_24= ((base_addr) >> 24 ) & 0xff,	\
		.type 		= 0x2,		 		\
		.s 		= 1, 				\
		.dpl 		= 0,				\
		.p 		= 1,				\
		.avl 		= 0,				\
		.l 		= 0,				\
		.db 		= 1,				\
		.g 		= 1				\
	})

#define BUILD_4K_DS_RW_SEG_DESC(base_addr, limit)		\
	((struct segment_descriptor)				\
	{							\
		.limit_15_0	= (limit) & 0xffff,		\
		.limit_19_16	= ((limit) >> 16 ) & 0xf,	\
		.base_addr_15_0	= (base_addr) & 0xffff,		\
		.base_addr_23_16= ((base_addr) >> 16 ) & 0xff,	\
		.base_addr_31_24= ((base_addr) >> 24 ) & 0xff,	\
		.type 		= 0x2,		 		\
		.s 		= 1, 				\
		.dpl 		= 0,				\
		.p 		= 1,				\
		.avl 		= 0,				\
		.l 		= 0,				\
		.db 		= 1,				\
		.g 		= 1				\
	})

#define BUILD_IDT_DESCRIPTOR(addr)					\
	((struct idt_descriptor)					\
	{								\
		.offset_0_15 = (addr) & 0xffff,				\
		.segment_selector = 0x8,				\
		.reserved = 0x0,					\
		.reserved2 = 0x8e,					\
		.offset_16_31 = ((addr) >> 16 ) & 0xffff		\
	})

struct gdtInfo
{
	uint16_t	size;
	uint32_t	addr;
} __attribute__((__packed__));

struct idtInfo
{
	uint16_t	size;
	uint32_t	addr;
} __attribute__((__packed__));

struct idt_descriptor
{
	uint16_t	offset_0_15;
	uint16_t	segment_selector;	/* 0x8 normaly */
	uint8_t		reserved;		/* 00000000b */
	uint8_t		reserved2;		/* 01110001b */
	uint16_t	offset_16_31;
} __attribute__((__packed__));

__attribute__((noreturn)) void mmu_init(void);
void * kmalloc(uint32_t size);
void kfree(void * addr);
void dump_memory_map();
#endif

