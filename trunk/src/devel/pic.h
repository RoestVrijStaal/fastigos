#define PIC1_CMD 0x20
#define PIC2_CMD 0xa0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xa1
#define PIC_ICW1 0x11
#define PIC_ICW4 0x01
#define PIC_EOI 0x20

void pic_init(void);
