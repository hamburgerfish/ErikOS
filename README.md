# ErikOS

This is a 32-bit operating system for the game Minesweeper.

It can be run on an emulator such as QEMU

CONTROLS:
    move cursor: WASD
    change mode: F
    interact: ENTER

ErikOS contains some elements of a traditional OS such as
1. video graphics driver
2. keyboard driver
3. global descriptor table
4. interrupt descriptor table
5. interrupt service routines
6. simple shell with text wrapping and scrolling (unused)
    - string manipulation
    - dynamically request memory from kernel

And the game minesweeper.

## booting **(code in boot/*)**
When booting an OS, BIOS looks at the first sector of the disk (cylinder 0, head 0, sector 1), or the first 512 bytes, and checks that bytes 511 and 512 are 0xAA55. If this condition is satisfied, BIOS will attempt to boot from this disk, loading it into memory address 0x7c00

This OS is much larger than 512 bytes, so this bootsector is also responsible for loading the rest of the OS from disk into memory. The kernel starts at memory address 0x1000 so this is where the rest of kernel is loaded.

The kernel is also larger than 0x6c00 bytes (0x7c00 - 0x1000), so loading all of it at once would overwrite the boot sector and some important BIOS functions, so 54 sectors, or 0x6c00 (54*512) bytes, are read into memory address 0x1000 - 0x7c00, then the second part of the kernel, in this case 55 sectors, is loaded into memory address 0x8000.

Since the kernel is compiled into a single contiguous binary file, it would not function properly if it is split into two parts as done by the bootsector. To allow the kernel to function properly, a custom linker file is used to specify the memory offsets at which the text part (code) and data part would be loaded into (0x1000 and 0x8000 respectively).

Once the kernel is fully loaded, control is given to the kernel by executing an unconditional jump to the 0x1000.

## video graphics **(code in drivers/screen.*)**
The default mode for the video graphics array (VGA) is text mode (mode 3) where the values starting from memory address 0xb8000 represent the ascii characters and colours shown on screen.

Before the unconditional jump to the kernel (0x1000), interrupt 0x10 is called with register ax = 0x0013. This sets the VGA to mode 13h where each byte starting from memory address 0xa0000 represents the colour value of a pixel on the screen **(code in boot/bootsect.asm)**.

Some helpful functions are used to easily set pixels on the screen at specified x, y coordinates, and easily draw images on the screen from bitmaps (arrays holding colour values and dimensions of images).

## minesweeper **(code in minesweeper/minesweeper.*)**
This is my implementation of minesweeper in C using my video graphics driver, without using the C stdlib, as it is OS specific.

It also uses my implementation of a timer, using the number of cpu cycles/frequency **(code in cpu/timer.*)** as well as my implementation of a seeded pseudo random number generator, using the number of cpu cycles as the seed.
