OUTPUT_FORMAT(elf32-i386)
ENTRY(kmain)        /* entry point */
SECTIONS {
    . = 0x1000;       /* load address, same as in src/bootloader.asm*/

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
