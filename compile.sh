echo boot sector compiling...
nasm src/boot/boot_sect.asm -f bin -o build/boot/boot_sect.bin

echo kernel compiling...
gcc -ffreestanding -c src/kernel/kernel.c -o build/kernel/kernel.o
ld -o build/kernel/kernel.bin -Ttext 0x1000 build/kernel/kernel.o --oformat binary

echo merging bootloader and kernel...
cat build/boot/boot_sect.bin build/kernel/kernel.bin > build/os-image

echo build finished!
