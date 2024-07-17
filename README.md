# OS
A learning by doing Operatig System <br/>

## How to
This operating system comes with a Makefile that uses gcc, ld and nasm. <br/>
<br/><br/>Build:<br/>
    bash: make
<br/><br/>Get Help:<br/>
    bash: make help

## Bootloader
This OS uses a self-written bootloader (see src/boot/)
It uses NASM as Assembly Language and boots the c-file src/kernel/kernel.c, which contains the entry-method kmain() <br/>

## Kernel
The Kernel is located in src/kernel/

## Drivers
The different Drivers are located in src/drivers/

## Dependencies
gcc/nasm for compilation<br/>
ld for linking<br/>
qemu for emulation<br/>
    Arch: install qemu with bash: sudo pacman -S qemu-desktop (qemu-desktop fully works, you don't need to install the full qemu package)
