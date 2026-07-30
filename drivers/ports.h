#ifndef PORTS_H
  #define PORTS_H

  #include <stdint.h>

  // reads a 8-bit number from the specified port and returns it
  uint8_t port_read8(uint16_t port);
  // writes a 8-bit number to the specified port
  void port_write8(uint16_t port, uint8_t data);

  // reads a 16-bit number from the specified port and returns it
  uint16_t port_read16(uint16_t port);
  // writes a 16-bit number to the specified port
  void port_write16(uint16_t port, uint16_t data);
#endif
