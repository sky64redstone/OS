#include "kio.h"

#include <stdint.h>
#include <stdarg.h>

/* 32 (max digits of 32bit binary int) + 1 (null terminator) */
#define MAX_BUFFER 33

outstream_t stdout;

void kput(char c) {
  stdout.put(c);
}

void kprint(const char* str) {
  stdout.print(str);
}

static void kprint_num(unsigned int number, unsigned int base) {
  static const char digits[] = "0123456789ABCDEF";
  char buffer[MAX_BUFFER];
  unsigned int i = sizeof(buffer) - 1;

  if (base < 2 || base > 16)
    return;

  buffer[i] = '\0';

  do {
    buffer[--i] = digits[number % base];
    number /= base;
  } while (number);

  kprint(&buffer[i]);
}

static unsigned int kstrlen(const char* str) {
  unsigned int len = 0;

  while (str[len])
    len++;

  return len;
}

static unsigned int kdigits(unsigned int number, unsigned int base) {
  unsigned int digits = 1;

  while (number >= base) {
    number /= base;
    digits++;
  }

  return digits;
}

static void kprint_num_width(
  unsigned int number,
  unsigned int base,
  unsigned int width,
  char pad
) {
  unsigned int digits = kdigits(number, base);

  while (digits < width) {
    kput(pad);
    width--;
  }

  kprint_num(number, base);
}

static void kprint_signed_width(
  int number,
  unsigned int width,
  char pad
) {
  unsigned int value;
  unsigned int digits;

  if (number < 0) {
    /*
     * Avoid overflowing on INT_MIN.
     * -number is not representable as int in that case.
     */
    value = (unsigned int)(-(number + 1)) + 1;
    digits = kdigits(value, 10) + 1;

    kput('-');

    while (digits < width) {
      kput(pad);
      width--;
    }

    kprint_num(value, 10);
  }
  else {
    value = (unsigned int)number;
    kprint_num_width(value, 10, width, pad);
  }
}

static void kprint_str_width(
  const char* str,
  unsigned int width
) {
  unsigned int len = kstrlen(str);

  while (len < width) {
    kput(' ');
    width--;
  }

  kprint(str);
}

void kvprintf(const char* fmt, va_list list) {
  while (*fmt) {
    if (*fmt == '%') {
      unsigned int width = 0;
      char pad = ' ';

      fmt++;

      if (*fmt == 0)
        return;

      /*
       * Parse field width
       * A leading zero selects zero-padding:
       *   %5s  with "hi" -> "   hi"
       *   %03i with  32  -> "032"
       */
      if (*fmt == '0') {
        pad = '0';
        fmt++;
      }

      while (*fmt >= '0' && *fmt <= '9') {
        width = width * 10 + (unsigned int)(*fmt - '0');
        fmt++;
      }

      switch (*fmt) {
        case '%': {
          kput('%');
          break;
        }
        case 'c': {
          char c = (char)va_arg(list, int);

          if (width > 1) {
            unsigned int padding = width - 1;

            while (padding--)
              kput(' ');
          }

          kput(c);
          break;
        }
        case 's': {
          const char* str = va_arg(list, char*);

          str = str ? str : "(null)";
          kprint_str_width(str, width);
          break;
        }
        case 'd':
        case 'i': {
          int i = va_arg(list, int);

          kprint_signed_width(i, width, pad);
          break;
        }
        case 'u': {
          unsigned int i = va_arg(list, unsigned int);

          kprint_num_width(i, 10, width, pad);
          break;
        }
        case 'X':
        case 'x': {
          unsigned int i = va_arg(list, unsigned int);

          kprint_num_width(i, 16, width, pad);
          break;
        }
        case 'o': {
          unsigned int i = va_arg(list, unsigned int);

          kprint_num_width(i, 8, width, pad);
          break;
        }
        case 'b':
        case 'B': {
          unsigned int i = va_arg(list, unsigned int);

          kprint_num_width(i, 2, width, pad);
          break;
        }
        case 'p':
        case 'P': {
          uint32_t i = (uint32_t)va_arg(list, void*);

          kprint_num_width(i, 16, width, pad);
          break;
        }
      } /* switch (*fmt) */
    } /* if (*fmt == '%') */
    else {
      kput(*fmt);
    }

    fmt++;
  } /* while (*fmt) */
}

__attribute__ ((format (printf, 1, 2)))
void kprintf(const char* fmt, ...) {
  va_list list;
  va_start(list, fmt);
  kvprintf(fmt, list);
  va_end(list);
}
