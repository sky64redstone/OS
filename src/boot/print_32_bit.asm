[bits 32]

; 2-byte vga value:
; 0b0000     | 0b0000           | 0b00000000
; background | foreground color | ascii character

; 4-bit vga color table:
;  0 black
;  1 blue
;  2 green
;  3 cyan
;  4 red
;  5 magenta
;  6 brown
;  7 light gray
;  8 dark gray
;  9 light blue
; 10 light green
; 11 light cyan
; 12 light red
; 13 light magenta
; 14 yellow
; 15 white

VIDEO_MEMORY equ 0xB8000
WHITE_ON_BLACK equ 0x0F

; prints a null-terminated string pointed to by EDX
; WARNING: no scrolling!!! We will soon be booting code
; written in a higher level language, where we can make
; much ligher work of these things.
print_str_pm:
    pusha
    mov edx, VIDEO_MEMORY ; Set edx to the start of vid mem.

print_str_pm_loop:
    mov al, [ebx] ; Store the char at EBX in AL
    mov ah, WHITE_ON_BLACK ; Store the attributes in AH

    cmp al, 0 ; if (al == 0), at end of string , so
    je print_str_pm_done ; jump to done

    mov [edx], ax ; Store char and attributes at current
    ; character cell.

    add ebx, 1 ; Increment EBX to the next char in string.
    add edx, 2 ; Move to next character cell in vid mem.

    jmp print_str_pm_loop ; loop around to print the next char.

print_str_pm_done:
    popa
    ret
