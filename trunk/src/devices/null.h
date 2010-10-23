#ifndef __NULL_DEVICE_H__
#define __NULL_DEVICE_H__

#include "device.h"

__FOS_DEVICE_STATUS null_init(void);
__FOS_DEVICE_STATUS null_read(uint8_t * byte);
__FOS_DEVICE_STATUS null_write(uint8_t byte);
__FOS_DEVICE_STATUS null_seek(uint32_t offset);
__FOS_DEVICE_STATUS null_size(uint32_t * size);

#endif
