#include <stdint.h>

#include "pic.h"
#include "drivers/ports.h" /* TODO move mb into cpu/? */
#include "kernel/irq.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI 0x20

#define PIC_ICW1_INIT 0x10
#define PIC_ICW1_ICW4 0x01
#define PIC_ICW4_8086 0x01

#define PIC_MASTER_VECTOR 0x20
#define PIC_SLAVE_VECTOR  0x28

uint16_t pic_mask = 0xFFFF;

void pic_write_mask() {
  port_write8(PIC1_DATA, pic_mask & 0XFF);  
  port_write8(PIC2_DATA, (pic_mask >> 8) & 0XFF);  
}

void pic_mask_vector(uint32_t vector) {
  if (vector >= IRQ_COUNT) {
    return;
  }

  pic_mask |= (uint16_t)(1 << vector);

  /*
   * The slave PIC is connected through master IRQ 2.
   * Keep IRQ 2 enabled while any slave IRQ is enabled.
   */
  if ((pic_mask & 0xFF00) != 0xFF00) {
    pic_mask &= (uint16_t)~(1 << 2);
  }

  pic_write_mask();
}

void pic_unmask_vector(uint32_t vector) {
  if (vector >= IRQ_COUNT) {
    return;
  }

  pic_mask &= (uint16_t)(~(1 << vector));

  if (vector >= 8) {
    pic_mask &= (uint16_t)(~(1 << 2));
  }

  pic_write_mask();
}

void pic_eoi(uint32_t vector) {
  if (vector >= 8) {
    port_write8(PIC2_COMMAND, PIC_EOI);
  }

  port_write8(PIC1_COMMAND, PIC_EOI);
}

struct irq_chip pic_irq_chip = {
  .name = "i8259-pic",
  .mask = pic_mask_vector,
  .unmask = pic_unmask_vector,
  .eoi = pic_eoi
};

int pic_init() {
  uint8_t master_mask = port_read8(PIC1_DATA);
  uint8_t slave_mask = port_read8(PIC2_DATA);

  port_write8(
    PIC1_COMMAND,
    PIC_ICW1_INIT | PIC_ICW1_ICW4
  );
  port_write8(
    PIC2_COMMAND,
    PIC_ICW1_INIT | PIC_ICW1_ICW4
  );

  port_write8(PIC1_DATA, PIC_MASTER_VECTOR);
  port_write8(PIC2_DATA, PIC_SLAVE_VECTOR);

  port_write8(PIC1_DATA, 1 << 2);
  port_write8(PIC2_DATA, 2);

  port_write8(PIC1_DATA, PIC_ICW4_8086);
  port_write8(PIC2_DATA, PIC_ICW4_8086);

  (void)master_mask;
  (void)slave_mask;

  /*
   * Start with every hardware IRQ disabled.
   * request_irq() unmasks lines when a driver successfully claims them.
   */
  pic_mask = 0xFFFF;
  pic_write_mask();

  return irq_init(&pic_irq_chip);
}
