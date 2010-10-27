#include "cpu.h"

typedef struct cpuid_s
{
	uint8_t cpuid_support;
} cpuid;

cpuid cpu_info;

__FOS_DEVICE_STATUS cpu_init(void)
{
	// get cpuid information here
/*
1. Determine if the CPUID instruction is supported by modifying the ID flag in the EFLAGS register. If the ID flag cannot be
  modified, the processor cannot be identified using the CPUID instruction.
2. Execute the CPUID instruction with EAX equal to 80000000h. CPUID function 80000000h is used to determine if Brand
  String is supported. If the CPUID function 80000000h returns a value in EAX greater than 80000000h the Brand String
 feature is supported and software should use CPUID functions 80000002h through 80000004h to identify the processor.
3. If the Brand String feature is not supported, execute CPUID with EAX equal to 1. CPUID function 1 returns the processor
  signature in the EAX register, and the Brand ID in the EBX register bits 0 through 7. If the EBX register bits 0 through 7
 contain a non-zero value, the Brand ID is supported. Software should scan the list of Brand Ids (see Table 9) to identify the
processor.
4. If the Brand ID feature is not supported, software should use the processor signature (see Figure 2) in conjunction with the
  cache descriptors (see Table 7) to identify the processor.
*/
	// if cpu is not capable to run fastigOS, return _ERROR to halt the system
	return __FOS_DEVICE_STATUS_OK;
}
