#include "typedefs.h"
#include "video.h"
#include "rtc.h"
#include "config.h"

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

void rtc_init(void)
{
	uint8_t equipment;

	outb(CMOS_CMD, 0x14);
	iowait;
	equipment = inb(CMOS_DATA);
	video_print_uint8(7, equipment);
	video_printstring(7,"TODO\n");
}

