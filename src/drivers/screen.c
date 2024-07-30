#include "screen.h"
#include "ports.h"

// private functions

//int _get_cursor_offset();
//void _set_cursor_offset(int offset);
//int _kprint_char(char c, int x, int y, char attr);
//int _get_offset(int x, int y);
//int _get_offset_x(int offset);
//int _get_offset_y(int offset);

// public functions declaration

void clear_screen() {
    int screen_size = (MAX_COLS * MAX_ROWS) << 1;
    unsigned char* screen = VIDEO_ADDRESS;
    
    for (int i = 0; i < screen_size; i += 2) {
        screen[i] = ' ';
        screen[i + 1] = WHITE_ON_BLACK;
    }
    
    _set_cursor_offset(0);
}

void kprint_at(const char* msg, int x, int y) {
    int offset;
    if (x >= 0 && y >= 0) {
        offset = _get_offset(x, y);
    } else {
        offset = _get_cursor_offset();
        x = _get_offset_x(offset);
        y = _get_offset_y(offset);
    }

    int i = 0;
    while (msg[i] != 0) {
        offset = _kprint_char(msg[i++], x, y, WHITE_ON_BLACK);
        x = _get_offset_x(offset);
        y = _get_offset_y(offset);
    }
}

void kprint(const char* msg) {
    kprint_at(msg, -1, -1);
}

// private function declaration

int _kprint_char(char c, int x, int y, char attr) {
    unsigned char* vidmem = VIDEO_ADDRESS;
    if (attr == 0) {
        attr = WHITE_ON_BLACK;
    }

    // check if the char is outside of memory
    if (x >= MAX_COLS || y >= MAX_ROWS) {
        int max_offset = (MAX_COLS * MAX_ROWS) << 1;
        vidmem[max_offset - 2] = 'e';
        vidmem[max_offset - 1] = RED_ON_WHITE;
        return _get_offset(x, y);
    }

    int offset;
    if (x >= 0 && y >= 0) {
        offset = _get_offset(x, y);
    } else {
        offset = _get_cursor_offset();
    }

    if (c == '\n') {
        y = _get_offset_y(offset);
        offset = _get_offset(0, y + 1);
    } else {
        vidmem[offset++] = c;
        vidmem[offset++] = attr;
    }

    _set_cursor_offset(offset);

    return offset;
}

int _get_cursor_offset() {
    // 1. ask for the high byte of the cursor offset
    port_write8(REG_SCREEN_CTRL, 14);
    int offset = port_read8(REG_SCREEN_DATA) << 8;
    // 2. ask for the low byte of the cursor offset
    port_write8(REG_SCREEN_CTRL, 15);
    offset |= port_read8(REG_SCREEN_DATA);
    // 3. convert offset from screen to memory space
    return offset << 1;
}

void _set_cursor_offset(int offset) {
    offset >>= 1;
    port_write8(REG_SCREEN_CTRL, 14);
    port_write8(REG_SCREEN_DATA, offset >> 8);
    port_write8(REG_SCREEN_CTRL, 15);
    port_write8(REG_SCREEN_DATA, offset & 0xFF);
}

int _get_offset(int x, int y) {
    return (y * MAX_COLS + x) << 1;
}

int _get_offset_x(int offset) {
    return (offset % MAX_COLS) / 2;
}

int _get_offset_y(int offset) {
    return offset / (2 * MAX_COLS);
}
