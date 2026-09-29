#ifndef DRIVERS_PS2_KEYBOARD_H
  #define DRIVERS_PS2_KEYBOARD_H

  /*
   * returns: 1 if it stored a valid char at *character otherwise 0
   */
  int ps2_keyboard_read(char* character);

#endif
