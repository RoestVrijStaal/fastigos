#include "config.h"
#include "typedefs.h"
#include "video.h"
#include "pit.h"

// fastigOS driver interface
volatile uint32_t init=(uint32_t)&video_init;
volatile uint32_t read=(uint32_t)&video_read;
volatile uint32_t write=(uint32_t)&video_write;
volatile uint32_t fini=(uint32_t)&video_fini;

volatile uint32_t	cursor_offset;

void video_fini(void)
{
}

uint8_t *video_read(void)
{
	return 0x0;
}

void video_write(uint8_t *string)
{
	video_printstring(7, (const char *)string);
}

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

	if ( cursor_offset >= ( ( SCREEN_COLS * SCREEN_LINES ) *2 ) )
	{
		video_scroll();
		cursor_offset=( ( SCREEN_COLS * SCREEN_LINES ) *2 ) - ( SCREEN_COLS *2);
	}

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
}

void video_printstring(int color, const char *string)
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

	real_cursor_offset = cursor_offset/2;

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

