#include "config.h"
#include "typedefs.h"
#include "video.h"
#include "pit.h"

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

volatile uint32_t	cursor_offset;

void video_init(void)
{
	uint8_t	*c;
	uint8_t *l;

	// the bootstrap saves here the cursor
	c = (uint8_t *)0x7C41;
	l = (uint8_t *)0x7C42;

	cursor_offset = SCREEN_COLS * *l;
	cursor_offset = cursor_offset + *c;
	cursor_offset = cursor_offset * 2;
}

void printk(char *string)
{
	video_printstring(7, "[-] ");
	video_printstring(7, string);
	video_printstring(7, "\n");
}

void video_clear(void)
{
	volatile char *video=(volatile char *)0xB8000;
	int c;

	for (c=0;c<=4000;c=c+2)
	{
		*video=(volatile char)0;
		video++;
		*video=(volatile char)7;
		video++;
	}
	cursor_offset = 0;
}
void video_scroll(void)
{
	volatile char *line=(volatile char *)0xB8000;
	volatile char *nextline=(volatile char *)0xB8000+(SCREEN_COLS*2);
	volatile char *lastline=(volatile char *)0xB8000+(SCREEN_COLS*(SCREEN_LINES-1))*2;

	unsigned int c;

	// roll up
	for (c=(SCREEN_COLS*2);c<=((SCREEN_COLS * SCREEN_LINES) *2);c++)
	{
		*line=*nextline;
		line++;
		nextline++;
	}
	// blank last line
	for (c=0;c<=SCREEN_COLS;c++)
	{
		*lastline=0;
		lastline++;
		*lastline=7;
		lastline++;
	}
}

void video_printc(int color, char *byte)
{
	volatile char *video=(volatile char *)0xB8000;

	video+=cursor_offset;
	if ( ( *byte >= 32 ) && ( *byte <= 126 ) )
	{ 
		*video=*byte;
		cursor_offset++;
		video++;
		*video=color;
		cursor_offset++;
	}
	else if ( *byte == '\n')
	{
		cursor_offset = (SCREEN_COLS *2) * (1 + cursor_offset / (SCREEN_COLS *2));
	}

	if ( cursor_offset >= ( ( SCREEN_COLS * (SCREEN_LINES) ) *2 ) )
	{
		video_scroll();
		//cursor_offset=( ( SCREEN_COLS * SCREEN_LINES ) *2 ) - ( SCREEN_COLS *2);
		cursor_offset=( SCREEN_COLS * (SCREEN_LINES-1) ) *2;
	}
}

void video_printstring(int color, char *string)
{

	while(*string!=0)
	{
		video_printc(color, (char *)string);
		string++;
	}
	video_setcursor();
}

void video_setcursor()
{
	volatile uint16_t	real_cursor_offset;
	volatile uint8_t	temp;

	real_cursor_offset = cursor_offset/2;
	outb(0x3d4, 0x0f);
	temp = (uint8_t)real_cursor_offset;
	outb(0x3d5, temp);
	outb(0x3d4, 0x0e);
	temp = real_cursor_offset >> 8;
	outb(0x3d5, temp);

	/*
	__asm__("mov AL, 0x0f");
	__asm__("mov DX, 0x3d4");
	__asm__("out DX, AL");
	
	__asm__("mov AX, [%0]"::"m"(real_cursor_offset));
	__asm__("mov DX, 0x3d5");
	__asm__("out DX, AL");

	__asm__("mov AL, 0x0e");
	__asm__("mov DX, 0x3d4");
	__asm__("out DX, AL");
	
	__asm__("mov AX, [%0]"::"m"(real_cursor_offset));
	__asm__("mov AL, AH");
	__asm__("mov DX, 0x3d5");
	__asm__("out DX, AL");
	*/
}

void video_print_uint32(int color, uint32_t number)
{
	static char hextable[] = "0123456789abcdef";
	uint8_t i;
	uint8_t c;
	uint8_t d;
	video_printstring(color, "0x");
	d = 28;
	for ( i=0; i<=7;i++)
	{
		c = number >> d & 0x0f;
		video_printc(color, hextable+c);
		d-=4;
	}
	video_setcursor();

}

void video_print_uint16(int color, uint16_t number)
{
	static char hextable[] = "0123456789abcdef";
	uint8_t i;
	uint8_t c;
	uint8_t d;
	video_printstring(color, "0x");
	d = 12;
	for ( i=0; i<=3;i++)
	{
		c = number >> d & 0x0f;
		video_printc(color, hextable+c);
		d-=4;
	}
	video_setcursor();

}

void video_print_uint8(int color, uint8_t number)
{
	static char hextable[] = "0123456789abcdef";
	uint8_t i;
	uint8_t c;
	uint8_t d;
	video_printstring(color, "0x");
	d = 4;
	for ( i=0; i<=1;i++)
	{
		c = number >> d & 0x0f;
		video_printc(color, hextable+c);
		d-=4;
	}
	video_setcursor();

}

