#ifndef KERNEL_MULTIBOOT_H
  #define KERNEL_MULTIBOOT_H

  #include <stdint.h>

  #define MULTIBOOT_FLAG_MEMORY            (1 << 0)
  #define MULTIBOOT_FLAG_BOOTDEV           (1 << 1)
  #define MULTIBOOT_FLAG_CMDLINE           (1 << 2)
  #define MULTIBOOT_FLAG_MODS              (1 << 3)
  #define MULTIBOOT_FLAG_ELF_SHDR          (1 << 5)
  #define MULTIBOOT_FLAG_MEM_MAP           (1 << 6)
  #define MULTIBOOT_FLAG_DRIVE_INFO        (1 << 7)
  #define MULTIBOOT_FLAG_BOOT_LOADER_NAME  (1 << 9)
  #define MULTIBOOT_FLAG_FRAMEBUFFER_INFO  (1 << 12)

  struct multiboot_info {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;

    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    union {
      struct {
        uint32_t tabsize;
        uint32_t strsize;
        uint32_t addr;
        uint32_t reserved;
      } aout;

      struct {
        uint32_t num;
        uint32_t size;
        uint32_t addr;
        uint32_t shndx;
      } elf;
    } syms;

    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;
    uint32_t boot_loader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t color_info[6];
  };

  struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
  } __attribute__((packed));

#endif
