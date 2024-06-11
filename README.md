# OS
A learning by doing Operatig System <br/>
TODO: Better Name

## How to
This operating system comes with a compiler configuration for windows and mingw64. <br/>
On Windows:<br/>
    Go in the cmd to the current Directory and type "clear", that should create the folder build. <br/>
    Then run "compile" and after that run "run"
<br/><br/>On Linux~<br/>
    Work in Progress

## Bootloader
This OS uses a self-written bootloader (see src/boot/)
It uses NASM as Assembly Language and boots the c-file src/kernel/kernel.c, which contains the entry-method kmain() <br/>
NOTE: the linking of the bootloader with the c-source-code doesn't work on windows with the batch files!!!

## Kernel
The Kernel is located in src/kernel/ and for now it just tries to print a x out (see src/kernel/kernel.c -> kmain() {...})

## Dependencies
mingw64 for compilation -> I used winlibs.com (NO liability for any damage)<br/>
qemu for emulation
