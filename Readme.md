# OS
An Operating System for x86 architectures. Written in C and assembly (Intel Syntax)

![screenshot of the OS running](https://github.com/sky64redstone/OS/blob/master/screenshot.png?raw=true)

## Compiling
This project uses Makefiles with gcc, binutils and nasm to compile.<br>
Tools you need: <br>
- `gcc`
- `ld`
- `nasm`
- `objcopy`
- `qemu-system-i386`
- `grub-file`
- `xorriso`
- `mformat`

On arch linux:
```
# pacman -S gcc binutils nasm qemu-desktop grub xorriso mtools
```

Build and run the project
```
$ make
```
Remove build files
```
$ make clean
```
Disassemble the operating system
```
$ make dis
```

## Components
### Bootloader
The bootloader is located in the `boot/` git submodule.

### Kernel
The kernel is located in the `kernel/` directory.<br>
kmain() is the kernel entry and is implemented in the file kernel/kernel.c

### CPU
Cpu architecture specific code is located in `cpu/`

### Drivers
The drivers are located in the `drivers/` directory.<br>
