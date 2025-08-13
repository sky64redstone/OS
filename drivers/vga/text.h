#ifndef VGA_TEXT
  #define VGA_TEXT

  void kput(char c, int x, int y);
  void kprint(const char* str, int x, int y);
  void clear_screen();

#endif
