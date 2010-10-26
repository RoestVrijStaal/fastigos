#ifndef __FDC_DEVICE_H__
#define __FDC_DEVICE_H__

#include "../device.h"
#include "../../mmu/mmu.h"

__FOS_DEVICE_STATUS fdc_init(void);
__FOS_DEVICE_STATUS fdc_deinit(void);

#endif
