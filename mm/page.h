#ifndef MM_PAGE_H
  #define MM_PAGE_H

  #include <stdint.h>
  #include <stddef.h>

  typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
  } memory_region_t;

  /* memory_region_t.type */
  #define MEMORY_REGION_USABLE 1

  #define FRAME_ALLOC_FAILED 0xFFFFFFFF

  #define PAGE_PRESENT (1 << 0)
  #define PAGE_WRITE   (1 << 1)
  #define PAGE_USER    (1 << 2)
  #define PAGE_PWT     (1 << 3)
  #define PAGE_PCD     (1 << 4)

  void paging_enable_bootstrap();
  /* returns 1 on success and 0 on failure. */
  int paging_map_page(uint32_t virtual, uint32_t physical, uint32_t flags);
  int paging_translate(uint32_t virtual_address, uint32_t* physical_address);

  void frame_allocator_init(
    const memory_region_t* regions, size_t region_count,
    uint64_t kernel_start, uint64_t kernel_end
  );
  void frame_reserve_range(uint64_t base, uint64_t length);
  uint32_t frame_alloc();
  /* returns 1 on success and 0 on failure. */
  int frame_free(uint32_t physical_address);
  void frame_print_regions();

#endif
