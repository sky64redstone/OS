#include "drivers/vga/text.h"
#include "version.h"

void kmain() {
  clear_screen();

  kprint(str_welcome, -1, -1);
}
