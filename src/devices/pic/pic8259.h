#ifndef __PIC8259_DEVICE_H__
#define __PIC8259_DEVICE_H__

#include "../device.h"
#include "../../mmu/mmu.h"

__FOS_DEVICE_STATUS pic8259_init(void);
__FOS_DEVICE_STATUS pic8259_deinit(void);

#endif
