#include "isr.h"
#include "idt.h"
#include "drivers/ports.h"
#include "drivers/vga/text.h"

// PIC Ports
#define PIC1_CMD 0x20
#define PIC1_DAT 0x21
#define PIC2_CMD 0xA0
#define PIC2_DAT 0xA1

#define PIC_EOI 0x20
#define ICW1 0x11

void pic_send_eoi(uint32_t int_no) {
  if (int_no >= 40) {
    port_write8(PIC2_CMD, PIC_EOI);
  }
  if (int_no >= 32) {
    port_write8(PIC1_CMD, PIC_EOI);
  }
}

void isr_install() {
  // ISRs
  idt_register( 0, (uint32_t)isr0);
  idt_register( 1, (uint32_t)isr1);
  idt_register( 2, (uint32_t)isr2);
  idt_register( 3, (uint32_t)isr3);
  idt_register( 4, (uint32_t)isr4);
  idt_register( 5, (uint32_t)isr5);
  idt_register( 6, (uint32_t)isr6);
  idt_register( 7, (uint32_t)isr7);
  idt_register( 8, (uint32_t)isr8);
  idt_register( 9, (uint32_t)isr9);
  idt_register(10, (uint32_t)isr10);
  idt_register(11, (uint32_t)isr11);
  idt_register(12, (uint32_t)isr12);
  idt_register(13, (uint32_t)isr13);
  idt_register(14, (uint32_t)isr14);
  idt_register(15, (uint32_t)isr15);
  idt_register(16, (uint32_t)isr16);
  idt_register(17, (uint32_t)isr17);
  idt_register(18, (uint32_t)isr18);
  idt_register(19, (uint32_t)isr19);
  idt_register(20, (uint32_t)isr20);
  idt_register(21, (uint32_t)isr21);
  idt_register(22, (uint32_t)isr22);
  idt_register(23, (uint32_t)isr23);
  idt_register(24, (uint32_t)isr24);
  idt_register(25, (uint32_t)isr25);
  idt_register(26, (uint32_t)isr26);
  idt_register(27, (uint32_t)isr27);
  idt_register(28, (uint32_t)isr28);
  idt_register(29, (uint32_t)isr29);
  idt_register(30, (uint32_t)isr30);
  idt_register(31, (uint32_t)isr31);

  // Configuring the PIC
  port_write8(PIC1_CMD, ICW1);
  port_write8(PIC2_CMD, ICW1);
  // ICW2: IDT offset
  port_write8(PIC1_DAT, 0x20);
  port_write8(PIC2_DAT, 0x28);
  // ICW3: wiring between the PICs
  port_write8(PIC1_DAT, 0x04);
  port_write8(PIC2_DAT, 0x02);
  // ICW4: mode (we enable 8086 mode)
  port_write8(PIC1_DAT, 0x01);
  port_write8(PIC2_DAT, 0x01);
  // OCW1: masking (we enable all IRQs)
  port_write8(PIC1_DAT, 0x00);
  port_write8(PIC2_DAT, 0x00);

  // IRQs
  idt_register(32, (uint32_t)irq0);
  idt_register(33, (uint32_t)irq1);
  idt_register(34, (uint32_t)irq2);
  idt_register(35, (uint32_t)irq3);
  idt_register(36, (uint32_t)irq4);
  idt_register(37, (uint32_t)irq5);
  idt_register(38, (uint32_t)irq6);
  idt_register(39, (uint32_t)irq7);
  idt_register(40, (uint32_t)irq8);
  idt_register(41, (uint32_t)irq9);
  idt_register(42, (uint32_t)irq10);
  idt_register(43, (uint32_t)irq11);
  idt_register(44, (uint32_t)irq12);
  idt_register(45, (uint32_t)irq13);
  idt_register(46, (uint32_t)irq14);
  idt_register(47, (uint32_t)irq15);

  // finish config
  idt_load();
}

const char* exception_msg[] = {
  "Division by zero",
  "Debug",

  "Reserved"
};

void print_uint32(uint32_t i) {
  int32_t shift = 28;
  while (shift >= 0) {
    uint8_t hex = (i >> shift) & 0xF;
    if (hex < 10) {
      kput('0' + hex, -1, -1);
    } else if (hex < 16) {
      kput('A' - 10 + hex, -1, -1);
    }
    shift -= 4;
  }
}

void isr_handler(isr_regs_t* regs) {
  if (regs->int_no >= 32) {
    pic_send_eoi(regs->int_no);
    kprint("IRQ received\n", -1, -1);
    return;
  }


  const char* msg = "Out of range for int number"; // default
  if (regs->int_no < sizeof(exception_msg) / sizeof(exception_msg[0])) {
    msg = exception_msg[regs->int_no];
  }

  kprint("Interrupt:\nException: ", -1, -1);
  kprint(msg, -1, -1);

  kprint("\nregs->int_no: ", -1, -1);
  print_uint32(regs->int_no);

  kprint("\nregs->err_no: ", -1, -1);
  print_uint32(regs->err_no);

  kprint("\nregs->eip: ", -1, -1);
  print_uint32(regs->eip);

  kprint("\nregs->cs: ", -1, -1);
  print_uint32(regs->cs);
  kput('\n', -1, -1);
}
