OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)        /* entry point */

/* eliminate warning:
 * "/usr/bin/ld: warning: build/kernel.elf has a LOAD segment
 * with RWX permissions"
 */
PHDRS {
  text PT_LOAD FLAGS(5); /* R-X */
  data PT_LOAD FLAGS(6); /* RW- */
  rodata PT_LOAD FLAGS(4); /* R-- */
}

SECTIONS {
    . = 0x100000;

    .multiboot ALIGN(4) : {
      KEEP(*(.multiboot))
    } :text

    .text : {
        *(.text)
    } :text

    . = ALIGN(4K);

    .rodata : {
        *(.rodata*)
    } :rodata

    .builtin_drivers ALIGN(4) : {
        __builtin_drivers_start = .;
        KEEP(*(.builtin_drivers))
        __builtin_drivers_end = .;
    } :rodata

    . = ALIGN(4K);

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
        *(COMMON)
    } :data
}
