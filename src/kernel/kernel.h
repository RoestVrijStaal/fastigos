#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "../common/typedefs.h"
#include "../debug/debug.h"
#include "../mmu/mmu.h"

// devices
#include "../devices/device.h"
#include "../devices/null.h"
#include "../devices/video/vga.h"
#include "../devices/disk/fdc.h"

__attribute__((noreturn)) void kernel_main();
__attribute__((noreturn)) void kernel_die(char *message);

#endif
