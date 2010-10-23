#ifndef __VGA_DEVICE_H__
#define __VGA_DEVICE_H__

#include "../device.h"

__FOS_DEVICE_STATUS vga_init(void);
__FOS_DEVICE_STATUS vga_deinit(void);
__FOS_DEVICE_STATUS vga_read(uint8_t * byte);
__FOS_DEVICE_STATUS vga_write(uint8_t byte);
__FOS_DEVICE_STATUS vga_seek(uint32_t offset);
__FOS_DEVICE_STATUS vga_size(uint32_t * size);
#endif
