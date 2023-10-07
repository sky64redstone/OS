@echo off

:: the file with the os
set file=build/os-image.img

:: check first if the file exists
if exist %file% (
    echo Emulating a Intel 386 Core
    :: emulates a intel 386 CPU
    qemu-system-i386 %file%
) else (
    echo Could not find the right file
)
