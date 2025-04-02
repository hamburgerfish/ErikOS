[org 0x7c00]
KERNEL_OFFSET equ 0x1000
KERNEL_2_OFFSET equ 0x8000

	mov [BOOT_DRIVE], dl	; BIOS sets us boot drive in dl on boot
	mov bp, 0xf000
	mov sp, bp

	mov bx, MSG_REAL_MODE
	call print
	call print_nl

	call load_kernel ; read kernel from disk
	call switch_to_pm ; disable interrupts, load GDT, jumps to BEGIN_PM
	jmp $

%include "boot/boot_sect_print.asm"
%include "boot/boot3_disk.asm"
%include "boot/gdt.asm"
%include "boot/32_print.asm"
%include "boot/switch.asm"

[bits 16]
load_kernel:
	mov bx, MSG_LOAD_KERNEL
	call print
	call print_nl

	mov bx, KERNEL_OFFSET ; read from disk and store in 0x1000
	mov dh, 54 ; number of sectors to read
	mov ch, 0x00 ; head number
	mov cl, 2 ; sector number
	mov dl, [BOOT_DRIVE]
	call disk_load
	mov bx, KERNEL_2_OFFSET
	mov dh, 55; 35
	mov ch, 0x01
	mov cl, 22 ;20 (skip 2 sectors 512*2 = 1024 bytes of padding to account for bootsect at 0x7c00)
	call disk_load

	mov ah, 0

	mov al, 0x13
	int 0x10
	ret

[bits 32]
BEGIN_PM:
	call KERNEL_OFFSET	; give control to kernel
	jmp $			; stay here if kernel returns control


BOOT_DRIVE db 0 ; store in memory as dl may get overwritten
MSG_REAL_MODE db "Started in 16-bit Real mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0

times 510 - ($-$$) db 0
dw 0xaa55
