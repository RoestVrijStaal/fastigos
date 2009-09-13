#include "devel/config.h"
#include "devel/typedefs.h"
#include "devel/video.h"

static inline void outb(uint16_t port, uint8_t data)
{
	// intel syntax!
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

void pic_init(void)
{
	video_printstring(7, "Id, ");
	__asm__ __volatile("cli");
	video_printstring(7, "cfg irq's, ");
	// ICW1
	video_printstring(7, "ICW1, ");
	outb(PIC1_CMD, PIC_ICW1);
	outb(PIC2_CMD, PIC_ICW1);

	// ICW2
	video_printstring(7, "ICW2, ");
	outb(PIC1_DATA, IRQ_OFFSET);
	outb(PIC2_DATA, IRQ_OFFSET+8);

	// ICW3
	video_printstring(7, "ICW3, ");
	outb(PIC1_DATA, 0x4);
	outb(PIC2_DATA, 0x2);

	// ICW4
	video_printstring(7, "ICW4, ");
	outb(PIC1_DATA, PIC_ICW4);
	outb(PIC2_DATA, PIC_ICW4);

	//outb(PIC1_DATA, 0xfb);
	//outb(PIC2_DATA, 0xff);

	video_printstring(7, "Ie.\n");
	__asm__ __volatile("sti");

}
