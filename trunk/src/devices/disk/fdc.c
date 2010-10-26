#include "fdc.h"
// http://www.osdever.net/tutorials/view/detecting-floppy-drives
// http://wiki.osdev.org/Floppy_Disk_Controller#The_Floppy_Subsystem_is_Ugly
// http://www.disy.cse.unsw.edu.au/lxr/source/drivers/block/floppy.c?v=linux-2.6.32
// http://koders.com/c/fid051291340B94EC7F5D1A38EF6843466C0B07627B.aspx?s=fdc#L7
// http://bos.asmhackers.net/docs/floppy/snippet_5/FLOPPY.ASM

/*
 * general return codes by fdc_ functions
 */
#define FDC_ERROR	 	-1
#define FDC_OK		 	 0

/*
 * the IRQ of fdc controller
 */
#define FDC_DEFAULT_IRQ		32+6	// default 32 int's + 6 irq's

/*
 * controllers can be handled by this controller
 */
#define FDC_8272A_CONTROLLER 0x80
#define FDC_ENHANCED_CONTROLLER 0x90

/*
 * drive struct descriptor
 */
typedef struct fdcDrive_s
{
	uint8_t type;
	int8_t motor;
	uint8_t track;
} fdcDrive;

/*
 * controller struct descriptor
 */
struct fdcStatus_s
{
	uint8_t type;
	uint8_t interrupt;
	fdcDrive mdrive;
	fdcDrive sdrive;
	uint8_t st0;
};
struct fdcStatus_s fdcStatus;

char *drive_types[6] = { "free", "360kb 5.25in", "1.2mb 5.25in", "720kb 3.5in", "1.44mb 3.5in", "2.88mb 3.5in" };

enum FDCRegisters
{
   FDC_STATUS_REGISTER_A                = 0x3F0, /* read-only */
   FDC_STATUS_REGISTER_B                = 0x3F1, /* read-only */
   FDC_DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   FDC_TAPE_DRIVE_REGISTER              = 0x3F3,
   FDC_MAIN_STATUS_REGISTER             = 0x3F4, /* read-only RQM,DIO,NON-DMA,CMD-BUSY,DRV3-BUSY,DRV2-BUSY,DRV1-BUSY,DRV0-BUSY */
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
   FDC_SCAN_HIGH_OR_EQUAL =         29,
   FDC_READ_ID_MFM =                74,	/* generates IRQ6 */
};

/*
 * tool function required by fdc_send
 */
void fdc_outb(uint16_t port, uint8_t data)
{
	__asm__ __volatile__("out %0, %b1"::"d"(port),"a"(data));
}

/*
 * tool function required by fdc_get
 */
uint8_t fdc_inb(uint16_t port)
{
	unsigned char ret;
	__asm__ __volatile__ ("inb %0, %1":"=a"(ret):"Nd"(port));
	return ret;
}

/*
 * sendbyte() routine from intel manual
 */
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

/*
 * getbyte() routine from intel manual
 */
uint8_t fdc_get()
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
   return FDC_ERROR;   /* read timeout */
}

/*
 * basic irq handler for controller
 */
void fdc_irq_handler()
{
	__asm__ __volatile__("pusha");
	__asm__ __volatile__("cli");

	fdcStatus.interrupt = 0x1;
	debug_print("fdc -> recived IRQ\n");
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC1_CMD),"a"((uint8_t)PIC_EOI));
	__asm__ __volatile__("outb %0, %b1"::"d"((uint16_t)PIC2_CMD),"a"((uint8_t)PIC_EOI));

	__asm__ __volatile__("sti");
	__asm__ __volatile__("popa; leave; iret");		// black magic
}

/*
 * tool function convert drive numeric to controller id value
 */
int8_t fdc_tool_get_drive(uint8_t what_drive)
{
	uint8_t drive = 0x0;
	if ( what_drive == 0x0 )
	{
		drive = 0x0c;
	}
	else if ( what_drive == 0x1 )
	{
		drive = 0x0d;
	}
	else
	{
		debug_print("fdc -> unrecognized drive ");
		debug_print_uint8(what_drive);
		debug_print("\n");
		return FDC_ERROR;
	}
	return drive;
}

/*
 * detect controller and return if this driver is capable to use it
 */
int8_t fdc_detect_controller(void)
{
	/* get type of fdc */
	fdc_send(FDC_VERSION);
	fdcStatus.type = fdc_get();
	if ( FDC_ENHANCED_CONTROLLER == fdcStatus.type )
	{
		debug_print("fdc -> enhanced controller found\n");
	}
	else if ( FDC_8272A_CONTROLLER == fdcStatus.type )
	{
		debug_print("fdc -> 8272A/765A controller found\n");
	}
	else
	{
		debug_print("fdc -> unknown controller found!\n");
		return FDC_ERROR;
	}
	return FDC_OK;
}

/*
 * detect drives attached to this controller
 */
int8_t fdc_detect_floppy_drives(void)
{
	uint8_t data;
	// get data from cmos
	fdc_outb(0x70, 0x10);
	data = fdc_inb(0x71);
	fdcStatus.mdrive.type = data >> 4;
	fdcStatus.sdrive.type = data & 0xf;
	debug_print("fdc -> master: ");

	debug_print(drive_types[fdcStatus.mdrive.type]);
	debug_print("\n");
	debug_print("fdc -> slave: ");
	debug_print(drive_types[fdcStatus.sdrive.type]);
	debug_print("\n");
	if ( (fdcStatus.mdrive.type == 0x0) && (fdcStatus.sdrive.type == 00 ))
	{
		return FDC_ERROR;
	}
	return FDC_OK;
}

/*
 * start motor of selected drive
 */
int8_t fdc_motorOn(uint8_t what_drive)
{
	debug_print("fdc_motorOn(");
	debug_print_uint8(what_drive);
	debug_print(")\n");
	uint8_t drive;
	if ( what_drive == 0 )
	{
		drive = 0x1c + 	fdcStatus.sdrive.motor;
		// mark motor as ON
		fdcStatus.mdrive.motor = 0x10;
	}
	else if ( what_drive == 1 )
	{
		drive = 0x2d + 	fdcStatus.mdrive.motor;
		// mark motor as ON
		fdcStatus.sdrive.motor = 0x20;
	}
	else
	{
		return FDC_ERROR;
	}
	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, drive);
	sleep(100);
	return FDC_OK;
}

/*
 * stop motor of selected drive
 */
int8_t fdc_motorOff(uint8_t what_drive)
{
	debug_print("fdc_motorOff(");
	debug_print_uint8(what_drive);
	debug_print(")\n");
	// the id drive codes of DOR differs from rest of controller
	uint8_t drive;
	if ( what_drive == 0 )
	{
		drive = 0x0c + fdcStatus.sdrive.motor;
		// mark motor as OFF
		fdcStatus.mdrive.motor = 0x00;
	}
	else if ( what_drive == 1 )
	{
		drive = 0x0d + 	fdcStatus.mdrive.motor;
		// mark motor as OFF
		fdcStatus.sdrive.motor = 0x00;
	}
	else
	{
		return FDC_ERROR;
	}
	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, drive);
	sleep(100);
	return FDC_OK;
}

/*
 * software reset controller
 */
int8_t fdc_reset()
{
        debug_print("reset_fdc() begin\n");
        fdcStatus.interrupt = 0x0;
        install_idt_handler(FDC_DEFAULT_IRQ, (uint32_t)fdc_irq_handler);
        fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, 0x00); /*disable controller*/
        fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, 0x0c); /*enable controller*/
        debug_print("reset_fdc() waiting int...\n");
	int32_t c = 100;
	// timeout of 100
        while(c > 0)
	{
		c--;
		if ( 0x1 == fdcStatus.interrupt)
		{
			c=0;
		}
		sleep(1);
	}
	if ( 0x1 != fdcStatus.interrupt)
	{
	        debug_print("reset_fdc() TIMEOUT\n");
		return FDC_ERROR;
	}
        debug_print("reset_fdc() int completed\n");
        debug_print("reset_fdc() return\n");
	return FDC_OK;
}

/*
 * recalibrate selected drive
 */
int8_t fdc_recalibrate(uint8_t what_drive)
{
        uint8_t retries;
        debug_print("fdc_recalibrate(");
        debug_print_uint8(what_drive);
        debug_print(")\n");
	// get drive id
        int8_t drive = fdc_tool_get_drive(what_drive);
	if ( FDC_ERROR == drive )
	{
		return FDC_ERROR;
	}

        uint8_t recalibration = 0;
	// install irq handler
        fdcStatus.interrupt = 0x0;
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
	//@todo this not work, review datasheet page 42 for compilant recalibrate
        install_idt_handler(FDC_DEFAULT_IRQ, (uint32_t)fdc_irq_handler);
	uint8_t track;
        for (retries = 0; retries < 14; retries++)
        {
		// tell controller recalibrate command
                fdc_send(FDC_RECALIBRATE);
                fdc_send(drive);
		// wait arbitrary time
                sleep(10);
		// get controller status
                fdc_send(FDC_SENSE_INTERRUPT);
                fdcStatus.st0 = fdc_get();
		track = fdc_get();
                debug_print("fdc -> st0: ");
                debug_print_uint8(track);
                debug_print("\n");

                if (!(fdcStatus.st0 & 0x10))
                {
                        recalibration = 1;
                        break;
                }
        }
        if (recalibration == 0)
        {
                debug_print("fdc -> recalibration timeout\n");
		return FDC_ERROR;
        }
        debug_print("fdc -> calibration result: drive ");
        debug_print_uint8(drive);
        debug_print(" sr0: ");
        debug_print_uint8(fdcStatus.st0);
        debug_print(" track: ");
        debug_print_uint8(track);
	if ( 0 == what_drive )
	{
        	fdcStatus.mdrive.track = track;
	}
	else
	{
        	fdcStatus.sdrive.track = track;
	}

        debug_print("\n");
	return FDC_OK;
}

/*
 *  initializes the controller
 */
__FOS_DEVICE_STATUS fdc_init(void)
{
	int8_t rc;
	debug_print("fdc_init()\n");
	// detect controller
	rc = fdc_detect_controller();
	if ( FDC_OK != rc )
	{
		debug_print("fdc_init() -> this drive cannot handle this controller\n");
		return __FOS_DEVICE_STATUS_ERROR;
	}
	// detect drives
	rc = fdc_detect_floppy_drives();
	if ( FDC_OK != rc )
	{
		debug_print("fdc_init() -> this controller don't have drives\n");
		return __FOS_DEVICE_STATUS_ERROR;
	}
	// reset controller
	fdc_reset();
	// asume motors are off
	fdcStatus.mdrive.motor = 0x0;
	fdcStatus.sdrive.motor = 0x0;
	fdc_motorOn(1);
	fdc_recalibrate(1);

	return __FOS_DEVICE_STATUS_OK;
}

