#include <stdint.h>

#include "drivers/ports.h"
#include "kernel/device.h"
#include "kernel/initcall.h"
#include "kernel/irq.h"
#include "kernel/kio.h"
#include "keyboard.h"

#define KEYBOARD_BUFFER_SIZE 128

static volatile char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint8_t keyboard_buffer_head;
static volatile uint8_t keyboard_buffer_tail;

static void ps2_keyboard_queue_char(char character) {
  uint8_t next = (uint8_t)(
    (keyboard_buffer_head + 1) & (KEYBOARD_BUFFER_SIZE - 1)
  );

  if (next == keyboard_buffer_tail) {
    return;
  }

  keyboard_buffer[keyboard_buffer_head] = character;
  keyboard_buffer_head = next;
}

int ps2_keyboard_read(char* character) {
  uint8_t tail;

  if (character == 0) {
    return 0;
  }

  tail = keyboard_buffer_tail;

  if (tail == keyboard_buffer_head) {
    return 0;
  }

  *character = keyboard_buffer[tail];
  keyboard_buffer_tail = (uint8_t)(
    (tail + 1) & (KEYBOARD_BUFFER_SIZE - 1)
  );

  return 1;
}

/*
 * TODO: move key codes into seperate file
 * to make it the same codes for every driver.
 */
enum key_code {
  KEY_NONE = 0,
  KEY_ENTER = '\n',
  KEY_ESCAPE = 0x110000,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_LEFT_CTRL,
  KEY_LEFT_SHIFT,
  KEY_RIGHT_SHIFT,
  KEY_LEFT_ALT,
  KEY_CAPS_LOCK,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_NUM_LOCK,
  KEY_SCROLL_LOCK,
  KEY_HOME,
  KEY_UP,
  KEY_PAGE_UP,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_END,
  KEY_DOWN,
  KEY_PAGE_DOWN,
  KEY_INSERT,
  KEY_DELETE,
  KEY_F11,
  KEY_F12,

  KEY_RIGHT_CTRL,
  KEY_RIGHT_ALT,
  KEY_KEYPAD_ENTER,
  KEY_KEYPAD_SLASH,

  KEY_LEFT_GUI,
  KEY_RIGHT_GUI,
  KEY_MENU,
  KEY_PRINT_SCREEN,
  KEY_PAUSE
};

struct key_mapping {
  uint32_t normal;
  uint32_t shifted;
  uint32_t altGr;
};

struct keyboard_state {
  uint8_t left_shift;
  uint8_t right_shift;
  uint8_t caps_lock;
  uint8_t left_ctrl;
  uint8_t right_ctrl;
  uint8_t left_alt;
  uint8_t right_alt;
  uint8_t extended;
};

/*
 * TODO: move key mappings into seperate file
 * to enable multiple mappings and switch them
 * at compile- and runtime.
 */
#include "keymap_de.h"

struct ps2_keyboard {
  struct device* device;
  uint16_t data_port;
  uint16_t command_port;
  uint32_t irq;

  struct keyboard_state state;
};

static uint8_t is_alpha(uint32_t code) {
  return ('a' <= code && 'z' >= code) ||
         code == 0xE4 || code == 0xF6 || code == 0xFC; /* ae, oe, ue umlaut */
}

static uint32_t ps2_keyboard_mapped_value(struct ps2_keyboard* kb, uint8_t scancode) {
  const struct key_mapping* mapping = &keymap_de[scancode];
  const uint8_t shift = kb->state.left_shift ||
                        kb->state.right_shift;
  const uint8_t ctrl  = kb->state.left_ctrl ||
                        kb->state.right_ctrl;
  const uint8_t altgr = kb->state.right_alt ||
                        (ctrl && kb->state.left_alt);

  if (altgr && mapping->altGr != KEY_NONE) {
    return mapping->altGr;
  }

  if (is_alpha(mapping->normal)) {
    if (shift != kb->state.caps_lock) {
      return mapping->shifted;
    }
    return mapping->normal;
  }

  if (shift) {
    return mapping->shifted;
  }

  return mapping->normal;
}

static void ps2_keyboard_update_modifiers(
  struct ps2_keyboard* kb, uint32_t scancode, uint8_t pressed
) {
  switch (scancode) {
    case KEY_LEFT_SHIFT: kb->state.left_shift = pressed; break;
    case KEY_RIGHT_SHIFT: kb->state.right_shift = pressed; break;
    case KEY_LEFT_CTRL: kb->state.left_ctrl = pressed; break;
    case KEY_RIGHT_CTRL: kb->state.right_ctrl = pressed; break;
    case KEY_LEFT_ALT: kb->state.left_alt = pressed; break;
    case KEY_RIGHT_ALT: kb->state.right_alt = pressed; break;
    case KEY_CAPS_LOCK:
      if (pressed)
        kb->state.caps_lock = !kb->state.caps_lock;
      break;
    default: break;
  }
}

static void ps2_keyboard_process_scan_code(struct ps2_keyboard* kb, uint8_t scancode) {
  if (scancode == 0xE0) {
    kb->state.extended = 1;
    return;
  }
  if (scancode == 0xE1) {
    kb->state.extended = 0;
    return;
  }

  uint8_t extended = kb->state.extended;
  kb->state.extended = 0;

  uint8_t pressed = !(scancode & 0x80);
  uint8_t code    =   scancode & 0x7F;

  uint32_t key;
  if (extended) {
    key = keymap_ext_de[code];
  } else {
    key = keymap_de[code].normal;
  }

  if (key == KEY_NONE) {
    return;
  }

  ps2_keyboard_update_modifiers(kb, key, pressed);

  if (!pressed) {
    /* event key released */
    return;
  }

  if (!extended && key < 0x110000) {
    key = ps2_keyboard_mapped_value(kb, code);
  }

  /* event key pressed */
  switch (key) {
    case KEY_ENTER:
    case KEY_KEYPAD_ENTER:
      ps2_keyboard_queue_char('\n');
      break;

    case KEY_BACKSPACE:
      ps2_keyboard_queue_char('\b');
      break;

    case KEY_TAB:
      ps2_keyboard_queue_char('\t');
      break;

    case KEY_ESCAPE:
      ps2_keyboard_queue_char('\x1B');
      break;

    default:
      if (!(key & ~0xFF)) {
        ps2_keyboard_queue_char((char)key);
      }
      break;
  }
}

static enum irq_result ps2_keyboard_irq(
  uint32_t irq, void* dev_id, isr_regs_t* regs
) {
  struct ps2_keyboard* keyboard = dev_id;

  if (keyboard == 0 || irq != keyboard->irq) {
    return IRQ_NONE;
  }

  /*
   * Bit 0 of port 0x64 indicates that the controller output buffer
   * contains data.
   */
  uint8_t status = port_read8(keyboard->command_port);

  if ((status & 0x01) == 0) {
    return IRQ_NONE;
  }

  uint8_t scan_code = port_read8(keyboard->data_port);

  /*
   * Move the existing scan-code/state handling into this function.
   * It must not perform long blocking operations.
   */
  ps2_keyboard_process_scan_code(
    keyboard,
    scan_code
  );

  (void)regs;

  return IRQ_HANDLED;
}

/* TODO temporary global object, because we don't have malloc yet */
static struct ps2_keyboard keyboard_instance;

static int ps2_keyboard_probe(struct device* device) {
  struct resource* data_resource = 
    device_get_resource(device, RESOURCE_IO, 0);
  struct resource* command_resource = 
    device_get_resource(device, RESOURCE_IO, 1);
  struct resource* irq_resource =
    device_get_resource(device, RESOURCE_IRQ, 0);

  if (
    data_resource == 0 ||
    command_resource == 0 ||
    irq_resource == 0
  ) {
    return -1;
  }

  /*
   * TODO: memory resource
   */
  struct ps2_keyboard* keyboard = &keyboard_instance;

  keyboard->device = device;
  keyboard->data_port = data_resource->start;
  keyboard->command_port = command_resource->start;
  keyboard->irq = irq_resource->start;

  keyboard->state.left_shift = 0;
  keyboard->state.right_shift = 0;
  keyboard->state.caps_lock = 0;
  keyboard->state.left_ctrl = 0;
  keyboard->state.right_ctrl = 0;
  keyboard->state.left_alt = 0;
  keyboard->state.right_alt = 0;
  keyboard->state.extended = 0;

  device_set_data(device, keyboard);

  /*
   * Hardware initialization before exposing its interrupt,
   * unless initialization itself requires interrupts.
   */
  int result = irq_request(
    keyboard->irq,
    ps2_keyboard_irq,
    IRQF_NONE,
    "ps2-keyboard",
    keyboard
  );

  if (result != 0) {
    device_set_data(device, 0);
    return result;
  }

  return 0;
}

static void ps2_keyboard_remove(struct device* device) {
  struct ps2_keyboard* keyboard = device_get_data(device);

  if (keyboard == 0) {
    return;
  }

  irq_free(keyboard->irq, keyboard);
  device_set_data(device, 0);
}

static const char* const ps2_keyboard_compatible[] = {
  "pc,ps2-keyboard",
  0
};

static struct device_driver ps2_keyboard_driver = {
  .name = "ps2-keyboard",
  .compatible_table = ps2_keyboard_compatible,
  .probe = ps2_keyboard_probe,
  .remove = ps2_keyboard_remove,
  .registered = 0
};

BUILTIN_DRIVER(ps2_keyboard_driver);
