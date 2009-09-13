#define FDC_OK					0
#define FDC_ERROR				1
#define FDC_TIMEOUT				-1


#define FDC_SRA					0x3f0
#define FDC_SRB					0x3f1
#define FDC_DOR					0x3f2
#define FDC_TDR					0x3f3		// really used?
#define FDC_MSR					0x3f4
#define FDC_DSR					0x3f4
#define FDC_DATA				0x3f5
#define FDC_DIR					0x3f6
#define FDC_CCR					0x3f7

#define FDC_COMMAND_SPECIFY			0x3
#define FDC_COMMAND_SENSE_INTERRUPT_STATUS	0x8
#define FDC_COMMAND_VERSION			0x10

#define FDC_VERSION_IS_STANDARD			0x0
#define FDC_VERSION_IS_ENHANCED			0x90

#define	FDC_D0					0x1c
#define	FDC_D1					0x2d
#define	FDC_D2					0x4e
#define	FDC_D3					0x8f

uint8_t fdc_identify(void);
uint8_t fdc_reset(void);
uint8_t fdc_motoron(uint8_t drive);
uint8_t fdc_motoroff();

