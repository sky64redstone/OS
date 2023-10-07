@echo off

set stdRoot=%~dp0
cd %stdRoot%

:: compile the boot-sect file
echo compile boot-sect assembly file...
"mingw64/bin/nasm" src/boot/boot-sect.asm -f bin -o build/boot/boot-sect.bin

:: compile the kernel-entry file
echo compile kernel-entry assembly file...
:: -f elf (elf == executable and linking format)
"mingw64/bin/nasm" src/boot/kernel-entry.asm -f win32 -o build/boot/kernel-entry.o

:: compile the kernel.c file
echo compile kernel.c file...
:: added -nostdlib -nostdinc
"mingw64/bin/gcc" -m32 -c src/kernel/kernel.c -o build/kernel/kernel.o -ffreestanding -nostdlib -nostdinc

:: linking the kernel.o file
echo linking kernel.o file...
:: 1.
::"mingw64/bin/ld" -o build/kernel/kernel.bin -Ttext 0x1000 build/boot/kernel-entry.o build/kernel/kernel.o --oformat binary

:: 2.
::"mingw64/bin/ld" -m elf_i386 -s -o build/kernel/kernel.bin build/boot/kernel-entry.o build/kernel/kernel.o

:: 3.
::"mingw64/bin/ld" -T link.ld -o build/kernel/kernel.temp build/boot/kernel-entry.o build/kernel/kernel.o -build-id=none
::"mingw64/bin/objcopy" -O elf32-i386 build/kernel/kernel.temp build/kernel/kernel.bin

:: 4.
"mingw64/bin/ld" -r -m i386pe -T NUL -o build/kernel/kernel.temp -Ttext 0x100000 build/boot/kernel-entry.o build/kernel/kernel.o

echo copying kernel.temp file...
"migw64/bin/objcopy" -O binary build/kernel/kernel.temp build/kernel/kernel.bin
::"migw64/x86_64-w64-mingw32/bin/objcopy" -O binary build/kernel/kernel.temp build/kernel/kernel.bin

:: Possible Solutions
::https://stackoverflow.com/questions/30939593/mingws-ld-cannot-perform-pe-operations-on-non-pe-output-file
::https://9to5answer.com/architecture-of-i386-input-file-is-incompatible-with-i386-x86-64
::https://stackoverflow.com/questions/32279598/how-to-compile-and-link-c-and-asm-together-on-windows-for-my-os

:: create floppy image
echo create floppy image file
type build/boot/boot-sect.bin, build/kernel/kernel.bin > build/os-image.img

pause
