@echo off

set stdRoot=%~dp0

:: check first if the build directory exists
if exist build\ (
    echo deleting image files...
    cd build\ && del /q *.img && cd %stdRoot%

    :: Go to the boot directory and delete every file
    if exist build\boot\ (
        echo deleting bootloader files...
        cd build\boot\ && del /q * && cd %stdRoot%
    ) else (
        :: Or else make the build bootloader folder
        echo make bootloader build folder...
        cd build && mkdir boot && cd %stdRoot%
    )

    :: Go to the boot directory and delete every file
    if exist build\kernel\ (
        echo deleting kernel files...
        cd build\kernel\ && del /q * && cd %stdRoot%
    ) else (
        :: Or else make the build kernel folder
        echo make kernel build folder...
        cd build && mkdir kernel && cd %stdRoot%
    )
) else (
    :: Make the build folder
    mkdir build
)
cd %stdRoot%
