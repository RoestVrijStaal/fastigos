#include "vga.h"

#define MOR_R	0x03cc
#define MOR_W	0x03c2
#define ISR0_R	0x03c2
//#define ISR1_R	0x03?a
#define FCR_R	0x3ca
//#define FCR_W		0x3?a
#define FVSER_R	0x3c3
#define FVSER_W	0x3c3

// vga registers
#define VGA_CRT_CR_COMMAND	0x3d4	// CRT control register command
#define VGA_CRT_CR_RW		0x3d5	// CRT control register

// vga commands (CRT Control Registers)
#define VGA_SVB	0x15		// start vertical blanking
#define VGA_EVB	0x16		// end vertical blanking

#define VGA_CSR	0x0a		// cursor start register
#define VGA_CER	0x0b		// cursor end register
#define VGA_CLH	0x0e		// cursor location high
#define VGA_CLL	0x0f		// cursor location low

uint16_t * vga_buffer;
uint16_t * vga_offset;
uint8_t vga_textmode_current_color;

void vga_outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

uint8_t vga_inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

void vga_getcursor(uint16_t * position)
{
	uint8_t high;
	uint8_t low;
	__asm__ __volatile__("cli");
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CLH);
	high = vga_inb(VGA_CRT_CR_RW);
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CLL);
	low = vga_inb(VGA_CRT_CR_RW);
	__asm__ __volatile__("sti");
	*position = (uint16_t)((high << 8) + low); 
}

void vga_setcursor(uint16_t position)
{
	uint8_t high = (uint8_t)(position >> 8);
	uint8_t low = (uint8_t)position;

	__asm__ __volatile__("cli");
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CLH);
	vga_outb(VGA_CRT_CR_RW, high);
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CLL);
	vga_outb(VGA_CRT_CR_RW, low);
	__asm__ __volatile__("sti");

}

void vga_setcursor_shape(uint8_t start, uint8_t end)
{
	start = start & 0x0f;
	end = end & 0x0f;

	__asm__ __volatile__("cli");
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CSR);
	vga_outb(VGA_CRT_CR_RW, start);
	vga_outb(VGA_CRT_CR_COMMAND, VGA_CER);
	vga_outb(VGA_CRT_CR_RW, end);
	__asm__ __volatile__("sti");
}

__FOS_DEVICE_STATUS vga_init(void)
{
	debug_print("vga_init()\n");
	// set screen buffer
	vga_buffer = 0xb8000;
	// set cursor shape	
	vga_setcursor_shape(0x0, 0xf);

	// set new text color
	vga_textmode_current_color = 0x7;
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS vga_deinit(void)
{
	debug_print("vga_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS vga_read(uint8_t * byte)
{
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS vga_write(uint8_t byte)
{
	uint16_t position;
	vga_getcursor(&position);
	uint16_t * curdata;
	curdata = vga_buffer + position;
	*curdata = (vga_textmode_current_color << 8 ) + byte;
	position++;
	vga_seek(position);
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS vga_seek(uint32_t offset)
{
	// screen is a circular buffer
	if ( offset > ((80*25)-1) )
	{
		offset = (offset % (80*25));
	}
	vga_setcursor((uint16_t)offset);
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS vga_size(uint32_t * size)
{
	//@todo get hardware capabilities
	*size = 80*25;
	return __FOS_DEVICE_STATUS_OK;
}

