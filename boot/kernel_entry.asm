global _start;
[bits 32]

_start:
	[extern kernel_main]	; Define calling point. Same name as kernel.c "main" function.
	call kernel_main 	; calls the C function. Linker knows where it is in memory
	jmp $
