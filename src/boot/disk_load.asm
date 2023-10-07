; load DH sectors to ES:BX from drive DL
disk_load:
    ; Store DX on stack so later we can recall
    ; how many sector were request to be read,
    ; even if it is altered in the meantime
    push dx

    ; BIOS read sector function
    mov ah, 0x02
    ; Read DH sectors
    mov al, dh
    ; Select cylinder 0
    mov ch, 0x00
    ; Select head 0
    mov dh, 0x00
    ; Start reading from second sector (i.e.
    ; after the boot sector)
    mov cl, 0x02
    
    ; BIOS interrupt
    int 0x13

    ; Jump if error (i.e. carry flag set)
    jc disk_error

    ; Restore DX from the stack
    pop dx
    ; if AL (sectors read) != DH (sectors expected)
    cmp dh, al
    ; display error message
    jne disk_error
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_str
    jmp $

DISK_ERROR_MSG: 
    db 'Disk read error!', ENDLINE, NULL