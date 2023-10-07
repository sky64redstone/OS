; Ensures that we jump straight into the kernel's entry function.
; We're in protected mode by now, so use 32-bit instructions.
[bits 32]
; Declate that we will be referencing the external symbol 'main'.
; so the linker can substitute the final address
[extern kmain]

; invoke main() in our C kernel
call kmain
; Hang forever when we return from the kernel
jmp $
