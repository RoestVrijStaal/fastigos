#define BREAKPOINT_INTEL __asm__("xchg BX, BX");
#define BREAKPOINT_ATT __asm__("xchg %bx, %bx");

#define PIC1_CMD 0x20
#define PIC2_CMD 0xa0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1
#define PIC_ICW1 0x11
#define PIC_ICW4 0x01
#define PIC_EOI 0x20

#define IO_PIT0 0x40
#define IO_PIT1 0x41
#define IO_PIT2 0x42
#define IO_PIT_COMMAND 0x43

#define IRQ_OFFSET 0x20

#define IRQ0 IRQ_OFFSET + 0
#define IRQ_TIMER IRQ0

#define IRQ6 IRQ_OFFSET + 6
#define IRQ_FDC IRQ6

// http://en.wikipedia.org/wiki/Interrupt_request

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
		.g 		= 0				\
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

