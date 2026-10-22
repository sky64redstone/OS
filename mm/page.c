#include <stdint.h>
#include <stddef.h>

#include "page.h"

#include "kernel/kio.h"

/*
 * Page:  block of virtual memory
 * Frame: block of physical memory
 *  - Wikipedia
 * PDE: Page Directory Entry
 * PTE: Page Table Entry
 *  - wiki.osdev.org
 */

/*
 * The intended startup sequence is:
 *  1. Obtain and parse the bootloader's physical memory map.
 *  2. Initialize the frame allocator and 
 *     reserve the kernel and bootloader-owned ranges.
 *  3. Build the bootstrap page tables.
 *  4. Enable paging.
 *  5. Begin allocating and installing dynamic mappings.
 */

#define PAGE_SIZE 4096
#define PAGE_MASK ((uint32_t)0xFFFFF000)
#define PAGE_ENTRIES 1024

#define PHYSICAL_LIMIT 0x100000000ULL
#define MAX_FRAMES (1 << 20)
#define BITMAP_WORDS (MAX_FRAMES / 32)

#define DIRECT_MAP_SIZE (64 * 1024 * 1024)
#define DIRECT_MAP_FRAMES (DIRECT_MAP_SIZE / PAGE_SIZE)

__attribute__((aligned(PAGE_SIZE)))
  static uint32_t boot_page_directory[PAGE_ENTRIES];
__attribute__((aligned(PAGE_SIZE)))
  static uint32_t boot_page_tables[16][PAGE_ENTRIES];

static uint32_t* current_page_directory;

static uint32_t frame_bitmap[BITMAP_WORDS];
static uint32_t allocated_bitmap[BITMAP_WORDS];

static void paging_build_bootstrap_tables();
static void frame_mark_usable(uint64_t base, uint64_t length);

/*
 * Defined in cpu/paging.asm.
 */
extern void paging_enable(uint32_t physical_directory);
extern void paging_invalidate_page(uint32_t virtual_address);

/*
 * Identity map the first 64 KiB at 0x100000 (where we load our kernel).
 * Assumes the kernel is still running with identity-like addressing.
 */
static void paging_build_bootstrap_tables() {
  for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
    boot_page_directory[i] = 0;
  }

  for (uint32_t pdi = 0; pdi < 16; pdi++) {
    for (uint32_t pti = 0; pti < PAGE_ENTRIES; pti++) {
      uint32_t physical_address = ((pdi * PAGE_ENTRIES) + pti) * PAGE_SIZE;

      boot_page_tables[pdi][pti] = physical_address | PAGE_PRESENT | PAGE_WRITE;
    }

    uint32_t table_physical = (uint32_t)(uintptr_t)&boot_page_tables[pdi][0];

    boot_page_directory[pdi] = (table_physical & PAGE_MASK) | PAGE_PRESENT | PAGE_WRITE;
  }

  /* Leave virtual address zero unmapped to catch null dereferences. */
  boot_page_tables[0][0] = 0;

  current_page_directory = boot_page_directory;
}

void paging_enable_bootstrap() {
  paging_build_bootstrap_tables();

  uint32_t directory_physical = (uint32_t)boot_page_directory;

  paging_enable(directory_physical);
}

int paging_map_page(uint32_t virtual, uint32_t physical, uint32_t flags) {
  if ((virtual & (PAGE_SIZE - 1)) != 0) {
    return 0;
  }

  if ((physical & (PAGE_SIZE - 1)) != 0) {
    return 0;
  }

  uint32_t directory_index = virtual >> 22;
  uint32_t table_index = (virtual >> 12) & 0x3FF;

  uint32_t pde = current_page_directory[directory_index];
  uint32_t* page_table;

  if ((pde & PAGE_PRESENT) == 0) {
    uint32_t table_physical = frame_alloc();

    if (table_physical == FRAME_ALLOC_FAILED) {
      return 0;
    }

    /*
     * Allocated page tables are below 64 MiB and are therefore
     * accessible through the initial identity mapping.
     */
    page_table = (uint32_t*)(uintptr_t)table_physical;

    for (uint32_t i = 0; i < PAGE_ENTRIES; i++) {
      page_table[i] = 0;
    }

    current_page_directory[directory_index] =
      table_physical | PAGE_PRESENT | PAGE_WRITE;
  } else {
    uint32_t table_physical = pde & PAGE_MASK;

    page_table = (uint32_t*)(uintptr_t)table_physical;
  }

  if ((page_table[table_index] & PAGE_PRESENT) != 0) {
    return 0;
  }

  /*
   * PDE permissions must permit at least the access requested
   * by this mapping. PTE permissions still restrict each page.
   */
  current_page_directory[directory_index] |= PAGE_WRITE;

  if ((flags & PAGE_USER) != 0) {
    current_page_directory[directory_index] |= PAGE_USER;
  }

  page_table[table_index] =
    (physical & PAGE_MASK) |
    PAGE_PRESENT |
    (flags & 0xFFFu);

  paging_invalidate_page(virtual);

  return 1;
}

int paging_translate(uint32_t virtual_address, uint32_t* physical_address) {
  if (physical_address == NULL) {
    return 0;
  }

  uint32_t directory_index = virtual_address >> 22;
  uint32_t table_index = (virtual_address >> 12) & 0x3FF;
  uint32_t offset = virtual_address & 0xFFF;

  uint32_t pde = current_page_directory[directory_index];

  if ((pde & PAGE_PRESENT) == 0) {
    return 0;
  }

  uint32_t table_physical = pde & PAGE_MASK;
  uint32_t* page_table = (uint32_t*)(uintptr_t)table_physical;

  uint32_t pte = page_table[table_index];

  if ((pte & PAGE_PRESENT) == 0) {
    return 0;
  }

  *physical_address = (pte & PAGE_MASK) | offset;

  return 1;
}

static void frame_mark_usable(uint64_t base, uint64_t length) {
  if (length == 0 || base >= PHYSICAL_LIMIT) {
    return;
  }

  uint64_t end = base + length;

  if (end < base || end > PHYSICAL_LIMIT) {
    end = PHYSICAL_LIMIT;
  }

  /* Only release complete frames within this usable region. */
  uint64_t first = (base + PAGE_SIZE - 1) / PAGE_SIZE;
  uint64_t last = end / PAGE_SIZE;

  if (first > MAX_FRAMES) {
    first = MAX_FRAMES;
  }

  if (last > MAX_FRAMES) {
    last = MAX_FRAMES;
  }

  for (uint64_t f = first; f < last; f++) {
    uint32_t frame = (uint32_t)f;
    uint32_t mask = 1u << (frame & 31);

    frame_bitmap[frame >> 5] &= ~mask;
    allocated_bitmap[frame >> 5] &= ~mask;
  }
}

void frame_allocator_init(
  const memory_region_t* regions, size_t region_count,
  uint64_t kernel_start, uint64_t kernel_end
) {
  for (uint32_t i = 0; i < BITMAP_WORDS; i++) {
    frame_bitmap[i] = 0xFFFFFFFFu;
    allocated_bitmap[i] = 0;
  }

  for (size_t i = 0; i < region_count; i++) {
    if (regions[i].type == MEMORY_REGION_USABLE) {
      frame_mark_usable(regions[i].base, regions[i].length);
    }
  }

  /* Reserve conventional low memory. */
  frame_reserve_range(0, 0x100000);

  /* Reserve the entire physical kernel image, including BSS. */
  if (kernel_end > kernel_start) {
    frame_reserve_range(kernel_start, kernel_end - kernel_start);
  }
}

void frame_reserve_range(uint64_t base, uint64_t length) {
  if (length == 0 || base >= PHYSICAL_LIMIT) {
    return;
  }

  uint64_t end = base + length;

  if (end < base || end > PHYSICAL_LIMIT) {
    end = PHYSICAL_LIMIT;
  }

  uint64_t first = base / PAGE_SIZE;
  uint64_t last = (end + PAGE_SIZE - 1) / PAGE_SIZE;

  if (last > MAX_FRAMES) {
    last = MAX_FRAMES;
  }

  for (uint64_t f = first; f < last; f++) {
    uint32_t frame = (uint32_t)f;
    uint32_t mask = 1u << (frame & 31);

    frame_bitmap[frame >> 5] |= mask;
    allocated_bitmap[frame >> 5] &= ~mask;
  }
}

uint32_t frame_alloc() {
  for (uint32_t frame = 0; frame < DIRECT_MAP_FRAMES; frame++) {
    uint32_t word = frame >> 5;
    uint32_t mask = 1u << (frame & 31);

    if ((frame_bitmap[word] & mask) == 0) {
      frame_bitmap[word] |= mask;
      allocated_bitmap[word] |= mask;

      return frame * PAGE_SIZE;
    }
  }

  return FRAME_ALLOC_FAILED;
}

int frame_free(uint32_t physical_address) {
  if ((physical_address & (PAGE_SIZE - 1)) != 0) {
    return 0;
  }

  uint32_t frame = physical_address / PAGE_SIZE;

  if (frame >= DIRECT_MAP_FRAMES) {
    return 0;
  }

  uint32_t word = frame >> 5;
  uint32_t mask = 1u << (frame & 31);

  if ((allocated_bitmap[word] & mask) == 0) {
    return 0;
  }

  allocated_bitmap[word] &= ~mask;
  frame_bitmap[word] &= ~mask;

  return 1;
}

