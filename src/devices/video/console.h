#ifndef __CONSOLE_DEVICE_H__
#define __CONSOLE_DEVICE_H__

#include "vga.h"

__FOS_DEVICE_STATUS console_init(void);
__FOS_DEVICE_STATUS console_deinit(void);
void printf(char *format, ...);

#endif
