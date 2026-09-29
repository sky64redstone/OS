[bits 32]

global _start
extern kmain

MBALIGN    equ 1 << 0
MEMINFO    equ 1 << 1
MBFLAGS    equ MBALIGN | MEMINFO
MBMAGIC    equ 0x1BADB002
MBCHECKSUM equ -(MBMAGIC + MBFLAGS)

section .multiboot
align 4
  dd MBMAGIC
  dd MBFLAGS
  dd MBCHECKSUM

section .bss
align 16
stack_bottom:
  resb 0x4000
stack_top:

section .text
_start:
  cli

  ; Multiboot does not guarantee a usable GDTR.
  lgdt [gdt_descriptor]

  ; Reload CS using our own GDT.
  jmp CODE_SEG:load_segments

load_segments:
  ; Multiboot leaves ESP undefined.
  mov esp, stack_top

  ; Multiboot:
  ;   EAX = 0x2BADB002
  ;   EBX = physical address of multiboot_info
  ;
  ; cdecl arguments are pushed right-to-left.
  push ebx
  push eax

  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  cld

  call kmain
  add esp, 8

.hang:
  cli
  hlt
  jmp .hang

gdt_start:
  dq 0x0000000000000000
  dq 0x00CF9A000000FFFF
  dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start - 1
  dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10
