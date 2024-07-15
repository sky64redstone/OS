#include "../drivers/screen.h"

void kmain() {
    screen_clear();
    screen_print_at("test string", 0, 0);
}
