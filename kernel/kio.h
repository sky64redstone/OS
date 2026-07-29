#ifndef KIO_H
  #define KIO_H

  typedef struct {
    void (*put)(char);
    void (*print)(const char*);
  } outstream_t;

  extern outstream_t stdout;

  void kput(char c);
  void kprint(const char* str);
  __attribute__ ((format (printf, 1, 2)))
  void kprintf(const char* fmt, ...);

#endif
