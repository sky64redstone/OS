#include "idt.h"

#define low16(x)  (uint16_t)((x) & 0xFFFF)
#define high16(x) (uint16_t)(((x) >> 16) & 0xFFFF)

#define IDT_SIZE 256

typedef struct {
  uint16_t low_offset;
  uint16_t selector;
  uint8_t  always0;
  uint8_t  flags;
  uint16_t high_offset;
} __attribute__((packed)) idt_gate_t;

typedef struct {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt_ptr_t;

idt_gate_t idt[IDT_SIZE];

void idt_register(int n, uint32_t handler) {
  idt[n].low_offset  = low16(handler);
  idt[n].selector    = 0x08; // see GDT (boot/src/bootloader.asm:~188)
  idt[n].always0     = 0;
  idt[n].flags       = 0b10001110;
  idt[n].high_offset = high16(handler);
}

extern void load_idt(idt_ptr_t* ptr);

void idt_load() {
  /*unsigned long idt_ptr[2];
  unsigned long idt_addr = (unsigned long)idt;
	idt_ptr[0] = (sizeof (idt_gate_t) * IDT_SIZE) + (low16(idt_addr) << 16);
	idt_ptr[1] = idt_addr >> 16;*/

  idt_ptr_t idt_ptr;

  idt_ptr.limit = sizeof(idt_gate_t) * IDT_SIZE - 1;
  idt_ptr.base = (uint32_t)idt;

  load_idt(&idt_ptr);
}
