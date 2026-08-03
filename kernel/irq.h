#ifndef KERNEL_IRQ_H
  #define KERNEL_IRQ_H

  #include <stdint.h>

  #include "cpu/isr.h"

  #define IRQ_COUNT 16

  enum irq_flag {
    IRQF_NONE   = 0<<0,
    IRQF_SHARED = 1<<0
  };

  enum irq_result {
    IRQ_NONE,
    IRQ_HANDLED
  };

  typedef enum irq_result (*irq_handler_t)(
    uint32_t vector, void* dev_id, isr_regs_t* regs
  );

  struct irq_chip {
    const char* name;

    void (*mask)  (uint32_t vector);
    void (*unmask)(uint32_t vector);
    void (*eoi)   (uint32_t vector);
  };

  int irq_init(struct irq_chip* chip);

  int irq_request(
    uint32_t vector, irq_handler_t handler, uint32_t flags,
    const char* name, void* dev_id
  );

  int irq_free(uint32_t vector, void* dev_id);

  void irq_mask(uint32_t vector);
  void irq_unmask(uint32_t vector);

  void irq_dispatch(uint32_t vector, isr_regs_t* regs);

#endif
