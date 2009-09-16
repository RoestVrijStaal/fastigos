#include "config.h"
#include "typedefs.h"
#include "video.h"
#include "segments.h"
#include "pic.h"
#include "pit.h"


uint16_t ticks;
uint32_t secs;
extern uint32_t	cursor_offset;


static inline void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

void timer_phase()
{
	__asm__ __volatile__("cli");
	uint16_t divisor = 0xffff;
	outb(IO_PIT_COMMAND, 0x36);             /* Set our command byte 0x36 */
	outb(IO_PIT0, divisor & 0xFF);   /* Set low byte of divisor */
	outb(IO_PIT0, divisor >> 8);     /* Set high byte of divisor */
	__asm__ __volatile__("sti");
}
void clock_tick()
{
	uint32_t saved_cursor_offset = cursor_offset;
	cursor_offset = (SCREEN_COLS*2) - (10*2);
	video_print_uint32(7, secs);
	cursor_offset = saved_cursor_offset;
	video_setcursor();
}

void clock_handler(void)
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	ticks++;
	if ( ticks % 18 == 0 )
	{
		secs++;
		clock_tick();
	}

	outb(PIC1_CMD, PIC_EOI);
	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret"); /* BLACK MAGIC! */
}

void pit_init(void)
{
	ticks=0;
	secs=0;

	timer_phase();
	install_idt_handler(IRQ_TIMER, (uint32_t)clock_handler);
	video_printstring(7, "Ok\n");
}


/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int delay)
{
    unsigned long eticks;

    eticks = ticks + delay;
    while(ticks < eticks);
}

