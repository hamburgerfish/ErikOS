; load dh sectors from drive dl into ES:BX
disk_load:
	pusha
	push dx
	
	mov ah, 0x02 ; for int 0x13, 0x02 = read
	mov al, dh   ; al <- number of sectors to read (0x01 .. 0x80)
	; cl <- sector (0x01 .. 0x11)
		     ; 0x01 is boot sector. First available one is 0x02
	; dl <- drive number. Caller sets it as a parameter and gets it from BIOS
	; (0 = floppy, 1 = floppy2, 0x80 = hdd, 0x81 = hdd2)
	mov dh, ch ; dh <- head number (0x0 .. 0xF)
	mov ch, 0x00 ; ch <- cylinder (0x0 .. 0x3FF, upper 2 bits in cx)

	; [es:bx] <- pointer to buffer where data will be stored
	; caller sets it up and is standard location for int 0x13
	int 0x13 	; BIOS interrupt
	jc disk_error	; if error (stored in carry bit)

	pop dx
	cmp al, dh ; BIOS sets al to the number of sectors read. Compare it.
	jne sectors_error
	popa
	ret

disk_error:
	mov bx, DISK_ERROR
	call print
	call print_nl
	mov dh, ah ; ah = error code, dl = disk drive that dropped the error
	call print_hex
	jmp disk_loop

sectors_error:
	mov bx, SECTORS_ERROR
	call print

disk_loop:
	jmp $


DISK_ERROR: db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0
