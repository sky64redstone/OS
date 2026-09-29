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
#include "kernel/multiboot.h"

#include "version.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

void kmain(uint32_t magic, struct multiboot_info* info) {
  stdout.put = vga_kput;
  stdout.print = vga_kprint;

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    kprintf("\nInvalid multiboot magic number (0x%x)\n", magic);
    while (1) {
      asm volatile("cli; hlt");
    }
  }

  vga_clear_screen();
  kprint(str_welcome);

  if (info->flags & MULTIBOOT_FLAG_BOOTDEV) {
    uint8_t drive = (info->boot_device >> 24) & 0xff;
    uint8_t part1 = (info->boot_device >> 16) & 0xff;
    uint8_t part2 = (info->boot_device >> 8) & 0xff;
    uint8_t part3 = info->boot_device & 0xff;

    kprintf(
      "Boot drive: 0x%x\n"
      "Partitions: %u/%u/%u\n",
      drive, part1, part2, part3
    );
  }

  if (info->flags & MULTIBOOT_FLAG_MEM_MAP) {
    uint32_t current = info->mmap_addr;
    uint32_t end = info->mmap_addr + info->mmap_length;

    while (current < end) {
      struct multiboot_mmap_entry* entry = (struct multiboot_mmap_entry*)current;

      kprintf(
        "memory: base=%x%08x length=%x%08x type=%u\n",
        (uint32_t)(entry->addr >> 32),
        (uint32_t)entry->addr,
        (uint32_t)(entry->len >> 32),
        (uint32_t)entry->len,
        entry->type
      );

      current += entry->size + sizeof(entry->size);
    }
  }

  if (info->flags & MULTIBOOT_FLAG_BOOT_LOADER_NAME) {
    const char* name = (const char*)info->boot_loader_name;
    kprintf("Boot loader: %s\n", name);
  }

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
