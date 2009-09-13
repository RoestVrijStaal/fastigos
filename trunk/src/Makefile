CFLAGS_INTEL=-Wall -m32 -nostdlib -static  -nostdinc -masm=intel --freestanding -fno-stack-protector -Werror
CFLAGS_ATT=-Wall -m32 -nostdlib -static  -nostdinc --freestanding -fno-stack-protector -Werror
all:
	dd if=/dev/zero of=../fda.img bs=1k count=1440
	mkfs.vfat -R 8 ../fda.img
	nasm kernel_signature.nasm -o kernel_signature.bin
	nasm -o mbr.bin -f bin bootstrap.nasm
	dd if=mbr.bin of=../fda.img bs=1 conv=notrunc seek=62

	gcc $(CFLAGS_INTEL) -S kernel.c -o kernel.S
	as kernel.S -o kernel.o

	gcc $(CFLAGS_INTEL) -S video.c -o video.S
	as video.S -o video.o

	gcc $(CFLAGS_INTEL) -S string.c -o string.S
	as string.S -o string.o

	gcc $(CFLAGS_ATT) -S segments.c -o segments.S
	as segments.S -o segments.o

	gcc $(CFLAGS_INTEL) -S util.c -o util.S
	as util.S -o util.o

	gcc $(CFLAGS_INTEL) -S pic.c -o pic.S
	as pic.S -o pic.o

	gcc $(CFLAGS_INTEL) -S pit.c -o pit.S
	as pit.S -o pit.o

	gcc $(CFLAGS_INTEL) -S fdc.c -o fdc.S
	as fdc.S -o fdc.o

	ld kernel.o util.o video.o string.o segments.o pic.o fdc.o pit.o --warn-common -T linker.ld -strip-all -o kernel_code.bin
	dd if=kernel_signature.bin of=kernel.bin bs=1 count=16
	dd if=kernel_code.bin of=kernel.bin conv=notrunc bs=1 seek=16
	dd if=kernel.bin of=../fda.img bs=1 conv=notrunc seek=512
clean:
	rm *.o
	rm *.S
	rm *.bin

