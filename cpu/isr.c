#include "isr.h"
#include "idt.h"
#include "drivers/ports.h"
#include "drivers/vga/text.h"
#include "kernel/kio.h"
#include "kernel/irq.h"

#define IDT_INT 0x8E

void isr_install() {
  /* ISRs */
  idt_register( 0, (uint32_t)isr0, IDT_INT);
  idt_register( 1, (uint32_t)isr1, IDT_INT);
  idt_register( 2, (uint32_t)isr2, IDT_INT);
  idt_register( 3, (uint32_t)isr3, IDT_INT);
  idt_register( 4, (uint32_t)isr4, IDT_INT);
  idt_register( 5, (uint32_t)isr5, IDT_INT);
  idt_register( 6, (uint32_t)isr6, IDT_INT);
  idt_register( 7, (uint32_t)isr7, IDT_INT);
  idt_register( 8, (uint32_t)isr8, IDT_INT);
  idt_register( 9, (uint32_t)isr9, IDT_INT);
  idt_register(10, (uint32_t)isr10, IDT_INT);
  idt_register(11, (uint32_t)isr11, IDT_INT);
  idt_register(12, (uint32_t)isr12, IDT_INT);
  idt_register(13, (uint32_t)isr13, IDT_INT);
  idt_register(14, (uint32_t)isr14, IDT_INT);
  idt_register(15, (uint32_t)isr15, IDT_INT);
  idt_register(16, (uint32_t)isr16, IDT_INT);
  idt_register(17, (uint32_t)isr17, IDT_INT);
  idt_register(18, (uint32_t)isr18, IDT_INT);
  idt_register(19, (uint32_t)isr19, IDT_INT);
  idt_register(20, (uint32_t)isr20, IDT_INT);
  idt_register(21, (uint32_t)isr21, IDT_INT);
  idt_register(22, (uint32_t)isr22, IDT_INT);
  idt_register(23, (uint32_t)isr23, IDT_INT);
  idt_register(24, (uint32_t)isr24, IDT_INT);
  idt_register(25, (uint32_t)isr25, IDT_INT);
  idt_register(26, (uint32_t)isr26, IDT_INT);
  idt_register(27, (uint32_t)isr27, IDT_INT);
  idt_register(28, (uint32_t)isr28, IDT_INT);
  idt_register(29, (uint32_t)isr29, IDT_INT);
  idt_register(30, (uint32_t)isr30, IDT_INT);
  idt_register(31, (uint32_t)isr31, IDT_INT);

  /* IRQs */
  idt_register(32, (uint32_t)irq0, IDT_INT);
  idt_register(33, (uint32_t)irq1, IDT_INT);
  idt_register(34, (uint32_t)irq2, IDT_INT);
  idt_register(35, (uint32_t)irq3, IDT_INT);
  idt_register(36, (uint32_t)irq4, IDT_INT);
  idt_register(37, (uint32_t)irq5, IDT_INT);
  idt_register(38, (uint32_t)irq6, IDT_INT);
  idt_register(39, (uint32_t)irq7, IDT_INT);
  idt_register(40, (uint32_t)irq8, IDT_INT);
  idt_register(41, (uint32_t)irq9, IDT_INT);
  idt_register(42, (uint32_t)irq10, IDT_INT);
  idt_register(43, (uint32_t)irq11, IDT_INT);
  idt_register(44, (uint32_t)irq12, IDT_INT);
  idt_register(45, (uint32_t)irq13, IDT_INT);
  idt_register(46, (uint32_t)irq14, IDT_INT);
  idt_register(47, (uint32_t)irq15, IDT_INT);

  idt_clear(48, -1);

  /* finish config */
  idt_load();
}

const char* exception_msg[] = {
  "Division by zero",
  "Debug",
  "Non-maskable",
  "Breakpoint",
  "Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Invalid TSS",
  "Segment Not Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  "Reserved",
  "Floating Point",
  "Alignment Check",
  "Machine Check",
  "SIMD Floating Point",
  "Virtualization",
  "Control Protection"
};

void isr_handler(isr_regs_t* regs) {
  if (regs->int_no >= 32 && regs->int_no < 48) {
    irq_dispatch(regs->int_no - 32, regs);
    return;
  }

  const char* msg = "Reserved"; /* default */
  if (regs->int_no < sizeof(exception_msg) / sizeof(exception_msg[0])) {
    msg = exception_msg[regs->int_no];
  }
  
  kprintf(
    "Interrupt: %s\n"
    "regs->int_no: %x\n"
    "regs->err_no: %x\n"
    "regs->eip: %x\n"
    "regs->cs: %x\n",
    msg, regs->int_no,
    regs->err_no,regs->eip,
    regs->cs
  );

  while (1) {
    asm volatile("cli; hlt");
  }
}
