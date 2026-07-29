#ifndef VGA_TEXT
  #define VGA_TEXT

  void vga_put(char c, int x, int y);
  void vga_print(const char* str, int x, int y);
  void vga_clear_screen();

#endif
