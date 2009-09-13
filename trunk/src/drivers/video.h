#define SCREEN_COLS	80
#define SCREEN_LINES	25

void video_clear(void);
void video_scroll(void);
void video_printc(int color, char *byte);
void video_printstring(int color, const char *string);
void video_setcursor();
void printk(char *string);
void video_print_uint8(int color, uint8_t number);
void video_print_uint16(int color, uint16_t number);
void video_print_uint32(int color, uint32_t number);
