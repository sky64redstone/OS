# OS
A learning by doing Operating System for x84_64 computers. Written in C and assembly (Intel Syntax)

## Compiling
This project uses Makefiles with gcc, binutils and nasm to compile.<br>
Tools you need: <br>
- `gcc`
- `ld`
- `nasm`
- `objcopy`
- `qemu-system-x86_64`

On arch linux:
```
sudo pacman -S gcc binutils nasm qemu-desktop
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
The bootloader is located in the boot/ git submodule.

### Kernel
The kernel is located in the kernel/ directory.<br>
kmain() is the kernel entry and is implemented in the file kernel/kernel.c

### Drivers
The drivers are located in the drivers/ directory.<br>

## Troubleshooting
### The kernel has unexpected problems / The BIOS restarts always
The kernel may not have been loaded fully. Try increasing the number of loaded
sectors in the boot/src/bootloader.asm file.
It is recommended to set the register dh to x, with: `x * 512 = sizeof(kernel.bin)`
```nasm
  mov si, init_msg
  call print

  ; HERE: (~line 30)
  mov dh, 3             ; load dh sectors
  mov dl, [BOOT_DRIVE]  ; select our boot drive
  mov bx, 0             ; set es indirectly to 0
  mov es, bx
  mov bx, KERNEL_OFFSET ; load it at address KERNEL_OFFSET
  call disk_load
```
