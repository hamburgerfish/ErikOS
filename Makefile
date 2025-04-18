C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c minesweeper/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h minesweeper/*.c)

OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

os-image.img: boot/bootsect.bin kernel.bin
	cat $^ > os-image.img

kernel.bin: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -T kernel.ld $^ --oformat binary
	#i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# for debugging
kernel.elf: boot/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^

run: os-image.img
	qemu-system-i386 -fda os-image.img

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# generic rules for wildcards
# to make an object compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o minesweeper/*.o
