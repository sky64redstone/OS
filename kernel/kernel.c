#include "drivers/vga/text.h"

#define str_welcome \
"   ___  ____   :\n" \
"  / _ \\/ ___|  :\n" \
" | | | \\___ \\  :\n" \
" | |_| |___) | :\n" \
"  \\___/|____/  : Self-written operating system in c and asm\n" \
"...............: version 0.0.0\n"

void kmain() {
  clear_screen();

  kprint(str_welcome, -1, -1);
}
