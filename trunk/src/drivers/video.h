#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "typedefs.h"
#include "debug.h"

#define video_mor_read	0x3cc
#define video_mor_write	0x3c2

int8_t video_init();
void video_setcursor(uint8_t start, uint8_t end);
uint8_t video_is_ramenable();

void video_clear();

#endif
