#define IO_PIT0 0x40
#define IO_PIT1 0x41
#define IO_PIT2 0x42
#define IO_PIT_COMMAND 0x43

#define PIT_DIVISOR 110

void pit_init(void);
void timer_wait(int delay);
