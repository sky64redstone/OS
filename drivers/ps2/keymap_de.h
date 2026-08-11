/*
 * This file contains the key mapping for PS/2 Keyboards with the german layout
 *
 * It supplies:
 * struct key_mapping keymap_de[]
 * uint32_t keymap_ext_de[]
 */

struct key_mapping;

/* german keymap */
const struct key_mapping keymap_de[128] = {
  [0x01] = { KEY_ESCAPE, KEY_ESCAPE, KEY_ESCAPE },

  [0x02] = { '1', '!', 0x00B9 },
  [0x03] = { '2', '"', 0x00B2 },
  [0x04] = { '3', 0x00A7, 0x00B3 },
  [0x05] = { '4', '$', KEY_NONE },
  [0x06] = { '5', '%', KEY_NONE },
  [0x07] = { '6', '&', KEY_NONE },
  [0x08] = { '7', '/', '{' },
  [0x09] = { '8', '(', '[' },
  [0x0A] = { '9', ')', ']' },
  [0x0B] = { '0', '=', '}' },
  [0x0C] = { 0x00DF, '?', '\\' },
  [0x0D] = { 0x00B4, '`', KEY_NONE },
  [0x0E] = { KEY_BACKSPACE, KEY_BACKSPACE, KEY_BACKSPACE },
  [0x0F] = { KEY_TAB, KEY_TAB, KEY_TAB },

  [0x10] = { 'q', 'Q', '@' },
  [0x11] = { 'w', 'W', KEY_NONE },
  [0x12] = { 'e', 'E', 0x20AC },
  [0x13] = { 'r', 'R', KEY_NONE },
  [0x14] = { 't', 'T', KEY_NONE },
  [0x15] = { 'z', 'Z', KEY_NONE },
  [0x16] = { 'u', 'U', KEY_NONE },
  [0x17] = { 'i', 'I', KEY_NONE },
  [0x18] = { 'o', 'O', KEY_NONE },
  [0x19] = { 'p', 'P', KEY_NONE },
  [0x1A] = { 0x00FC, 0x00DC, KEY_NONE },
  [0x1B] = { '+', '*', '~' },
  [0x1C] = { KEY_ENTER, KEY_ENTER, KEY_ENTER },
  [0x1D] = { KEY_LEFT_CTRL, KEY_LEFT_CTRL, KEY_LEFT_CTRL },

  [0x1E] = { 'a', 'A', KEY_NONE },
  [0x1F] = { 's', 'S', KEY_NONE },
  [0x20] = { 'd', 'D', KEY_NONE },
  [0x21] = { 'f', 'F', KEY_NONE },
  [0x22] = { 'g', 'G', KEY_NONE },
  [0x23] = { 'h', 'H', KEY_NONE },
  [0x24] = { 'j', 'J', KEY_NONE },
  [0x25] = { 'k', 'K', KEY_NONE },
  [0x26] = { 'l', 'L', KEY_NONE },
  [0x27] = { 0x00F6, 0x00D6, KEY_NONE },
  [0x28] = { 0x00E4, 0x00C4, KEY_NONE },
  [0x29] = { '^', 0x00B0, KEY_NONE },
  [0x2A] = { KEY_LEFT_SHIFT, KEY_LEFT_SHIFT, KEY_LEFT_SHIFT },
  [0x2B] = { '#', '\'', KEY_NONE },

  [0x2C] = { 'y', 'Y', KEY_NONE },
  [0x2D] = { 'x', 'X', KEY_NONE },
  [0x2E] = { 'c', 'C', KEY_NONE },
  [0x2F] = { 'v', 'V', KEY_NONE },
  [0x30] = { 'b', 'B', KEY_NONE },
  [0x31] = { 'n', 'N', KEY_NONE },
  [0x32] = { 'm', 'M', 0x00B5 },
  [0x33] = { ',', ';', KEY_NONE },
  [0x34] = { '.', ':', KEY_NONE },
  [0x35] = { '-', '_', KEY_NONE },
  [0x36] = { KEY_RIGHT_SHIFT, KEY_RIGHT_SHIFT, KEY_RIGHT_SHIFT },
  [0x37] = { '*', '*', '*' },
  [0x38] = { KEY_LEFT_ALT, KEY_LEFT_ALT, KEY_LEFT_ALT },
  [0x39] = { ' ', ' ', ' ' },
  [0x3A] = { KEY_CAPS_LOCK, KEY_CAPS_LOCK, KEY_CAPS_LOCK },

  [0x3B] = { KEY_F1, KEY_F1, KEY_F1 },
  [0x3C] = { KEY_F2, KEY_F2, KEY_F2 },
  [0x3D] = { KEY_F3, KEY_F3, KEY_F3 },
  [0x3E] = { KEY_F4, KEY_F4, KEY_F4 },
  [0x3F] = { KEY_F5, KEY_F5, KEY_F5 },
  [0x40] = { KEY_F6, KEY_F6, KEY_F6 },
  [0x41] = { KEY_F7, KEY_F7, KEY_F7 },
  [0x42] = { KEY_F8, KEY_F8, KEY_F8 },
  [0x43] = { KEY_F9, KEY_F9, KEY_F9 },
  [0x44] = { KEY_F10, KEY_F10, KEY_F10 },

  [0x45] = { KEY_NUM_LOCK, KEY_NUM_LOCK, KEY_NUM_LOCK },
  [0x46] = { KEY_SCROLL_LOCK, KEY_SCROLL_LOCK, KEY_SCROLL_LOCK },

  [0x47] = { '7', '7', '7' },
  [0x48] = { '8', '8', '8' },
  [0x49] = { '9', '9', '9' },
  [0x4A] = { '-', '-', '-' },
  [0x4B] = { '4', '4', '4' },
  [0x4C] = { '5', '5', '5' },
  [0x4D] = { '6', '6', '6' },
  [0x4E] = { '+', '+', '+' },
  [0x4F] = { '1', '1', '1' },
  [0x50] = { '2', '2', '2' },
  [0x51] = { '3', '3', '3' },
  [0x52] = { '0', '0', '0' },
  [0x53] = { ',', ',', ',' },

  [0x56] = { '<', '>', '|' },

  [0x57] = { KEY_F11, KEY_F11, KEY_F11 },
  [0x58] = { KEY_F12, KEY_F12, KEY_F12 }
};

/*
 * extended german keymap
 * maps bytes that come after 0xE0
 */
const uint32_t keymap_ext_de[128] = {
  [0x1C] = KEY_KEYPAD_ENTER,
  [0x1D] = KEY_RIGHT_CTRL,
  [0x35] = KEY_KEYPAD_SLASH,
  [0x37] = KEY_PRINT_SCREEN,
  [0x38] = KEY_RIGHT_ALT, /* AltGr */

  [0x47] = KEY_HOME,
  [0x48] = KEY_UP,
  [0x49] = KEY_PAGE_UP,
  [0x4B] = KEY_LEFT,
  [0x4D] = KEY_RIGHT,
  [0x4F] = KEY_END,
  [0x50] = KEY_DOWN,
  [0x51] = KEY_PAGE_DOWN,
  [0x52] = KEY_INSERT,
  [0x53] = KEY_DELETE,

  [0x5B] = KEY_LEFT_GUI,
  [0x5C] = KEY_RIGHT_GUI,
  [0x5D] = KEY_MENU
};
