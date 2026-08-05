#include "cpu/isr.h"
#include "cpu/pic.h"

#include "drivers/ps2/ps2.h"
#include "drivers/vga/text.h"

#include "kernel/device.h"
#include "kernel/initcall.h"
#include "kernel/kio.h"

#include "version.h"

void kmain() {
  stdout.put = vga_kput;
  stdout.print = vga_kprint;
  vga_clear_screen();
  //kprint(str_welcome);
  vga_print(str_welcome,-1,-1);

  /*
   * Install exception and hardware IRQ gates, but do not globally enable
   * interrupts yet.
   */
  isr_install();

  /*
   * Establish the generic core before registering either side of a
   * device-driver association.
   */
  device_core_init();

  /*
   * Initialize the interrupt controller and begin with IRQs masked.
   */
  pic_init();

  /*
   * Register statically linked driver descriptors.
   */
  builtin_drivers_init();

  /*
   * Detect/register platform devices. Registering this device invokes
   * ps2_keyboard_probe() because the driver is already registered.
   */
  ps2_bus_init();

  asm volatile("sti");

  while (1) {
    asm volatile("hlt");
  }
}
