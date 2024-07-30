#include "../drivers/screen.h"

void kmain() {
    clear_screen();
    
    int offset = _kprint_char('A', 0, 0, WHITE_ON_BLACK);
    _kprint_char((char)_get_offset_x(offset), 1, 0, WHITE_ON_BLACK);
    _kprint_char('C', 0, 1, WHITE_ON_BLACK);

    __asm__("cli");
    __asm__("hlt");
}
