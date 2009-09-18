#include "typedefs.h"
#include "memory.h"
#include "mutex.h"


void *mutex_create()
{
	uint8_t *dir;
	dir = resb(1);
	*dir = 0;
	return dir;
}

void mutex_free(void *dir)
{
	freeb((uint32_t)dir, 1);
}

uint8_t mutex_try_lock(uint8_t *dir)
{
	__asm__ __volatile__("cli");
	if ( *dir == 0 )
	{
		*dir = 1;
		__asm__ __volatile__("sti");
		return MUTEX_OK;
	}
	else
	{
		__asm__ __volatile__("sti");
		return MUTEX_ERROR;
	}
}

uint8_t mutex_try_unlock(uint8_t *dir)
{
	__asm__ __volatile__("cli");
	if ( *dir == 1 )
	{
		*dir = 0;
		__asm__ __volatile__("sti");
		return MUTEX_OK;
	}
	else
	{
		__asm__ __volatile__("sti");
		return MUTEX_ERROR;
	}
}

