%define ENDLINE 0x0D, 0x0A
%define NULL 0

;msg: db 'Hello world!', ENDLINE, NULL
;mov si, msg
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
