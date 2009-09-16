#define BREAKPOINT_INTEL __asm__("xchg BX, BX");
#define BREAKPOINT_ATT __asm__("xchg %bx, %bx");

#define CMOS_CMD 0x70
#define CMOS_DATA 0x71

#define CMOS_COMMAND_SECONDS 0x0
#define CMOS_COMMAND_SECONDS_ALARM 0x1
#define CMOS_COMMAND_MINUTE 0x2
#define CMOS_COMMAND_MINUTE_ALARM 0x3
#define CMOS_COMMAND_HOUR 0x4
#define CMOS_COMMAND_HOUR_ALARM 0x5
#define CMOS_COMMAND_DAYWEEK 0x6
#define CMOS_COMMAND_DAYMONTH 0x7
#define CMOS_COMMAND_MONTH 0x8
#define CMOS_COMMAND_YEAR 0x9
#define CMOS_COMMAND_SRA 0xa
#define CMOS_COMMAND_SRB 0xb
#define CMOS_COMMAND_SRC 0xc
#define CMOS_COMMAND_SRD 0xd
#define CMOS_COMMAND_POST_DIAGNOSTIC 0xe
#define CMOS_COMMAND_SHUTDOWN_STATUS 0xf
#define CMOS_COMMAND_FDC_DRIVE_TYPES 0x10
#define CMOS_COMMAND_HD_DRIVE_TYPES 0x12
#define CMOS_COMMAND_EQUIPMENT 0x14
#define CMOS_COMMAND_BASE_MEM_LOW_BYTE 0x15
#define CMOS_COMMAND_BASE_MEM_HIGH_BYTE 0x16
#define CMOS_COMMAND_BASE_MEM_EXTENDED_LOW_BYTE 0x17
#define CMOS_COMMAND_BASE_MEM_EXTENDED_HIGH_BYTE 0x18
#define CMOS_COMMAND_DISK0_TYPE 0x19
#define CMOS_COMMAND_DISK1_TYPE 0x1a
#define CMOS_COMMAND_CHECKSUM_CMOS_10_20_HIGH_BYTE 0x2e
#define CMOS_COMMAND_CHECKSUM_CMOS_10_20_LOW_BYTE 0x2f
#define CMOS_COMMAND_EXTENDED_MEMORY_LOW_BYTE 0x30
#define CMOS_COMMAND_EXTENDED_MEMORY_HIGH_BYTE 0x31
#define CMOS_COMMAND_BCD_CENTURY 0x32
#define CMOS_COMMAND_MISCELLANEOUS 0x33

#define IRQ_OFFSET 0x20

#define IRQ0 IRQ_OFFSET + 0
#define IRQ_TIMER IRQ0

#define IRQ6 IRQ_OFFSET + 6
#define IRQ_FDC IRQ6

#define iowait __asm__ __volatile__("jmp next; next: nop");

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

