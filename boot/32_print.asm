[bits 32] ; using 32 bit protected mode

; define constants
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0e ; colour byte for each character (white on black = 0x0f)

print_string_pm:
	pusha
	mov edx, VIDEO_MEMORY

print_string_pm_loop:
	mov al, [ebx] ; caller sets ebx as address of first character
	mov ah, WHITE_ON_BLACK

	cmp al, 0 ; check if end of string
	je print_string_pm_done

	mov [edx], ax ; store character + colour in video memory
	add ebx, 1 ; next character
	add edx, 2 ; next video memory position

	jmp print_string_pm_loop

print_string_pm_done:
	popa
	ret

