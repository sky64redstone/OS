reset=`tput sgr0`
green=`tput setaf 2`
yellow=`tput setaf 3`
blue=`tput setaf 4`
purple=`tput setaf 5`
red=`tput setaf 9`

CC = gcc
AS = nasm
DAS = ndisasm
LD = ld
OBJCOPY = objcopy
OBJDUMP = objdump
GDB = gdb
QEMU = qemu-system-i386
PRE = @

asmfiles=cpu/interrupts.asm
cfiles=	kernel/kernel.c kernel/kio.c kernel/irq.c kernel/init.c kernel/device.c \
		drivers/ports.c drivers/vga/text.c drivers/ps2/keyboard.c drivers/ps2/ps2.c \
		cpu/idt.c cpu/isr.c cpu/pic.c

cflags=-m32 -ffreestanding -nostdlib -fno-pic \
	-fno-stack-protector -Wall -Wextra -Werror \
	-fno-asynchronous-unwind-tables -fno-unwind-tables \
	-Wstrict-prototypes -g -O0 -fno-pie -I.
ldflags=-m elf_i386 -T linker.ld -nostdlib

qemu_flags = \
	-drive format=raw,file=build/image.bin \
	-no-reboot \
	-no-shutdown

qemu_debug_flags = \
	$(qemu_flags) \
	-S \
	-gdb tcp::1234 \
	-d int,cpu_reset,guest_errors \
	-D build/qemu.log

.PHONY: build run debug debug-qemu clean

ofiles=$(patsubst %.asm,build/%.asm.o,$(asmfiles)) $(patsubst %.c,build/%.c.o,$(cfiles))

run: build/image.bin
	@echo -e $(green)Running$(reset) on i386 architecture...
	$(PRE)$(QEMU) $(qemu_flags)

debug: build/image.bin build/kernel.elf
	@echo -e $(green)Debugging$(reset) on i386 architecture...
	$(PRE)$(QEMU) $(qemu_debug_flags) &
	$(PRE)$(GDB) \
		-ex "set architecture i386" \
		-ex "symbol-file build/kernel.elf" \
		-ex "target remote localhost:1234"

build: build/image.bin
	@echo -e $(green)Built$(reset) for i386/x86 hardware

dis: build/image.bin
	@echo -e [txt] $(yellow)Disassembling kernel.elf in build/kernel.dis$(reset)...
	$(PRE)$(OBJDUMP) -M intel -d build/kernel.elf > build/kernel.dis
	@echo -e [txt] $(yellow)Disassembling the image in build/image.dis$(reset)...
	$(PRE)$(DAS) -b 32 $< > build/image.dis

clean:
	$(PRE)rm -rf build/

build/image.bin: build/boot/src/bootloader.asm.bin build/kernel.bin
	@echo -e [bin] $(red)Combining the kernel and bootloader$(reset)...
	$(PRE)cat $^ > $@
	@size=$$(stat -c '%s' $@); \
	sectors=$$(((size + 511) / 512)); \
	echo -e "$(purple)Image size$(reset): $$size bytes ($$sectors sectors)"
	@echo -e [$(yellow)WARN$(reset)] $(red)DON\'T FORGET TO LOAD ALL SECTORS$(reset)
	@echo -e See boot/src/bootloader.asm line ~29 set dh to sector count
	$(PRE)dd if=/dev/zero bs=512 count=1 >> $@ 2>/dev/null

build/kernel.bin: build/boot/src/kernel-entry.asm.o ${ofiles}
	@echo -e [elf] $(purple)Linking the kernel$(reset)...
	$(PRE)$(LD) ${ldflags} $^ -o build/kernel.elf
	@echo -e [bin] $(purple)Stripping ELF metadata$(reset)...
	$(PRE)$(OBJCOPY) -O binary build/kernel.elf $@

build/%.c.o: %.c
	$(PRE)mkdir -p $(dir $@)
	@echo -e [elf] $(blue)Compiling$(reset) $<...
	$(PRE)$(CC) ${cflags} -c $< -o $@

build/%.asm.o: %.asm
	$(PRE)mkdir -p $(dir $@)
	@echo -e [elf] $(yellow)Assembling$(reset) $<...
	$(PRE)$(AS) $< -f elf32 -o $@

build/%.asm.bin: %.asm
	$(PRE)mkdir -p $(dir $@)
	@echo -e [bin] $(yellow)Assembling$(reset) $<...
	$(PRE)$(AS) $< -f bin -o $@
