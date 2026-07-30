#ifndef PS2_KEYBOARD_H
  #define PS2_KEYBOARD_H

  #include "cpu/isr.h"

  void ps2_keyboard_interrupt(isr_regs_t* regs);
#endif
