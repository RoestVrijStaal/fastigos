#ifndef __NULL_H__
#define __NULL_H__

#include "../devel/system.h"
#include "../devel/debug.h"

int8_t null_init();
int8_t null_deinit();
int8_t null_seek(uint32_t position);
int8_t null_read(int8_t * byte);
int8_t null_write(int8_t byte);

#endif
