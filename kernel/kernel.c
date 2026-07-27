#include "drivers/vga/text.h"
#include "cpu/isr.h"
#include "version.h"

void kmain() {
  clear_screen();

  kprint(str_welcome, -1, -1);

  isr_install();

  while (1);

  kprint("hlt", 0, 24);
}
