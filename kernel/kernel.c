#include "drivers/vga/text.h"

void kmain() {
  clear_screen();
  kprint("Hello World! :)", 0, 1);
}
