CC = gcc
AS = nasm
DAS = ndisasm
LD = ld
OBJDUMP = objdump
GDB = gdb
QEMU = qemu-system-i386
GRUB_FILE = grub-file
GRUB_MKRESCUE = grub-mkrescue

PRE = @

REQUIRED_TOOLS := $(CC) $(AS) $(LD) $(QEMU) $(GRUB_FILE) $(GRUB_MKRESCUE) \
			xorriso mformat

asmfiles = \
	boot/entry.asm \
	cpu/interrupts.asm

cfiles = \
	kernel/kernel.c \
	kernel/kio.c \
	kernel/irq.c \
	kernel/init.c \
	kernel/device.c \
	kernel/shell.c \
	drivers/ports.c \
	drivers/vga/text.c \
	drivers/ps2/keyboard.c \
	drivers/ps2/ps2.c \
	cpu/idt.c \
	cpu/isr.c \
	cpu/pic.c

cflags = \
	-m32 \
	-ffreestanding \
	-nostdlib \
	-fno-pic \
	-fno-stack-protector \
	-fno-asynchronous-unwind-tables \
	-fno-unwind-tables \
	-fno-pie \
	-Wall \
	-Wextra \
	-Werror \
	-Wstrict-prototypes \
	-g \
	-O0 \
	-I.

ldflags = \
	-m elf_i386 \
	-T linker.ld \
	-nostdlib \
	-z max-page-size=0x1000

qemu_flags = \
	-cdrom build/os.iso \
	-no-reboot \
	-no-shutdown

qemu_debug_flags = \
	$(qemu_flags) \
	-S \
	-gdb tcp::1234 \
	-d int,cpu_reset,guest_errors \
	-D build/qemu.log

reset=$(shell tput sgr0)
green=$(shell tput setaf 2)
yellow=$(shell tput setaf 3)
blue=$(shell tput setaf 4)
purple=$(shell tput setaf 5)
red=$(shell tput setaf 9)

.PHONY: build run debug clean dis check-tools

ofiles = \
	$(patsubst %.asm,build/%.asm.o,$(asmfiles)) \
	$(patsubst %.c,build/%.c.o,$(cfiles))

run: build/os.iso
	@echo -e "$(green)Running$(reset) on i386 architecture..."
	$(PRE)$(QEMU) $(qemu_flags)

debug: build/os.iso build/kernel.elf
	@echo -e "$(green)Debugging$(reset) on i386 architecture..."
	$(PRE)$(QEMU) $(qemu_debug_flags) &
	$(PRE)$(GDB) \
		-ex "set architecture i386" \
		-ex "symbol-file build/kernel.elf" \
		-ex "target remote localhost:1234"

build: build/os.iso
	@echo -e "$(green)Built$(reset) for i386/x86 hardware"

dis: build/kernel.elf
	@echo -e "$(yellow)Disassembling$(reset) kernel.elf..."
	$(PRE)$(OBJDUMP) -M intel -d build/kernel.elf > build/kernel.dis

clean:
	$(PRE)rm -rf build/

check-tools:
	@echo -e "$(yellow)Checking$(reset) tools..."
	@missing=0; \
	for tool in $(REQUIRED_TOOLS); do \
		if command -v "$$tool" >/dev/null 2>&1; then \
			printf ' $(green)found$(reset): %s -> %s\n' "$$tool" "$$(command -v "$$tool")"; \
		else \
			printf '$(red)missing$(reset): %s\n' "$$tool"; \
			missing=1; \
		fi; \
	done; \
	if [ "$$missing" -ne 0 ]; then \
		printf '\nPlease install the missing packages or check PATH:\n'; \
		printf 'PATH: $(PATH)\n'; \
		exit 1; \
	fi
	@echo -e "$(green)All tools available$(reset)"

build/os.iso: check-tools build/kernel.elf boot/grub/grub.cfg
	@echo -e "$(yellow)Creating$(reset) GRUB ISO..."
	$(PRE)mkdir -p build/isofiles/boot/grub
	$(PRE)cp build/kernel.elf build/isofiles/boot/kernel.elf
	$(PRE)cp boot/grub/grub.cfg build/isofiles/boot/grub/grub.cfg
	$(PRE)$(GRUB_FILE) --is-x86-multiboot build/kernel.elf
	$(PRE)$(GRUB_MKRESCUE) -o $@ build/isofiles

build/kernel.elf: linker.ld $(ofiles)
	@echo -e "$(purple)Linking$(reset) kernel..."
	$(PRE)$(LD) $(ldflags) $(ofiles) -o $@

build/%.c.o: %.c
	$(PRE)mkdir -p $(dir $@)
	@echo -e "$(blue)Compiling$(reset) $<..."
	$(PRE)$(CC) $(cflags) -c $< -o $@

build/%.asm.o: %.asm
	$(PRE)mkdir -p $(dir $@)
	@echo -e "$(yellow)Assembling$(reset) $<..."
	$(PRE)$(AS) $< -f elf32 -o $@
