reset=`tput sgr0`
green=`tput setaf 2`
yellow=`tput setaf 3`
blue=`tput setaf 4`
purple=`tput setaf 5`
red=`tput setaf 9`

asmfiles=cpu/interrupts.asm
cfiles=	kernel/kernel.c kernel/kio.c \
		drivers/ports.c drivers/vga/text.c drivers/ps2/keyboard.c \
		cpu/idt.c cpu/isr.c

cflags=-m32 -ffreestanding -nostdlib -fno-pic -fno-stack-protector -I.
ldflags=-m elf_i386 -T linker.ld -nostdlib

ofiles=$(patsubst %.asm,build/%.asm.o,$(asmfiles)) $(patsubst %.c,build/%.c.o,$(cfiles))

run: build/image.bin
	@echo -e $(green)Running on x86_64 hardware$(reset)...
	@qemu-system-x86_64 -no-reboot -drive format=raw,file=$<

debug: build/image.bin
	@echo -e $(green)Debugging on x86_64 hardware$(reset)...
	@qemu-system-x86_64 -monitor stdio -drive format=raw,file=$<

build: build/image.bin
	@echo -e $(green)Built for x86_64 hardware$(reset)

dis: build/image.bin
	@echo -e [txt] $(yellow)Disassembling kernel.elf in build/kernel.dis$(reset)...
	@objdump -M intel -d build/kernel.elf > build/kernel.dis
	@echo -e [txt] $(yellow)Disassembling the image in build/image.dis$(reset)...
	@ndisasm -b 32 $< > build/image.dis

clean:
	@rm -rf build/

build/image.bin: build/boot/src/bootloader.bin build/kernel.bin
	@echo -e [bin] $(red)Combining the kernel and bootloader$(reset)...
	@cat $^ > $@
	@dd if=/dev/zero bs=512 count=1 >> $@ 2>/dev/null

build/kernel.bin: build/boot/src/kernel-entry.asm.o ${ofiles}
	@echo -e [elf] $(purple)Linking the kernel$(reset)...
	@ld ${ldflags} $^ -o build/kernel.elf
	@echo -e [bin] $(purple)Stripping ELF metadata$(reset)...
	@objcopy -O binary build/kernel.elf $@

build/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo -e [elf] $(blue)Compiling$(reset) $<...
	@gcc ${cflags} -c $< -o $@

build/%.asm.o: %.asm
	@mkdir -p $(dir $@)
	@echo -e [elf] $(yellow)Assembling$(reset) $<...
	@nasm $< -f elf32 -o $@

build/%.bin: %.asm
	@mkdir -p $(dir $@)
	@echo -e [bin] $(yellow)Assembling$(reset) $<...
	@nasm $< -f bin -o $@
