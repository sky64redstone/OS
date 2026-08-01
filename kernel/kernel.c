#include "drivers/vga/text.h"
#include "cpu/isr.h"
#include "version.h"
#include "kio.h"

void kmain() {
  vga_clear_screen();

  stdout.put = vga_kput;
  stdout.print = vga_kprint;

  kprint(str_welcome);

  isr_install();

  while (1) {
    asm volatile("hlt");
  }
}
