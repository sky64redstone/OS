# OS
A learning by doing Operatig System <br/>

## How to
This operating system comes with a compiler configuration for windows and mingw64. <br/>
<br/><br/>On Linux:<br/>
    bash: sh compile.sh
    bash: sh run-i386.sh // or run-x86.sh

## Bootloader
This OS uses a self-written bootloader (see src/boot/)
It uses NASM as Assembly Language and boots the c-file src/kernel/kernel.c, which contains the entry-method kmain() <br/>

## Kernel
The Kernel is located in src/kernel/ and for now it just tries to print a x out (see src/kernel/kernel.c -> kmain() {...})

## Dependencies
gcc/nasm for compilation
qemu for emulation<br/>
    Arch: install qemu with bash: sudo pacman -S qemu-desktop
