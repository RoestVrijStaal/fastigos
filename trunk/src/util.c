#include "typedefs.h"
#include "video.h"
#include "config.h"

static __inline__ void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

static __inline__ uint8_t inb(uint16_t port)
{
   unsigned char ret;
   asm volatile ("inb %0, %1":"=a"(ret):"Nd"(port));
   return ret;
}

void cmosdump(void)
{
	uint8_t i,c;
	unsigned char cmosdata;

	video_printstring(7, "CMOS dump: \n");
	video_printstring(7, "     |-00-|-01-|-02-|-03-|-04-|-05-|-06-|-07-|-08-|-09-|-0a-|-0b-|-0c-|-0d-");
	c=0;
	for (i=0;i<=0x3f;i++)
	{
		if (c >= 14)
		{
			c = 0;
		}
		if ( c == 0 )
		{
			video_printstring(7, "|\n");
			video_print_uint8(7,i);
			video_printstring(7, " |");

		}
		c++;

		outb(CMOS_CMD, i);
		iowait;
		cmosdata = inb(CMOS_DATA);
		video_print_uint8(7, cmosdata);
		if ( c < 14 )
		{
			video_printstring(7, "|");
		}
	}
	video_printstring(7, "\n\n");
}

