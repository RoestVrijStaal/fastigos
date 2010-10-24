#include "console.h"

__FOS_DEVICE_STATUS console_init(void)
{
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS console_deinit(void)
{
	return __FOS_DEVICE_STATUS_OK;
}

void _printf_string(char *text)
{
	while (*text != '\0')
	{
		vga_write(*text);
		text++;
	}
}
//@todo vararg is a shit
void printf(char *format, ...)
{
	_printf_string(format);
}
