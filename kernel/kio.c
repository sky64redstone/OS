#include "kio.h"

#include <stdint.h>
#include <stdarg.h>

#define MAX_BUFFER 64

outstream_t stdout;

void kput(char c) {
  stdout.put(c);
}

void kprint(const char* str) {
  stdout.print(str);
}

void kprint_num(unsigned int number, unsigned int base) {
  static const char digits[] = "0123456789ABCDEF";
  char buffer[33];
  unsigned int i = sizeof(buffer) - 1;

  if (base < 2 || base > 16)
    return;

  buffer[i] = '\0';

  do {
    buffer[--i] = digits[number % base];
    number /= base;
  } while (number != 0);

  kprint(&buffer[i]);
}

void kvprintf(const char* fmt, va_list list) {
  while (*fmt != 0) {
    if (*fmt == '%') {
      fmt++;
      if (*fmt == 0) {
        return;
      }

      switch (*fmt) {
        case '%': {
          kput('%');
          break;
        }
        case 'c': {
          kput((char)va_arg(list, int));
          break;
        }
        case 's': {
          const char* str = va_arg(list, char*);
          kprint(str ? str : "(null)");
          break;
        }
        case 'd':
        case 'i': {
          int i = va_arg(list, int);
          if (i < 0) {
            kput('-');
            i = -i;
          }
          kprint_num(i, 10);
          break;
        }
        case 'u': {
          unsigned int i = va_arg(list, unsigned int);
          kprint_num(i, 10);
          break;
        }
        case 'X':
        case 'x': {
          unsigned int i = va_arg(list, unsigned int);
          kprint_num(i, 16);
          break;
        }
        case 'o': {
          unsigned int i = va_arg(list, unsigned int);
          kprint_num(i, 8);
          break;
        }
        case 'b':
        case 'B': {
          unsigned int i = va_arg(list, unsigned int);
          kprint_num(i, 2);
          break;
        }
        case 'p':
        case 'P': {
          uint32_t i = (uint32_t)va_arg(list, void*);
          kprint_num(i, 16);
          break;
        }
      }
    } // if (*fmt == '%')
    else {
      kput(*fmt);
    }
    fmt++;
  } // while (*fmt != 0)
}

__attribute__ ((format (printf, 1, 2)))
void kprintf(const char* fmt, ...) {
  va_list list;
  va_start(list, fmt);
  kvprintf(fmt, list);
  va_end(list);
}
