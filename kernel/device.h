#ifndef KERNEL_DEVICE_H
  #define KERNEL_DEVICE_H

  #include <stdint.h>

  #define DEVICE_NAME_LENGTH 32

  struct resource {
    uint32_t start;
    uint32_t end;
    uint32_t flags;
  };

  enum resource_flags {
    RESOURCE_IO  = (1 << 0),
    RESOURCE_MEM = (1 << 1),
    RESOURCE_IRQ = (1 << 2)
  };

  struct device_driver;

  struct device {
    const char* name;
    const char* compatible;

    struct resource* resources;
    uint32_t resource_count;

    struct device_driver* driver;
    void* driver_data;

    uint8_t registered;
  };

  struct device_driver {
    const char* name;
    const char* const* compatible_table;

    int (*probe)(struct device* device);
    void (*remove)(struct device* device);

    uint8_t registered;
  };

  int device_core_init();

  int device_register(struct device* device);
  int device_unregister(struct device* device);

  int driver_register(struct device_driver* driver);
  int driver_unregister(struct device_driver* driver);

  struct resource* device_get_resource(
    struct device* device, uint32_t type, uint32_t index
  );

  void device_set_data(struct device* device, void* data);
  void* device_get_data(struct device* device);

#endif
