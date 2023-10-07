[org 0x7C00]
; This is the memory offset to which we will load our kernel
KERNEL_OFFSET equ 0x1000

    ; BIOS stores our boot drive in DL, so it's
    ; best to remember this for later.
    mov [BOOT_DRIVE], dl

    mov bp, 0x8000     ; Set the base of the stack a little above where BIOS
    mov sp, bp         ; loads our boot sector - so it won ’t overwrite us.

    mov si, MSG_REAL_MODE
    call print_str

    call load_kernel

    call switch_to_pm

    jmp $

%include "src/boot/print_16_bit.asm"
%include "src/boot/print_32_bit.asm"
%include "src/boot/switch_to_pm.asm"
%include "src/boot/disk_load.asm"

[bits 16]
; load kernel
load_kernel:
    ; Print a message to say we are loading the kernel
    mov bx, MSG_LOAD_KERNEL
    call print_str

    ; Set-up parameters for our disk_load routine, so
    ; that we load the first 15 sectors (excluding
    ; the boot sector) from the boot disk (i.e. our
    ; kernel code) to address KERNEL_OFFSET
    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]
    call disk_load

    ret
    
;page 54
[bits 32]
; Here we arrive after switching to and init protected mode.
BEGIN_PM:
    ; Use our 32-bit print routine to 
    ; announce we are in protected mode
    mov ebx, MSG_PROT_MODE
    call print_str_pm

    ; Now jump to the address of our loaded
    ; kernel code, assume the brace position,
    ; and cross your fingers.
    call KERNEL_OFFSET

    ; Hang.
    jmp $

BOOT_DRIVE:
    db 0
MSG_REAL_MODE:
    db 'Starting from 16 bit...', ENDLINE, NULL
MSG_PROT_MODE:
    db 'Starting with 32 bit...', NULL
MSG_LOAD_KERNEL:
    db 'Loading kernel into memory...', ENDLINE, NULL

; fill the remaining space of 512 bytes with 0
times 510 - ($ - $$) db 0
; magic number for the BIOS
dw 0xaa55
