#include "fdc.h"
// http://www.osdever.net/tutorials/view/detecting-floppy-drives
// http://wiki.osdev.org/Floppy_Disk_Controller#The_Floppy_Subsystem_is_Ugly
// http://www.disy.cse.unsw.edu.au/lxr/source/drivers/block/floppy.c?v=linux-2.6.32
// http://koders.com/c/fid051291340B94EC7F5D1A38EF6843466C0B07627B.aspx?s=fdc#L7
// http://bos.asmhackers.net/docs/floppy/snippet_5/FLOPPY.ASM

#define FDC_ERROR_TIMEOUT 	-1
#define FDC_OK		 	 0

#define FDC_DEFAULT_IRQ		32+6	// default 32 int's + 6 irq's
struct fdcStatus_s
{
	uint8_t type;
	uint8_t interrupt;
	uint8_t mdrive;
	uint8_t sdrive;
	uint8_t sr0;
	uint8_t track;
};
struct fdcStatus_s fdcStatus;

char *drive_types[6] = { "free", "360kb 5.25in", "1.2mb 5.25in", "720kb 3.5in", "1.44mb 3.5in", "2.88mb 3.5in" };

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

#define FDC_NEC_CONTROLLER 0x80
#define FDC_ENHANCED_CONTROLLER 0x90

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

void fdc_detect_controller(void)
{
	/* get type of fdc */
	fdc_send(FDC_VERSION);
	fdcStatus.type = fdc_get();
	if ( FDC_ENHANCED_CONTROLLER == fdcStatus.type )
	{
		debug_print("fdc -> enhanced controller found\n");
	}
	else if ( FDC_NEC_CONTROLLER == fdcStatus.type )
	{
		debug_print("fdc -> 8272A/765A controller found\n");
	}
	else
	{
		debug_print("fdc -> unknown controller found!\n");
	}
}

void fdc_recived_irq()
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

void fdc_motorOn(uint8_t what_drive)
{
	debug_print("fdc_motorOn(");
	debug_print_uint8(what_drive);
	debug_print(")\n");

	uint8_t drive = 0x0;
	if ( what_drive == 0x0 )
	{
		drive = 0x1c;
	}
	else if ( what_drive == 0x1 )
	{
		drive = 0x2d;
	}
	else
	{
		debug_print("fdc -> unrecognized drive ");
		debug_print_uint8(what_drive);
		debug_print("\n");
		return;
	}
	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, drive);
}

void fdc_motorOff(uint8_t what_drive)
{
	debug_print("fdc_motorOff(");
	debug_print_uint8(what_drive);
	debug_print(")\n");

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
		return;
	}
	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, drive);
}

void fdc_reset()
{
	debug_print("reset_fdc() begin\n");
	fdcStatus.interrupt = 0x0;
	install_idt_handler(FDC_DEFAULT_IRQ, (uint32_t)fdc_recived_irq);
	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, 0x00); /*disable controller*/
  	fdc_outb(FDC_DIGITAL_OUTPUT_REGISTER, 0x0c); /*enable controller*/
	debug_print("reset_fdc() waiting int...\n");
	while(!fdcStatus.interrupt);
	debug_print("reset_fdc() int completed\n");
	debug_print("reset_fdc() return\n");
}

static void fdc_recalibrate(uint8_t what_drive)
{
	uint8_t retries;
	debug_print("fdc_recalibrate(");
	debug_print_uint8(what_drive);
	debug_print(")\n");

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
		return;
	}

	uint8_t recalibration = 0;
	fdcStatus.interrupt = 0x0;
        install_idt_handler(FDC_DEFAULT_IRQ, (uint32_t)fdc_recived_irq);

	for (retries = 0; retries < 14; retries++)
	{
		fdc_send(FDC_RECALIBRATE);
		fdc_send(drive);
		sleep(100);
		fdc_send(FDC_SENSE_INTERRUPT);
		fdcStatus.sr0 = fdc_get();
		fdcStatus.track = fdc_get();
		if (!(fdcStatus.sr0 & 0x10))
		{
			recalibration = 1;
			break;
		}
	}
	if (recalibration == 0)
	{
		debug_print("fdc -> recalibration timeout\n");
	}
	else
	{
		debug_print("fdc -> calibration result: drive ");
		debug_print_uint8(drive);
		debug_print(" sr0: ");
		debug_print_uint8(fdcStatus.sr0);
		debug_print(" track: ");
		debug_print_uint8(fdcStatus.track);
		debug_print("\n");
	}
}
static void fdc_dumpreg()
{
// W 0 0 0     0 1 1 1 0
// R               PCN_0          = 8
// R               PCN_1          = 8
// R               PCN_2          = 8
// R               PCN_3          = 8
// R   SRT           HUT          = 4/4
// R     HLT          ND          = 7/1
// R              SC/EOT          = 8
// R LOCK 0 D3 D2 D1 D0 GAP WGATE = 1/1/1/1/1/1/1/1
// R 0 EIS EFIFO POLL FIFOTHR     = 1/1/1/1/4
// R              PRETRK          = 8
 
}
static void fdc_configure()
{
// W 0   0     0    1      0 0 1 1
// W 0   0     0    0      0 0 0 0
// W 0 EIS EFIFO POLL __FIFOTHR___ // EIS=No implied seeks / EFIFO=FIFO disabled / POLL=Polling Enabled / FIFOTHR=FIFO Thresold set 1 byte
// W ___________PRETRK____________ // PRETRK=Pre compensation set to track 0
	debug_print("fdc_configure()\n");

	fdc_send(FDC_CONFIGURE);
	fdc_send(0x0);
	fdc_send(0xf);
	fdc_send(0x0);
}

void fdc_detect_floppy_drives(void)
{
	uint8_t data;
	// get data from cmos
	fdc_outb(0x70, 0x10);
	data = fdc_inb(0x71);
	fdcStatus.mdrive = data >> 4;
	fdcStatus.sdrive = data & 0xf;
	debug_print("fdc -> master: ");
	debug_print(drive_types[fdcStatus.mdrive]);
	debug_print("\n");
	debug_print("fdc -> slave: ");
	debug_print(drive_types[fdcStatus.sdrive]);
	debug_print("\n");
}

__FOS_DEVICE_STATUS fdc_init(void)
{
	debug_print("fdc_init()\n");
	fdc_detect_controller();
	fdc_detect_floppy_drives();
	fdc_reset();
	fdc_configure();

	fdc_motorOn(0);
	fdc_recalibrate(0);
	return __FOS_DEVICE_STATUS_OK;
}

__FOS_DEVICE_STATUS fdc_deinit(void)
{
	debug_print("fdc_deinit()\n");
	return __FOS_DEVICE_STATUS_OK;
}


