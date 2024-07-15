; load DH sectors to ES:BX from drive DL
disk_load:
    	; Store DX on stack so later we can recall
    	; how many sector were request to be read,
    	; even if it is altered in the meantime
    	push dx

    	; BIOS read sector function
    	mov ah, 0x02
    	; Read dh sectors
	mov al, dh
    	; Select head 0
    	mov dh, 0x00
	; select cylinder 0
	mov ch, 0x00
    	; Start reading from second sector (i.e.
    	; after the boot sector)
	; starts from 1, not 0
    	mov cl, 0x02
    
    	; BIOS interrupt
    	int 0x13

    	; Jump if error (i.e. carry flag set)
    	jc disk_read_error

    	; Restore DX from the stack
    	pop dx
    	; if AL (sectors read) != DH (sectors expected)
    	cmp dh, al
    	; display error message
    	jne disk_cmp_error
    	ret

disk_read_error:
    	mov si, DISK_ERROR_READ_MSG
    	jmp disk_error

disk_cmp_error:
    	mov si, DISK_ERROR_SECTORS_MSG
	jmp disk_error

disk_error:
	call print_str
	mov dx, ax
	call print_hex
	jmp $

DISK_ERROR_READ_MSG: 
    db "Disk read error!", ENDL, "Error: ", 0

DISK_ERROR_SECTORS_MSG:
    db "Read sectors don't match expected sectors!", ENDL, "Error: ",  0
