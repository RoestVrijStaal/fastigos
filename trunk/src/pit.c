#include "config.h"
#include "typedefs.h"
#include "video.h"
#include "segments.h"
#include "pic.h"
#include "pit.h"


uint32_t ticks;
uint32_t secs;
uint16_t msecs;

extern uint32_t	cursor_offset;


static inline void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

void timer_phase()
{
	uint16_t divisor;

	__asm__ __volatile__("cli");
	divisor = 0xffff / PIT_DIVISOR;
/*
D7 SC1
D6 SC2
	00 = first counter (0)
	01 = second counter (1)
	10 = thirth counter (2)
D5 RW1
D4 RW0
	00 = Counter latch
	01 = RW LSB only
	10 = RW MSB only
	11 = RW LSB first MSB later
D3 M2 Select mode
D2 M1 Select mode
D1 M0 Select mode
	000 = Interrupt on terminal count
	001 = Hard triggered one shot
	010 = Rate generator
	x11 = Square wave generator (011 or 111)
	100 = Software triggered storbe
	101 = Hardware triggered storbe
D0 BCD (0=binary,1=BCD)
 */
	// 0x36b = 00 11 011 0b
	outb(IO_PIT_COMMAND, 0x34);             /* Set our command byte 0x36 */
	outb(IO_PIT0, divisor & 0xff);   	/* Set low byte of divisor */
	outb(IO_PIT0, divisor >> 8);     	/* Set high byte of divisor */

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
	if ( ticks % 2 == 0 )
	{
		msecs++;
	}
	if ( ticks % (int)(18.2 * PIT_DIVISOR) == 0 )
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
	msecs=0;

	install_idt_handler(IRQ_TIMER, (uint32_t)clock_handler);
	timer_phase();
	video_printstring(7, "Ok\n");
}


/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int delay)
{
    uint32_t eticks;

    eticks = msecs + delay;
    while(msecs < eticks);
}

