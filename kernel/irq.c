#include "irq.h"

#define IRQ_ACTIONS_PER_LINE 4

struct irq_action {
  irq_handler_t handler;
  void* dev_id;
  const char* name;
  uint32_t flags;
  uint8_t used;
};

struct irq_descriptor {
  struct irq_action actions[IRQ_ACTIONS_PER_LINE];
  uint32_t handler_count;
};

struct irq_descriptor descriptors[IRQ_COUNT];
struct irq_chip* active_irq_chip;

uint32_t irq_save() {
  uint32_t flags;

  asm volatile(
    "pushf\n"
    "pop %0\n"
    "cli"
    : "=r"(flags)
    :
    : "memory"
  );

  return flags;
}

void irq_restore(uint32_t flags) {
  if ((flags & (1 << 9)) != 0) {
    asm volatile("sti" : : : "memory");
  }
}

int irq_init(struct irq_chip* chip) {
  if (chip == 0 || chip->mask == 0 || chip->unmask == 0 || chip->eoi == 0) {
    return -1;
  }

  active_irq_chip = chip;

  for (uint32_t vector = 0; vector < IRQ_COUNT; vector++) {
    descriptors[vector].handler_count = 0;

    for (uint32_t i = 0; i < IRQ_ACTIONS_PER_LINE; i++) {
      descriptors[vector].actions[i].handler = 0;
      descriptors[vector].actions[i].dev_id = 0;
      descriptors[vector].actions[i].name = 0;
      descriptors[vector].actions[i].flags = IRQF_NONE;
      descriptors[vector].actions[i].used = 0;
    }

    active_irq_chip->mask(vector);
  }

  return 0;
}

int irq_request(
  uint32_t vector, irq_handler_t handler, uint32_t flags,
  const char* name, void* dev_id
) {
  if (active_irq_chip == 0 || vector >= IRQ_COUNT || handler == 0) {
    return -1;
  }

  uint32_t saved_flags = irq_save();
  struct irq_descriptor* descriptor = &descriptors[vector];

  if (descriptor->handler_count != 0) {
    if ((flags & IRQF_SHARED) == 0) {
      irq_restore(saved_flags);
      return -2;
    }

    for (uint32_t i = 0; i < IRQ_ACTIONS_PER_LINE; i++) {
      struct irq_action* action = &descriptor->actions[i];

      if (action->used == 0) {
        continue;
      }

      if ((action->flags & IRQF_SHARED) == 0) {
        irq_restore(saved_flags);
        return -3;
      }

      if (action->dev_id == dev_id) {
        irq_restore(saved_flags);
        return -4;
      }
    } /* for */
  } /* if handler_count != 0 */
 
  for (uint32_t i = 0; i < IRQ_ACTIONS_PER_LINE; i++) {
    struct irq_action* action = &descriptor->actions[i];

    if (action->used != 0) {
      continue;
    }

    action->handler = handler;
    action->dev_id = dev_id;
    action->name = name;
    action->flags = flags;
    action->used = 1;
    
    descriptor->handler_count++;

    if(descriptor->handler_count == 1) {
      active_irq_chip->unmask(vector);
    }

    irq_restore(saved_flags);

    return 0;
  }

  irq_restore(saved_flags);
  return -5;
}

int irq_free(uint32_t vector, void* dev_id) {
  if (active_irq_chip == 0 || vector >= IRQ_COUNT) {
    return -1;
  }

  uint32_t saved_flags = irq_save();
  struct irq_descriptor* descriptor = &descriptors[vector];

  for (uint32_t i = 0; i < IRQ_ACTIONS_PER_LINE; i++) {
    struct irq_action* action = &descriptor->actions[i];

    if (action->used == 0 || action->dev_id != dev_id) {
      continue;
    }

    action->handler = 0;
    action->dev_id = 0;
    action->name = 0;
    action->flags = IRQF_NONE;
    action->used = 0;

    descriptor->handler_count--;

    if (descriptor->handler_count == 0) {
      active_irq_chip->mask(vector);
    }

    irq_restore(saved_flags);
    return 0;
  }

  irq_restore(saved_flags);
  return -2;
}

void irq_mask(uint32_t vector) {
  if (active_irq_chip == 0 || vector >= IRQ_COUNT) {
    return;
  }
  active_irq_chip->mask(vector);
}

void irq_unmask(uint32_t vector) {
  if (active_irq_chip == 0 || vector >= IRQ_COUNT) {
    return;
  }
  active_irq_chip->unmask(vector);
}

void irq_dispatch(uint32_t vector, isr_regs_t* regs) {
  if (active_irq_chip == 0 || vector >= IRQ_COUNT) {
    return;
  }

  struct irq_descriptor* descriptor = &descriptors[vector];
  uint8_t handled = 0;

  for (uint32_t i = 0; i < IRQ_ACTIONS_PER_LINE; i++) {
    struct irq_action* action = &descriptor->actions[i];

    if (action->used == 0) {
      continue;
    }

    if (action->handler(vector, action->dev_id, regs) == IRQ_HANDLED) {
      handled = 1;
    }
  }

  /*
   * We have currently no use for this, but maybe in the future.
   */
  (void)handled;

  active_irq_chip->eoi(vector);
}

