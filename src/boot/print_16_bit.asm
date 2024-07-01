%define ENDL 0x0D, 0x0A

;msg: db 'Hello world!', ENDLINE, NULL
;mov bx, msg
;call print_str
print_str:
    ; save registers we will modify
    push si
    push ax
    push bx
    mov ah, 0x0e        ; int 10/ah = 0eh -> scrolling teletype BIOS routine

loop_str:
    lodsb               ; loads next character in al
    or al, al           ; verify if next character is null
    jz finished_str

    mov ah, 0x0E        ; call bios interrupt
    mov bh, 0           ; set page number to 0
    int 0x10

    jmp loop_str

finished_str:
    pop bx
    pop ax
    pop si
    ret

;mov dx, 0x0000 ; print 0
;call print_hex
print_hex:
    pusha
    
    mov si, HEX_OUT + 2

    mov cx, 0

next_character:
    inc cx

    mov bx, dx
    and bx, 0xF000
    shr bx, 4

    add bh, 0x30

    cmp bh, 0x39
    jg add_7

add_character_hex:
    mov [si], bh

    inc si

    shl dx, 4

    cmp cx, 4
    jnz next_character
    jmp hex_done

hex_done:
    mov si, HEX_OUT
    
    call print_str

    popa

    ret

add_7:
    add bh, 0x7

    jmp add_character_hex

HEX_OUT:
    db "0x0000", 0
