#include "null.h"

__FOS_DEVICE_STATUS null_init(void)
{
	debug_print("null_init()\n");
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS null_deinit(void)
{
	debug_print("null_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS null_read(uint8_t * byte)
{
	*byte = 0x0;
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS null_write(uint8_t byte)
{
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS null_seek(uint32_t offset)
{
	return __FOS_DEVICE_STATUS_ERROR;
}

__FOS_DEVICE_STATUS null_size(uint32_t * size)
{
	return __FOS_DEVICE_STATUS_ERROR;
}
