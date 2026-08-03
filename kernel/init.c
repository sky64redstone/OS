#include "kernel/initcall.h"

extern struct device_driver* __builtin_drivers_start[];
extern struct device_driver* __builtin_drivers_end[];

void builtin_drivers_init() {
  struct device_driver** driver;

  driver = __builtin_drivers_start;

  while (driver < __builtin_drivers_end) {
    driver_register(*driver);
    driver++;
  }
}
