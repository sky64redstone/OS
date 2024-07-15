// llio.h
//
// low level input output header file
//
// prefix: llio (e.g. llio_function_name)
//

#ifndef LLIO_H
    #define LLIO_H

    // C wrapper function that reads a byte from the specified port
    unsigned char llio_port_byte_in(unsigned short port) {
        // "=a" (result) means: put al reg in var result when finished
        // "d" (port) means:    load edx with port
        unsigned char result;
        __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
        return result;
    }

    // C wrapper function that writes a byte to the specified port
    void llio_port_byte_out(unsigned short port, unsigned char data) {
        // "a" (data) means: load eax with data
        // "d" (port) means: load edx with port
        __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
    }

    // C wrapper function that reads a word from the specified port
    unsigned short llio_word_in(unsigned short port) {
        unsigned short result;
        __asm__("in %%dx, %%ax" : "=a" (result) : "d" (port));
        return result;
    }

    // C wrapper function that writes a word to the specified port
    void llio_port_word_out(unsigned short port, unsigned short data) {
        __asm__("out %%ax, %%dx" : :"a" (data), "d" (port));
    }

#endif
