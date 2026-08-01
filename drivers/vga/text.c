#include "text.h"
#include "drivers/ports.h"

#define SCREEN_MEM ((volatile char*)0xB8000)
#define SCREEN_W 80
#define SCREEN_H 25
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5
#define DEFAULT_COLOR 0x0F

int _get_screen_offset(int x, int y) {
  return y * SCREEN_W + x;
}

int _get_screen_x(int offset) {
  return offset % SCREEN_W;
}

int _get_screen_y(int offset) {
  return offset / SCREEN_W;
}

void _set_cursor_offset(int offset) {
  // high byte
  port_write8(REG_SCREEN_CTRL, 14);
  port_write8(REG_SCREEN_DATA, offset >> 8);
  // low byte
  port_write8(REG_SCREEN_CTRL, 15);
  port_write8(REG_SCREEN_DATA, offset & 0xFF);
}

int _get_cursor_offset() {
  // high byte
  port_write8(REG_SCREEN_CTRL, 14);
  int offset = port_read8(REG_SCREEN_DATA) << 8;
  // low byte
  port_write8(REG_SCREEN_CTRL, 15);
  offset |= port_read8(REG_SCREEN_DATA);
  return offset;
}

int _handle_scrolling(int offset) {
  int y = _get_screen_y(offset);

  if (y >= SCREEN_H) {
    const int diff = SCREEN_H - y + 1;
    const int count = SCREEN_W * diff * 2;
    const int mem_max = SCREEN_W * SCREEN_H * 2;
    const int max = mem_max - count;

    for (int i = 0; i < max; i++) {
      SCREEN_MEM[i] = SCREEN_MEM[i + count];
    }

    for (int i = max; i < mem_max; i += 2) {
      SCREEN_MEM[i] = ' ';
      SCREEN_MEM[i + 1] = DEFAULT_COLOR;
    }
  
    offset = _get_screen_offset(0, SCREEN_H - 1);

    return offset;
  }

  return offset;
}

int _vga_put(char c, int offset) {
  if (offset < 0) {
    offset = _get_cursor_offset();
  }

  if (c == '\n') {
    offset = _get_screen_offset(0, _get_screen_y(offset) + 1);
  } else {
    int off = offset * 2;
    SCREEN_MEM[off] = c;
    SCREEN_MEM[off + 1] = DEFAULT_COLOR;
    offset++;
  }

  offset = _handle_scrolling(offset);

  return offset;
}

void vga_put(char c, int x, int y) {
  int offset;
  if (x < 0 || y < 0) {
    offset = -1;
  } else {
    offset = _get_screen_offset(x, y);
  }

  offset = _vga_put(c, offset);
  _set_cursor_offset(offset);
}

void _vga_print(const char* str, int offset) {
  while (*str != 0) {
    offset = _vga_put(*str, offset);
    str++;
  }
  _set_cursor_offset(offset);
}

void vga_print(const char* str, int x, int y) {
  int offset;
  if (x < 0 || y < 0) {
    offset = -1;
  } else {
    offset = _get_screen_offset(x, y);
  }

  _vga_print(str, offset);
}

void vga_clear_screen() {
  const int max_cells = SCREEN_W * SCREEN_H * 2;
  for (int offset = 0; offset < max_cells; offset += 2) {
    SCREEN_MEM[offset] = ' ';
    SCREEN_MEM[offset + 1] = DEFAULT_COLOR;
  }
  _set_cursor_offset(0);
}

void vga_kput(char c) {
  vga_put(c, -1, -1);
}

void vga_kprint(const char* str) {
  vga_print(str, -1, -1);
}

