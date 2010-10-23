#ifndef __FDC_DEVICE_H__
#define __FDC_DEVICE_H__

#include "../device.h"

__FOS_DEVICE_STATUS fdc_init(void);
__FOS_DEVICE_STATUS fdc_deinit(void);
__FOS_DEVICE_STATUS fdc_read(uint8_t * byte);
__FOS_DEVICE_STATUS fdc_write(uint8_t byte);
__FOS_DEVICE_STATUS fdc_seek(uint32_t offset);
__FOS_DEVICE_STATUS fdc_size(uint32_t * size);
#endif
