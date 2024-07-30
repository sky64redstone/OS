#ifndef SCREEN_H
    #define SCREEN_H

    #define VIDEO_ADDRESS (unsigned char*)0xB8000

    #define MAX_ROWS 25
    #define MAX_COLS 80

    #define WHITE_ON_BLACK (char)0x0F
    #define RED_ON_WHITE   (char)0xF4

    #define REG_SCREEN_CTRL 0x3D4
    #define REG_SCREEN_DATA 0x3D5

    void clear_screen();
    void kprint_at(const char* msg, int x, int y);
    void kprint(const char* msg);

    int _kprint_char(char c, int x, int y, char attr);
    int _get_offset_x(int offset);
    int _get_offset_y(int offset);
    int _get_cursor_offset();
    void _set_cursor_offset(int offset);
    int _get_offset(int x, int y);

#endif
