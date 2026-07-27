; isr_handler defined in cpu/isr.h
[extern isr_handler]

[global load_idt]

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

isr_common:
  pusha

  ; push data segment selector
  xor eax, eax
  mov ax, ds
  push eax

  ; use kernel data segment
  mov ax, 0x10
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  ; hand over stack to our c function
  push esp
  call isr_handler
  pop eax

  ; restore original segment pointers
  pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
  popa
  add esp, 8 ; remove int_no and err_no

  ; pops cs, eip, eflags, ss, and esp
  iret

irq_common:
  add esp, 8 ; remove int_no and err_no
  iret

%macro ISR_NORMAL 1
  global isr%1
  isr%1:
    cli
    push byte %1
    push byte 0
    jmp isr_common
%endmacro

%macro ISR_ERROR 1
  global isr%1
  isr%1:
    cli
    push byte %1
    jmp isr_common
%endmacro

; Define the ISRs using the macros
ISR_NORMAL 0  ; 0: Divide By Zero Exception
ISR_NORMAL 1  ; 1: Debug Exception
ISR_NORMAL 2  ; 2: Non-Maskable Interrupt
ISR_NORMAL 3  ; 3: Breakpoint Exception
ISR_NORMAL 4  ; 4: Overflow Exception
ISR_NORMAL 5  ; 5: Out of Bounds Exception
ISR_NORMAL 6  ; 6: Invalid Opcode Exception
ISR_NORMAL 7  ; 7: Device Not Available Exception
ISR_ERROR  8  ; 8: Double Fault Exception
ISR_NORMAL 9  ; 9: Coprocessor Segment Overrun
ISR_ERROR  10 ; 10: Invalid TSS Exception
ISR_ERROR  11 ; 11: Segment Not Present Exception
ISR_ERROR  12 ; 12: Stack Fault Exception
ISR_ERROR  13 ; 13: General Protection Fault
ISR_ERROR  14 ; 14: Page Fault Exception
ISR_NORMAL 15 ; 15: Reserved
ISR_NORMAL 16 ; 16: Floating Point Exception
ISR_ERROR  17 ; 17: Alignment Check Exception
ISR_NORMAL 18 ; 18: Machine Check Exception
ISR_NORMAL 19 ; 19: SIMD Floating Point Exception
ISR_NORMAL 20 ; 20: Virtualization Exception
ISR_NORMAL 21 ; 21: Control Protection Exception
ISR_NORMAL 22 ; 22: Reserved
ISR_NORMAL 23 ; 23: Reserved
ISR_NORMAL 24 ; 24: Reserved
ISR_NORMAL 25 ; 25: Reserved
ISR_NORMAL 26 ; 26: Reserved
ISR_NORMAL 27 ; 27: Reserved
ISR_NORMAL 28 ; 28: Reserved
ISR_NORMAL 29 ; 29: Reserved
ISR_ERROR  30 ; 30: Reserved
ISR_NORMAL 31 ; 31: Reserved

%macro IRQ_NORMAL 1
  global irq%1
  irq%1:
    cli
    push byte %1 + 32
    push byte 0
    jmp isr_common
%endmacro

IRQ_NORMAL 0
IRQ_NORMAL 1
IRQ_NORMAL 2
IRQ_NORMAL 3
IRQ_NORMAL 4
IRQ_NORMAL 5
IRQ_NORMAL 6
IRQ_NORMAL 7
IRQ_NORMAL 8
IRQ_NORMAL 9
IRQ_NORMAL 10
IRQ_NORMAL 11
IRQ_NORMAL 12
IRQ_NORMAL 13
IRQ_NORMAL 14
IRQ_NORMAL 15
