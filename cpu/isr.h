#ifndef ISR_H
  #define ISR_H

  #include <stdint.h>

  void isr_install();

  typedef struct {
    uint32_t ds;
    // pushed by pusha
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    // pushed by isr procedure
    uint32_t int_no, err_no;
    // pushed by the cpu
    uint32_t eip, cs, eflags, useresp, ss;
  } isr_regs_t;

  void isr_handler(isr_regs_t* regs);

  #define ISR_FUNC(x) extern void isr##x()

  ISR_FUNC( 0);
  ISR_FUNC( 1);
  ISR_FUNC( 2);
  ISR_FUNC( 3);
  ISR_FUNC( 4);
  ISR_FUNC( 5);
  ISR_FUNC( 6);
  ISR_FUNC( 7);
  ISR_FUNC( 8);
  ISR_FUNC( 9);
  ISR_FUNC(10);
  ISR_FUNC(11);
  ISR_FUNC(12);
  ISR_FUNC(13);
  ISR_FUNC(14);
  ISR_FUNC(15);
  ISR_FUNC(16);
  ISR_FUNC(17);
  ISR_FUNC(18);
  ISR_FUNC(19);
  ISR_FUNC(20);
  ISR_FUNC(21);
  ISR_FUNC(22);
  ISR_FUNC(23);
  ISR_FUNC(24);
  ISR_FUNC(25);
  ISR_FUNC(26);
  ISR_FUNC(27);
  ISR_FUNC(28);
  ISR_FUNC(29);
  ISR_FUNC(30);
  ISR_FUNC(31);

  #define IRQ_FUNC(x) extern void irq##x()

  IRQ_FUNC( 0);
  IRQ_FUNC( 1);
  IRQ_FUNC( 2);
  IRQ_FUNC( 3);
  IRQ_FUNC( 4);
  IRQ_FUNC( 5);
  IRQ_FUNC( 6);
  IRQ_FUNC( 7);
  IRQ_FUNC( 8);
  IRQ_FUNC( 9);
  IRQ_FUNC(10);
  IRQ_FUNC(11);
  IRQ_FUNC(12);
  IRQ_FUNC(13);
  IRQ_FUNC(14);
  IRQ_FUNC(15);
#endif
