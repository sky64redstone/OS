#include "device.h"

#define DEVICE_COUNT_MAX   32
#define DRIVER_COUNT_MAX   32

struct device* devices[DEVICE_COUNT_MAX];
struct device_driver* drivers[DRIVER_COUNT_MAX];

uint32_t string_equal(const char* left, const char* right) {
  if (left == 0 || right == 0) {
    return 0;
  }

  while (*left != '\0' && *right != '\0') {
    if (*left != *right) {
      return 0;
    }

    left++;
    right++;
  }

  return *left == '\0' && *right == '\0';
}

uint32_t driver_matches_device(
  struct device_driver* driver, struct device* device
) {
  if (driver == 0 || device == 0 ||
      driver->compatible_table == 0 ||
      device->compatible == 0) {
    return 0;
  }

  const char* const* compatible = driver->compatible_table;

  while (*compatible != 0) {
    if (string_equal(*compatible, device->compatible)) {
      return 1;
    }

    compatible++;
  }

  return 0;
}

int bind_device(struct device* device, struct device_driver* driver) {
  if (device->driver != 0) {
    return -1;
  }

  if (!driver_matches_device(driver, device)) {
    return -2;
  }

  device->driver = driver;
  int result = driver->probe(device);

  if (result != 0) {
    device->driver = 0;
    device->driver_data = 0;
    return result;
  }

  return 0;
}

void try_bind_device(struct device* device) {
  if (device->driver != 0) {
    return;
  }

  for (uint32_t index = 0; index < DRIVER_COUNT_MAX; index++) {
    if (drivers[index] == 0) {
      continue;
    }

    if (bind_device(device, drivers[index]) == 0) {
      return;
    }
  }
}

void try_bind_driver(struct device_driver* driver) {
  for (uint32_t index = 0; index < DEVICE_COUNT_MAX; index++) {
    if (devices[index] == 0 || devices[index]->driver != 0) {
      continue;
    }

    bind_device(devices[index], driver);
  }
}

int device_core_init() {
  for (uint32_t index = 0; index < DEVICE_COUNT_MAX; index++) {
    devices[index] = 0;
  }

  for (uint32_t index = 0; index < DRIVER_COUNT_MAX; index++) {
    drivers[index] = 0;
  }

  return 0;
}

int device_register(struct device* device) {
  if (device == 0 || device->name == 0 ||
      device->compatible == 0 || device->registered != 0) {
    return -1;
  }

  for (uint32_t index = 0; index < DEVICE_COUNT_MAX; index++) {
    if (devices[index] != 0) {
      continue;
    }

    devices[index] = device;
    device->registered = 1;
    device->driver = 0;
    device->driver_data = 0;

    try_bind_device(device);
    return 0;
  }

  return -2;
}

int device_unregister(struct device* device) {
  if (device == 0 || device->registered == 0) {
    return -1;
  }

  if (device->driver != 0 && device->driver->remove != 0) {
    device->driver->remove(device);
  }

  device->driver = 0;
  device->driver_data = 0;
  device->registered = 0;

  for (uint32_t index = 0; index < DEVICE_COUNT_MAX; index++) {
    if (devices[index] != device) {
      continue;
    }

    devices[index] = 0;
    return 0;
  }

  return -2;
}

int driver_register(struct device_driver* driver) {
  if (driver == 0 || driver->name == 0 ||
      driver->compatible_table == 0 ||
      driver->probe == 0 || driver->registered != 0) {
    return -1;
  }

  for (uint32_t index = 0; index < DRIVER_COUNT_MAX; index++) {
    if (drivers[index] != 0) {
      continue;
    }

    drivers[index] = driver;
    driver->registered = 1;

    try_bind_driver(driver);
    return 0;
  }

  return -2;
}

int driver_unregister(struct device_driver* driver) {
  if (driver == 0 || driver->registered == 0) {
    return -1;
  }

  for (uint32_t index = 0; index < DEVICE_COUNT_MAX; index++) {
    if (devices[index] == 0 ||
        devices[index]->driver != driver) {
      continue;
    }

    if (driver->remove != 0) {
      driver->remove(devices[index]);
    }

    devices[index]->driver = 0;
    devices[index]->driver_data = 0;
  }

  for (uint32_t index = 0; index < DRIVER_COUNT_MAX; index++) {
    if (drivers[index] != driver) {
      continue;
    }

    drivers[index] = 0;
    driver->registered = 0;
    return 0;
  }

  return -2;
}

struct resource* device_get_resource(
  struct device* device, uint32_t type, uint32_t index
) {
  if (device == 0) {
    return 0;
  }

  uint32_t matched = 0;

  for (
    uint32_t resource_index = 0;
    resource_index < device->resource_count;
    resource_index++
  ) {
    if ((device->resources[resource_index].flags & type) == 0) {
      continue;
    }

    if (matched == index) {
      return &device->resources[resource_index];
    }

    matched++;
  }

  return 0;
}

void device_set_data(struct device* device, void* data) {
  if (device != 0) {
    device->driver_data = data;
  }
}

void* device_get_data(struct device* device) {
  if (device == 0) {
    return 0;
  }

  return device->driver_data;
}
