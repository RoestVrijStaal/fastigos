#include "null.h"

int8_t null_init()
{
	debug_write_string("null_init() begin\n");
	debug_write_string("null_init() end\n");
	return 0;
}

int8_t null_deinit()
{
	debug_write_string("null_deinit() begin\n");
	debug_write_string("null_deinit() end\n");
	return 0;
}

int8_t null_seek(uint32_t position)
{
	debug_write_string("null_seek( ");
	debug_write_uint32(position);
	debug_write_string(" ) begin\n");
	debug_write_string("null_seek() end\n");
	return 0;
}

int8_t null_read(int8_t * byte)
{
	debug_write_string("null_read( ptr: 0x");
	debug_write_uint32(byte);
	debug_write_string(" ) begin\n");
	byte = 0;
	debug_write_string("null_read() end\n");
	return 0;
}

int8_t null_write(int8_t byte)
{
	debug_write_string("null_write( ");
	debug_write_string(" val: ");
	debug_write_uint8(byte);
	debug_write_string(" ) begin\n");
	byte = 0;
	debug_write_string("null_write() end\n");
	return 0;
}

