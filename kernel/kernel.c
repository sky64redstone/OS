#include <stdint.h>

#include "boot/multiboot.h"

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

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kmain(uint32_t magic, multiboot_info_t* info) {
  stdout.put = vga_kput;
  stdout.print = vga_kprint;

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
    kprintf("\nInvalid multiboot magic number (0x%x)\n", magic);
    return;
  }

  vga_clear_screen();
  kprint(str_welcome);

  /* Print out the flags. */
  kprintf ("flags = 0x%x\n", (unsigned) info->flags);

  /* Are mem_* valid? */
  if (CHECK_FLAG (info->flags, 0))
    kprintf ("mem_lower = %uKB, mem_upper = %uKB\n",
            (unsigned) info->mem_lower, (unsigned) info->mem_upper);

  /* Is boot_device valid? */
  if (CHECK_FLAG (info->flags, 1))
    kprintf ("boot_device = 0x%x\n", (unsigned) info->boot_device);

  /* Are mods_* valid? */
  if (CHECK_FLAG (info->flags, 3))
  {
    multiboot_module_t *mod;
    unsigned int i;

    kprintf ("mods_count = %d, mods_addr = 0x%x\n",
            (int) info->mods_count, (int) info->mods_addr);
    for (i = 0, mod = (multiboot_module_t *) info->mods_addr;
         i < info->mods_count;
    i++, mod++)
         kprintf (" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                 (unsigned) mod->mod_start,
                 (unsigned) mod->mod_end,
                 (char *) mod->cmdline);
  }

  /* Bits 4 and 5 are mutually exclusive! */
  if (CHECK_FLAG (info->flags, 4) && CHECK_FLAG (info->flags, 5))
  {
    kprintf ("Both bits 4 and 5 are set.\n");
    return;
  }

  /* Is the symbol table of a.out valid? */
  if (CHECK_FLAG (info->flags, 4))
  {
    multiboot_aout_symbol_table_t *multiboot_aout_sym = &(info->u.aout_sym);

    kprintf ("multiboot_aout_symbol_table: tabsize = 0x%0x, "
    "strsize = 0x%x, addr = 0x%x\n",
    (unsigned) multiboot_aout_sym->tabsize,
            (unsigned) multiboot_aout_sym->strsize,
            (unsigned) multiboot_aout_sym->addr);
  }

  /* Is the section header table of ELF valid? */
  if (CHECK_FLAG (info->flags, 5))
  {
    multiboot_elf_section_header_table_t *multiboot_elf_sec = &(info->u.elf_sec);

    kprintf ("multiboot_elf_sec: num = %u, size = 0x%x,"
    " addr = 0x%x, shndx = 0x%x\n",
    (unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
            (unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
  }

  /* Are mmap_* valid? */
  if (CHECK_FLAG (info->flags, 6))
  {
    multiboot_memory_map_t *mmap;

    kprintf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
            (unsigned) info->mmap_addr, (unsigned) info->mmap_length);
    for (mmap = (multiboot_memory_map_t *) info->mmap_addr;
         (unsigned long) mmap < info->mmap_addr + info->mmap_length;
    mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
    + mmap->size + sizeof (mmap->size)))
         kprintf (" size = 0x%x, base_addr = 0x%x%08x,"
         " length = 0x%x%08x, type = 0x%x\n",
         (unsigned) mmap->size,
                 (unsigned) (mmap->addr >> 32),
                 (unsigned) (mmap->addr & 0xffffffff),
                 (unsigned) (mmap->len >> 32),
                 (unsigned) (mmap->len & 0xffffffff),
                 (unsigned) mmap->type);
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
