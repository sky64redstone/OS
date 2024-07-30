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
C_SOURCES = $(wildcard src/kernel/*.c   src/drivers/*.c)
C_HEADERS = $(wildcard src/kernel/*.h   src/drivers/*.h)
OBJECTS   = $(C_SOURCES:.c=.o)
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
	qemu-system-i386 $^

run-x86: build/os-image
	qemu-system-x86_64 $^

# build section
rebuild: clean build

build: build/os-image

build/os-image: build/boot/boot_sect.bin build/kernel/kernel.bin
	cat $^ > $@

build/kernel/kernel.bin: build/boot/kernel-entry.o ${OBJECTS}
	${LINKER} -o $@ -Ttext 0x1000 $^ --oformat binary

# clear section
clean:
	@rm -rf build/os-image
	@rm -rf build/*.bin        build/*.o
	@rm -rf build/boot/*.bin   build/boot/*.o
	@rm -rf build/kernel/*.bin build/kernel/*.o
	@rm -rf src/kernel/*.o     src/kernel/*.bin
	@rm -rf src/drivers/*.o    src/drivers/*.bin
	@echo finished cleaning...

# rules section
%.o: %.c
	${CCOMPILER} ${CFLAGS} -ffreestanding -c $< -o $@

build/%.o: src/%.asm
	${ASSEMBLER} $< -f elf64 -o $@

build/%.bin: src/%.asm
	${ASSEMBLER} $< -f bin -o $@
