#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "typedefs.h"
#include "debug.h"

#define video_mor_read	0x3cc
#define video_mor_write	0x3c2

struct video_s
{
	uint16_t *mem_offset;
	uint8_t color;
	uint8_t x;
	uint8_t y;
};
struct video_s video_regs;


void video_setcursor(uint8_t start, uint8_t end);
uint8_t video_is_ramenable();

void video_clear();

int8_t video_init();
int8_t video_deinit();
int8_t video_seek(uint32_t position);
int8_t video_read(int8_t * byte);
int8_t video_write(int8_t byte);

#endif
