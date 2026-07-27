#ifndef IDT_H
  #define IDT_H

  #include <stdint.h>

  void idt_register(int n, uint32_t handler);

  void idt_load();
#endif
