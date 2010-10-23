CFLAGS_INTEL= -Wall -m32 -nostdlib -static -nostdinc -masm=intel --freestanding -fno-stack-protector         -I devel/ -I drivers/ 
CFLAGS_ATT  = -Wall -m32 -nostdlib -static -nostdinc             --freestanding -fno-stack-protector -Werror -I devel/ -I drivers/
#GCC=gcc-4.1
GCC=gcc
#-ansi

all:
	dd if=/dev/zero of=../fda.img bs=1k count=1440
	mkfs.vfat -R 8 ../fda.img

	nasm -o mbr.bin -f bin boot/bootstrap.nasm
	dd if=mbr.bin of=../fda.img bs=1 conv=notrunc seek=62

	$(GCC) $(CFLAGS_INTEL) -S kernel/kernel.c -o kernel.S -I./kernel
	as kernel.S -o kernel.o

	$(GCC) $(CFLAGS_INTEL) -S debug/debug.c -o debug.S -I./debug -DUSE_BOCHS_PORT_E9_HACK
	as debug.S -o debug.o

	$(GCC) $(ATT_INTEL) -S mmu/mmu.c -o mmu.S -I./mmu
	as mmu.S -o mmu.o

	#devices

	$(GCC) $(CFLAGS_INTEL) -S devices/null.c -o null.S -I./devices -DUSE_BOCHS_PORT_E9_HACK
	as null.S -o null.o

	ld kernel.o debug.o mmu.o null.o --warn-common -T linker.ld -strip-all -o fbcore.bin

	dd if=fbcore.bin of=../fda.img bs=1 conv=notrunc seek=512
clean:
	rm *.o
	rm *.S
	rm *.bin
	rm *~
