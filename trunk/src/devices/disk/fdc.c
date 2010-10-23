#include "fdc.h"

#define FDC_ERROR_TIMEOUT 	-1
#define FDC_OK		 	 0

enum FDCRegisters
{
   FDC_STATUS_REGISTER_A                = 0x3F0, /* read-only */
   FDC_STATUS_REGISTER_B                = 0x3F1, /* read-only */
   FDC_DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   FDC_TAPE_DRIVE_REGISTER              = 0x3F3,
   FDC_MAIN_STATUS_REGISTER             = 0x3F4, /* read-only */
   FDC_DATARATE_SELECT_REGISTER         = 0x3F4, /* write-only */
   FDC_DATA_FIFO                        = 0x3F5,
   FDC_DIGITAL_INPUT_REGISTER           = 0x3F7, /* read-only */
   FDC_CONFIGURATION_CONTROL_REGISTER   = 0x3F7  /* write-only */
};

enum FloppyCommands
{
   FDC_READ_TRACK =                 2,	/* generates IRQ6 */
   FDC_SPECIFY =                    3,      /* set drive parameters */
   FDC_SENSE_DRIVE_STATUS =         4,
   FDC_WRITE_DATA =                 5,      /* write to the disk */
   FDC_READ_DATA =                  6,      /* read from the disk */
   FDC_RECALIBRATE =                7,      /* seek to cylinder 0 */
   FDC_SENSE_INTERRUPT =            8,      /* ack IRQ6, get status of last command */
   FDC_WRITE_DELETED_DATA =         9,
   FDC_READ_ID =                    10,	/* generates IRQ6 */
   FDC_READ_DELETED_DATA =          12,
   FDC_FORMAT_TRACK =               13,    
   FDC_SEEK =                       15,     /* seek both heads to cylinder X */
   FDC_VERSION =                    16,	/* used during initialization, once */
   FDC_SCAN_EQUAL =                 17,
   FDC_PERPENDICULAR_MODE =         18,	/* used during initialization, once, maybe */
   FDC_CONFIGURE =                  19,     /* set controller parameters */
   FDC_LOCK =                       20,     /* protect controller params from a reset */
   FDC_VERIFY =                     22,
   FDC_SCAN_LOW_OR_EQUAL =          25,
   FDC_SCAN_HIGH_OR_EQUAL =         29
};

void fdc_outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

uint8_t fdc_inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

/* sendbyte() routine from intel manual */
void fdc_send(int byte)
{
   volatile int msr;
   int tmo;
   
   for (tmo = 0;tmo < 128;tmo++) {
      msr = fdc_inb(FDC_MAIN_STATUS_REGISTER);
      if ((msr & 0xc0) == 0x80) {
	 fdc_outb(FDC_DATA_FIFO,byte);
	 return;
      }
      fdc_inb(0x80);   /* delay */
   }
}

/* getbyte() routine from intel manual */
int fdc_get()
{
   volatile int msr;
   int tmo;
   
   for (tmo = 0;tmo < 128;tmo++) {
      msr = fdc_inb(FDC_MAIN_STATUS_REGISTER);
      if ((msr & 0xd0) == 0xd0) {
	 return fdc_inb(FDC_DATA_FIFO);
      }
      fdc_inb(0x80);   /* delay */
   }
   return -1;   /* read timeout */
}


__FOS_DEVICE_STATUS fdc_init(void)
{
	debug_print("fdc_init()\n");

	/* get type of fdc */
	fdc_send(FDC_VERSION);
	uint8_t type = fdc_get();
	
	debug_print_uint8(type);
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS fdc_deinit(void)
{
	debug_print("fdc_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}


