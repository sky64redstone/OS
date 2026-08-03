OUTPUT_FORMAT(elf32-i386)
ENTRY(kmain)        /* entry point */

/* eliminate warning:
 * "/usr/bin/ld: warning: build/kernel.elf has a LOAD segment
 * with RWX permissions"
 */
PHDRS {
  text PT_LOAD FLAGS(5); /* R-X */
  data PT_LOAD FLAGS(6); /* RW- */
}

SECTIONS {
    . = 0x1000;       /* load address, same as in src/bootloader.asm*/

    .text : {
        *(.text)
    } :text

    .rodata : {
        *(.rodata*)
    } :text

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
        *(COMMON)
    } :data

    .builtin_drivers ALIGN(4) : {
        __builtin_drivers_start = .;
        KEEP(*(.builtin_drivers))
        __builtin_drivers_end = .;
    }
}
