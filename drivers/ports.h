#ifndef PORTS_H
  #define PORTS_H

  // reads a 8-bit number from the specified port and returns it
  unsigned char port_read8(unsigned short port);
  // writes a 8-bit number to the specified port
  void port_write8(unsigned short port, unsigned char data);

  // reads a 16-bit number from the specified port and returns it
  unsigned short port_read16(unsigned short port);
  // writes a 16-bit number to the specified port
  void port_write16(unsigned short port, unsigned short data);
#endif
