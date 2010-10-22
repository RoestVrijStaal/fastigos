#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "typedefs.h"
#include "debug.h"

void mem_init(uint32_t totalmemory);
uint32_t * malloc(uint32_t size);
#endif
