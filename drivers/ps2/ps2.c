#include "kernel/device.h"

struct resource ps2_keyboard_resources[] = {
  {
    .start = 0x60,
    .end = 0x60,
    .flags = RESOURCE_IO
  },
  {
    .start = 0x64,
    .end = 0x64,
    .flags = RESOURCE_IO
  },
  {
    .start = 1,
    .end = 1,
    .flags = RESOURCE_IRQ
  }
};

struct device ps2_keyboard_device = {
  .name = "ps2-keyboard.0",
  .compatible = "pc,ps2-keyboard",
  .resources = ps2_keyboard_resources,
  .resource_count =
    sizeof(ps2_keyboard_resources) /
    sizeof(ps2_keyboard_resources[0]),
  .driver = 0,
  .driver_data = 0,
  .registered = 0
};

int ps2_bus_init() {
  /*
   * Controller detection and self-test should eventually happen here.
   * We register the keyboard device only after detecting the controller.
   */
  return device_register(&ps2_keyboard_device);
}
