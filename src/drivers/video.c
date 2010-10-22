#include "video.h"
/*
http://www.osdever.net./documents/vga_ports.txt
*/

int8_t video_init()
{
	uint8_t	vgareg;

	debug_write_string("video_init()\n");

	// assume vga display
	video_regs.mem_offset = (uint16_t *)0xB8000;
	video_regs.color = 0x7;
	video_regs.x = 0;
	video_regs.y = 0;

	vgareg = inb(0x3cc);
	vgareg = vgareg && 0x2;
	outb(0x3c2, vgareg);

	/* block cursor */
	video_setcursor(0x0, 0xff);


	video_is_ramenable();
	outb(0x3c3, 0x1);
	video_is_ramenable();

	video_clear();

	debug_write_string("video_init() return\n");
	return 0;
}

uint8_t video_is_ramenable()
{
	debug_write_string("video_is_ramenable()\n");

	uint8_t	vgareg;

	vgareg = inb(0x3cc);
	debug_write_string("inb 0x3cc =");
	debug_write_uint8(vgareg);
	debug_write_string("\n");
	vgareg = vgareg >> 1;
	vgareg = vgareg && 0x1;
	debug_write_string("video_is_ramenable() return ");

	if ( vgareg == 0x1 )
	{
		debug_write_string("True\n");
		return TRUE;
	}
	debug_write_string("False\n");
	return FALSE;
}

void video_setcursor(uint8_t start, uint8_t end)
{
	debug_write_string("video_setcursor(");
	debug_write_uint8(start);
	debug_write_string(", ");
	debug_write_uint8(end);
	debug_write_string(")\n");

	start = start & 0x0f;
	end = end & 0x0f;

	outb(0x3d4, 0x0a);
	outb(0x3d5, start);
	outb(0x3d4, 0x0b);
	outb(0x3d5, end);
	debug_write_string("video_setcursor() return\n");
}

void video_clear()
{
	uint32_t c;
	uint16_t * memptr = video_regs.mem_offset;
	for(c=0;c<2000;c++)	// 80x25=2k
	{
		* memptr = (video_regs.color << 8) + 0x0;
		memptr++;
	}
}

int8_t vide_deinit()
{
	return 0;
}

int8_t video_seek(uint32_t position)
{
	return 0;
}

int8_t video_read(int8_t * byte)
{
	return 0;
}

int8_t video_write(int8_t byte)
{
	return 0;
}

