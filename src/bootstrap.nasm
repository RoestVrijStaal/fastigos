;
; bootstrap -- mbr program to load fastigOS kernel
;
; Author: Jordi Rubió
;
; Date: 16-09-2009
;

%define	SEGMENT_NULL			0x0
%define SEGMENT_CODE			0x8
%define SEGMENT_STACK			0x10
%define SEGMENT_DATA			0x18

%define BOOTSTRAP_STACK			0x7c5e
%define BOOTSTRAP_STACKSIZE		0x00ff

%define	BOOTSTRAP_KERNEL_SIGNATURE	0x7e00
%define	BOOTSTRAP_KERNEL_REALMODE	0x7e00
%define	BOOTSTRAP_KERNEL_PMODE		0x7e000
%define BREAKPOINT			xchg BX, BX

%define	SYSTEM_MEMORY			0x7C3E-4
; normal bootstrap runs on 0x7c00
; this bootstrap runs at 0x3e offset, beyond the FAT mbr structure

[BITS 16]
[ORG 0x7C3E]
_start:
	jmp	begin
begin:
configure_stack:
	cli						; disable interrupts

	mov	AX, BOOTSTRAP_STACK			;
	mov	BX, BOOTSTRAP_STACKSIZE			;
	mov	SS, AX					; configure Stack
	mov	SP, BX					;

	mov	SI, strings.hello			;
	call	print_routine				; say hello!

detect_ram:
	mov	EAX, 0xe801
	int	0x15
	jc	.nosupport
;AX = CX = extended memory between 1M and 16M, in K (max 3C00h = 15MB)
;BX = DX = extended memory above 16M, in 64K blocks
	push	AX			; save low memory
	mov	AX, BX			;
	mov	DX, 64			; high memory in 64k blocks
	mul	EDX			;
	pop	CX			;
	add	AX, CX			; sum all
	mov	[SYSTEM_MEMORY], AX

	jmp	.continue
.nosupport:
	mov	SI, strings.err_noram			;
	call	print_routine				; unable to detect free ram
	jmp	infinite
.continue:

							; read first 16blocks (8k in total)
	mov	AX, 4					;
	push	AX					; 3 retry's
read_disc1:
	mov	AX,	BOOTSTRAP_KERNEL_REALMODE	; Kernel memory offset (real mode address)
	mov	ES,	AX				; 
	mov	BX,	0				; offset 0
	mov	AH,	2				; Load disk data to ES:BX
	mov	AL,	16				; Load 16 sectors (maximum in floppy geometry)
	mov	CH,	0				; Cylinder=0
	mov	CL,	2				; Sector=2
	mov	DH,	0				; Head=0
	mov	DL,	0				; Drive=0
	int	13h
	jc	.retry					; if error, retry
	jmp	read_disc2				; all ok, continue with next block
.retry:
	pop	AX					; obtain retry counter
	dec	AX					; substract one
	cmp	AX, 0					; timeout ?
	je	print_error				; shit, big mistake!
	push	AX					; save again

	mov	SI, strings.error_dontload		; print error message
	call	print_routine				;
	
	call	reset_fd0
	jmp	read_disc1				; retry

read_disc2:
	pop	AX					;
	mov	AX, 4					;
	push	AX					; 3 retry's
.tryread:
	mov	AX,	BOOTSTRAP_KERNEL_REALMODE+8192	; after the first read
	mov	ES,	AX
	mov	BX,	0
	mov	AH,	2				; Load disk data to ES:BX
	mov	AL,	16				; Load 16 sectors
	mov	CH,	0				; Cylinder=1
	mov	CL,	1				; Sector=1
	mov	DH,	0				; Head=0
	mov	DL,	0				; Drive=0
	int	13h
	jc	.retry
	jmp	to_pmode
.retry:
	pop	AX
	dec	AX
	cmp	AX, 0
	je	print_error
	push	AX

	mov	SI, strings.error_dontload
	call	print_routine
	
	call	reset_fd0

	jmp	.tryread

to_pmode:
	lidt	[idtinfo]			; load IDT
	lgdt	[gdtinfo]			; load GDT
						
	mov	EAX, CR0			; prepare protected mode
	or	AL, 1				; set bit of protected mode in cpu
	mov	CR0, EAX			;
						;
						; configure stack
	mov	EAX, SEGMENT_STACK		; in protected mode, in second register of GDT
	mov	SS, EAX				;
	mov	EAX, 0xFFFE			;
	mov	ESP, EAX			;
						;
						; configure data segments
	mov	AX, SEGMENT_DATA		; in protected mode, the thirth registr of GDT
	mov	DS, AX				;
	mov	ES, AX				;
	mov	FS, AX				;
	mov	GS, AX				;

	jmp	SEGMENT_CODE:pmode		; Let's go to the party!

; ############################################################################
; ### Read error
; ############################################################################
print_error:
	mov	SI, strings.error_read_fd
	call	print_routine
	call	infinite
; ############################################################################
; ### Print routine
; ############################################################################
; @param SI - string to print
; ############################################################################
print_routine:
.loop:
	lodsb
	cmp	al, 0
	je	.end
	mov	AH, 0Eh
	mov	BX, 7
	int	10h
	jmp	.loop
.end:
	ret
; ############################################################################
; ### Reset floppy disk 
; ############################################################################
reset_fd0:
	xor	AH, AH			; reset disk
	xor	DL, DL			; drive 0
	int	0x13
	ret
; ############################################################################
; ### Halt system
; ############################################################################
infinite:
	mov	SI, strings.err_system_halted
	call	print_routine
.loop:
	hlt
	jmp	.loop

; ############################################################################
; ### 32bit bootloader part
; ############################################################################

[BITS 32]
								; I'm in protected mode!
pmode:								;
	mov	EBP, 0x1fffe
	mov	ESP, 0x1fffe

	xor	EAX, EAX
	mov	AX, [SYSTEM_MEMORY]
	push	EAX				; ramsize uint32_t
	push	EAX				; EFLAGS
	push	EAX				; EIP
	jmp	SEGMENT_CODE:BOOTSTRAP_KERNEL_PMODE		; shut up the signature! and run the kernel (bye bye!)
; ############################################################################
; ### All strings
; ############################################################################
strings:
.hello:			db	"Loading fastigOS...",10,13,0
.error_read_fd:		db	"Err reading drive, ",0
.error_dontload:	db	"retrying...",10,13,0
.err_system_halted:	db	"system halted",0
.err_noram:		db	"unable to detect RAM size",10,13,0
; ############################################################################
; ### IDT data
; ############################################################################
idtinfo:
	dw	0
	dd	0
; ############################################################################
; ### GDT data
; ############################################################################
gdtinfo:
	dw	gdt.end - gdt -1
	dd	gdt
; ############################################################################
; ### GDT descriptors
; ############################################################################
gdt:
.nul:
	dw	0
	dw	0
	db	0
	db	0
	db	0
	db	0
; dw Limit L0-L15(16)
; dw Base B0-B15(16)
; db Base B16-B23(8)
; db P(1)/DPL(2)/S(1=codi/dades)/E(1)/X(1)/RW(1)/0
; db G(1)/OpSize(1)/0/0/Limit L16-L19(4)
; db Base B24-B31

.code:				; Code Segment: flat 4Gb
	dw	0xFFFF
        dw	0x0000
        db	0x00
	db	10011010b
	db	11001111b
        db	0x00

.stack:				; Stack Segment: 64k beyond 1st megabyte
        dw	0xFFFF
        dw	0x0000
        db	0x00
	db	10010010b
	db	11001111b
        db	0x00
;.stack:				; Stack Segment: 64k beyond 1st megabyte
;        dw	0xFFFF
;        dw	0x0000
;        db	0x10
;	db	10010010b
;	db	01000000b
;        db	0x00
.data:				; Data segment: flat 4G
	dw	0xFFFF
	dw	0x0000
        db	0x00
	db	10010010b
	db	11001111b
        db	0x00
; dw Limit L0-L15(16)
; dw Base B0-B15(16)
; db Base B16-B23(8)
; db P(1)/DPL(2)/0/1011
; db G(1)/0/0/AVL(1)/Limit L16-L19(4)
; db Base B24-B31	
.end:                ; Used to calculate the size of the GDT
