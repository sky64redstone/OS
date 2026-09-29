#include <stdint.h>

#include "cpu/isr.h"
#include "cpu/pic.h"

#include "drivers/ps2/ps2.h"
#include "drivers/ps2/keyboard.h"
#include "drivers/vga/text.h"

#include "kernel/device.h"
#include "kernel/initcall.h"
#include "kernel/kio.h"
#include "kernel/shell.h"

#include "version.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

void kmain(uint32_t multiboot_magic, uint32_t multiboot_info) {
  (void)multiboot_info;

  stdout.put = vga_kput;
  stdout.print = vga_kprint;

  if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    kprintf("\nInvalid multiboot magic number (0x%x)\n", multiboot_magic);
    while (1) {
      asm volatile("cli; hlt");
    }
  }

  vga_clear_screen();
  kprint(str_welcome);

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

  /*
   * Initialize the kernel shell and print the first prompt.
   */
  shell_init();

  asm volatile("sti");

  while (1) {
    char character;

    /*
     * Prevent an interrupt from arriving between the empty-queue
     * check and hlt.
     */
    asm volatile("cli" : : : "memory");

    if (ps2_keyboard_read(&character)) {
      asm volatile("sti" : : : "memory");

      shell_input(character);
      continue;
    }

    /*
     * STI followed immediately by HLT closes the sleep race:
     * an IRQ arriving before HLT will wake the CPU.
     */
    asm volatile("sti; hlt" : : : "memory");
  }
}
