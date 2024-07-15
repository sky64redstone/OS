#
# commands:
#
# make          - build & run the os
# make run      - run existing build on i386 arch
# make run-i386 - run existing build on i386 arch
# make run-x86  - run existing build on x86_64 arch
# make rebuild  - clear existing build & build it again
# make build    - only update the build
# make clean    - remove existing build
# make help     - prints out a help message
#

# var's section
C_SOURCES = $(wildcard src/kernel/*.c src/drivers/*.c)
C_HEADERS = $(wildcard src/kernel/*.h src/drivers/*.h)
C_OBJ     = $(C_SOURCES:.c=.o)
CCOMPILER = gcc
ASSEMBLER = nasm
LINKER    = ld
CFLAGS    =

# first rule is run by default
default: build/os-image run

# help section
help:
	$(info #-------------------------------------------------------#)
	$(info | make commands:                                        |)
	$(info #-------------------------------------------------------#)
	$(info | make          - build & run the os                    |)
	$(info | make run      - run existing build on i386 arch       |)
	$(info | make run-i386 - run existing build on x86_64          |)
	$(info | make rebuild  - clear existing build & build it again |)
	$(info | make build    - only update the build                 |)
	$(info | make clean    - remove existing build                 |)
	$(info | make help     - prints out a help message             |)
	$(info #-------------------------------------------------------#)

# run section
run: run-i386

run-i386: build/os-image
	qemu-system-i386 -fda $^

run-x86: build/os-image
	qemu-system-x86_64 -fda $^

# build section
rebuild: clean build

build: build/os-image

build/os-image: build/boot/boot_sect.bin build/kernel/kernel.bin
	cat $^ > $@

build/kernel/kernel.bin: build/boot/kernel-entry.o ${C_OBJ}
	${LINKER} -o $@ -Ttext 0x1000 $^ --oformat binary

#build/boot/kernel-entry.o: src/boot/kernel-entry.asm
#	${ASSEMBLER} $^ -f elf64 -o $@

#build/boot/boot_sect.bin: src/boot/boot_sect.asm
#	${ASSEMBLER} $^ -f bin -o $@

# clear section
.PHONY: clean
clean:
	@rm -rf build/*.bin build/*.o build/os-image
	@rm -rf build/boot/*.bin build/boot/*.o
	@rm -rf build/kernel/*.bin build/kernel/*.o
	@rm -rf build/kernel/*.o build/kernel/*.bin
	@rm -rf build/drivers/*.o build/drivers/*.bin
	@echo finished cleaning...

# rules section
%.o: %.c ${C_HEADERS}
	${CCOMPILER} ${CFLAGS} -ffreestanding -c $< -o $@

build/%.o: src/%.asm
	${ASSEMBLER} $< -f elf64 -o $@

build/%.bin: src/%.asm
	${ASSEMBLER} $< -f bin -o $@
