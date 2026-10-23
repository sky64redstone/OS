[bits 32]

section .text

global paging_enable
global paging_invalidate_page

; void paging_enable(uint32_t physical_directory)
paging_enable:
  ; arg1: physical address of the page directory.
  mov eax, [esp + 4]
  mov cr3, eax

  ; Enable paging
  mov eax, cr0
  or eax, 0x80000000

  ; Enable supervisor write protection (486+)
  or eax, 0x00010000

  mov cr0, eax
  ; ret interprets the physical address as virtual
  ret

paging_invalidate_page:
  mov eax, [esp + 4]
  invlpg [eax]
  ret
