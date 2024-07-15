; Memory Table:
;
; 0x0      - 0x400:	Interrupt Vector Table (1KB)
; 0x400    - 0x500:	BIOS Data Area (256 bytes)
; 0x500    - 0x7C00:	Free Memory
; 0x7C00   - 0x7E00:	Loaded Boot Sector (512 bytes)
; 0x7E00   - 0x9FC00:	Free Memory
; 0x9FC00  - 0xA0000:	Extended BIOS Data Area (639 KB)
; 0xA0000  - 0xC0000:	Video Memory (128 KB)
; 0xC0000  - 0x100000:	BIOS (256 KB)
; 0x100000 - infinite:	Free Memory

; Drive Table:
;
; DL = 0x00		first floppy disk
; DL = x		xth floppy disk
; DL = 0x7F		128th floppy disk
; DL = 0x80		first hard disk
; DL = x		(x - 0x7F)th hard disk
; DL = 0xE0		CD/DVD, or 97th hard disk
; DL = 0xFF		128th hard disk
	
; Set the memory offset to 0x7C00
[org 0x7C00]
; Hint 16 bit real mode
[bits 16]

	; memory offset to which we will load our kernel
	; Its also the default _start adrress after linking with ld
	KERNEL_OFFSET equ 0x1000

	; BIOS stores our boot drive in DL, 
	; so remember it for later
	mov [BOOT_DRIVE], dl

	; setup the stack
	mov bp, 0x8000
	mov sp, bp

	mov si, REAL_MODE_MSG
	call print_str

	call load_kernel

	call switch_to_pm

	jmp $

; print_str:
;   si: string address
; print_hex:
;   dx: hex number
%include "src/boot/print_16_bit.asm"
; disk_load:
;   bx: loaded data address
;   dh: sectors count (1 sector == 512 bytes)
;   dl: BIOS drive number - typically 0x80 for the "C" drive
%include "src/boot/disk_load.asm"
%include "src/boot/gdt.asm"
%include "src/boot/print_32_bit.asm"
%include "src/boot/switch_to_pm.asm"

[bits 16]
load_kernel:
	mov si, LOAD_KERNEL_MSG
	call print_str
	mov dx, [BOOT_DRIVE]
	and dx, 0x00FF
	call print_hex
	mov si, ENDL_MSG
	call print_str
	
	; load 15 secors increase this value when 
	; the kernel has an unexprected bug
	mov dh, 15
	mov bx, KERNEL_OFFSET
	mov dl, [BOOT_DRIVE]
	call disk_load

	ret

[bits 32]
BEGIN_PM:
	mov ebx, PROT_MODE_MSG
	call print_str_pm

	call KERNEL_OFFSET

	jmp $

; BOOT_DRIVE stores the bootdrive from DL
BOOT_DRIVE: db 0
ENDL_MSG: db ENDL, 0
REAL_MODE_MSG: db "startig from 16 bit real mode...", ENDL, 0
LOAD_KERNEL_MSG: db "loading kernel from disk: ", 0, ENDL, 0
PROT_MODE_MSG: db ENDL, "successfully landed in 32 bit protected mode...", 0

; When compiled, our program must fit into 512 bytes,
; with the last two bytes being the magic number,
; so here, tell our assembly compiler to pas out our
; program with enough zero bytes to bring us to the
; 510th byte
times 510-($-$$) db 0
; Last to bytes (one word) from the magic number,
; so the  BIOS knowss we are a boot sector.
dw 0xAA55
