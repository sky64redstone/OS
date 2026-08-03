#ifndef KERNEL_INITCALL_H
  #define KERNEL_INITCALL_H

  #include "device.h"

  #define BUILTIN_DRIVER(driver) \
    struct device_driver* const \
    __builtin_driver_##driver \
    __attribute__((used, section(".builtin_drivers"))) = &(driver)

  void builtin_drivers_init();

#endif
