%define	SEGMENT_NUL	0
%define SEGMENT_CODI	8
%define SEGMENT_PILA	16
%define SEGMENT_DADES	24

%define	BOOTSTRAP_SECOND_STAGE_REALMODE	0x7E00
%define	BOOTSTRAP_SECOND_STAGE_PMODE	0x7E000

[BITS 16]
[ORG 0x7C3E]
_start:

configure_stack:
	cli

	mov	AX, 0x7c5e	; stack after bootloade
	mov	BX, 0x00ff	; 255byte depth
	mov	SS, AX		;
	mov	SP, BX		;

	mov	SI, strings.hello
	call	print_routine

	; read first 8k (16k in total)

	mov	AX, 4		;
	push	AX		; 3 retry's
llegir_disc:
	mov	AX,	BOOTSTRAP_SECOND_STAGE_REALMODE	; desplaçament pel kernel
							; 1024 (taula t'interrupcions)
	mov	ES,	AX
	mov	BX,	0
	mov	AH,	2	; Load disk data to ES:BX
	mov	AL,	16	; Load 16 sectors
	mov	CH,	0	; Cylinder=0
	mov	CL,	2	; Sector=2
	mov	DH,	0	; Head=0
	mov	DL,	0	; Drive=0
	int	13h
	jc	.retry
	jmp	llegir_disc2
.retry:
	pop	AX
	dec	AX
	cmp	AX, 0
	je	print_error
	push	AX

	mov	SI, strings.error_dontload
	call	print_routine
	
	xor	AH, AH			; reset disk
	xor	DL, DL			; drive 0
	int	0x13
	jmp	llegir_disc
								; read next 8k

llegir_disc2:
	pop	AX
	mov	AX, 4		;
	push	AX		; 3 retry's
.tryread:
	mov	AX,	BOOTSTRAP_SECOND_STAGE_REALMODE+8192	; desplaçament pel kernel
	mov	ES,	AX
	mov	BX,	0
	mov	AH,	2	; Load disk data to ES:BX
	mov	AL,	16	; Load 16 sectors
	mov	CH,	0	; Cylinder=1
	mov	CL,	2	; Sector=2
	mov	DH,	0	; Head=0
	mov	DL,	0	; Drive=0
	int	13h
	jc	.retry
	jmp	verify_kernel_signature
.retry:
	pop	AX
	dec	AX
	cmp	AX, 0
	je	print_error
	push	AX

	mov	SI, strings.error_dontload
	call	print_routine
	
	xor	AH, AH			; reset disk
	xor	DL, DL			; drive 0
	int	0x13

	jmp	.tryread

verify_kernel_signature:
	mov	SI, kernel_signature
	mov	BX, BOOTSTRAP_SECOND_STAGE_REALMODE
	mov	ES, BX
	mov	BX, 0
	mov	CX, 16
.verifyloop:
	lodsb
	mov	DL, [ES:BX]
	inc	BX
	cmp	AL, DL
	jne	print_kernel_nomatch
	loop	.verifyloop
alldone:
	mov	AX, 0
	mov	ES, AX

to_pmode:
	lgdt	[gdtinfo]

	mov	EAX, CR0
	or	AL, 1
	mov	CR0, EAX

		; stack
	mov	EAX, SEGMENT_PILA
	mov	SS, EAX
	mov	EAX, 0xFFFE
	mov	ESP, EAX

		; configure segments
	mov	AX, SEGMENT_DADES
	mov	DS, AX
	mov	ES, AX
	mov	FS, AX
	mov	GS, AX

	jmp	SEGMENT_CODI:pmode

; ############################################################################
; ### Kernel signature don't match
; ############################################################################
print_kernel_nomatch:
	mov	SI, strings.error_kernel_nomatch
	call	print_routine
	call	infinite
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
; ### Halt system
; ############################################################################
infinite:
	mov	SI, strings.err_system_halted
	call	print_routine
.loop:
	hlt
	jmp	.loop
[BITS 32]
	; I'm in protected mode!
pmode:
	jmp	SEGMENT_CODI:BOOTSTRAP_SECOND_STAGE_PMODE+0x10	; shut up the signature!
strings:
.hello:			db	"Loading fastigOS...",10,13,0
.error_read_fd:		db	"Err reading drive, ",0
.error_dontload:	db	"retrying...",10,13,0
.error_kernel_nomatch:	db	"Kernel not found ", 0
.err_system_halted:	db	"system halted",0
gdtinfo:
	dw	gdt.fi - gdt -1
	dd	gdt
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

.codi:               ; Segment de codi: 4Gb
	dw	0xFFFF
        dw	0x0000
        db	0x00
	db	10011010b
	db	11001111b
        db	0x00

.pila:               ; Pila 64k per sobre del 1er mega
        dw	0xFFFF
        dw	0x0000
        db	0x10
	db	10010010b
	db	01000000b
        db	0x00

.dades:               ; Segment de dades: 4G
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
.fi:                ; Used to calculate the size of the GDT
kernel_signature:
incbin "kernel_signature.bin"
