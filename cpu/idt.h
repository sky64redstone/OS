#ifndef IDT_H
  #define IDT_H

  #include <stdint.h>

  void idt_register(int n, uint32_t handler, uint8_t flags);

  void idt_clear(int from, int to);

  void idt_load();
#endif
