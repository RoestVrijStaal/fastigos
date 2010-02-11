#include "video.h"
/*
http://www.osdever.net./documents/vga_ports.txt
*/
int8_t video_init()
{
	uint8_t	vgareg;

	debug_write_string("video_init()\n");

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
	/* big shit */
}
