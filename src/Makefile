CFLAGS_INTEL= -Wall -m32 -nostdlib -static -nostdinc -masm=intel --freestanding -fno-stack-protector         -I devel/ -I drivers/ -ansi
CFLAGS_ATT  = -Wall -m32 -nostdlib -static -nostdinc             --freestanding -fno-stack-protector -Werror -I devel/ -I drivers/ -ansi
GCC=gcc-4.1

all:
	dd if=/dev/zero of=../fda.img bs=1k count=1440
	mkfs.vfat -R 8 ../fda.img

	nasm -o mbr.bin -f bin bootstrap.nasm
	dd if=mbr.bin of=../fda.img bs=1 conv=notrunc seek=62

	$(GCC) $(CFLAGS_INTEL) -S kernel.c -o kernel.S
	as kernel.S -o kernel.o

	$(GCC) $(CFLAGS_INTEL) -S io.c -o io.S
	as io.S -o io.o

	$(GCC) $(CCFLAGS_ATT) -S system.c -o system.S
	as system.S -o system.o

	$(GCC) $(CCFLAGS_INTEL) -S memory.c -o memory.S
	as memory.S -o memory.o

	$(GCC) $(CFLAGS_INTEL) -S debug.c -o debug.S
	as debug.S -o debug.o

	$(GCC) $(CFLAGS_INTEL) -S drivers/video.c -o video.S
	as video.S -o video.o

	ld kernel.o io.o system.o memory.o debug.o video.o --warn-common -T linker.ld -strip-all -o kernel.bin

	dd if=kernel.bin of=../fda.img bs=1 conv=notrunc seek=512
clean:
	rm *.o
	rm *.S
	rm *.bin
	rm *~
