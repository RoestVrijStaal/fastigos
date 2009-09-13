#include "devel/typedefs.h"

void *memset(void *s, uint8_t c, uint32_t size)
{
    char *p = (char *) s;
    while (size-- > 0)
        *p++ = c;
    return s;
}

