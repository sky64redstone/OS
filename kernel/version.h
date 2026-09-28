#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_FIX   0

#ifndef STRINGIFY
  #define STRINGIFY_IMPL(x) #x
  #define STRINGIFY(x) STRINGIFY_IMPL(x)
#endif

#define VERSION_STRING STRINGIFY(VERSION_MAJOR) "." \
  STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_FIX)

#define str_welcome \
  "   ___  ____   :\n" \
  "  / _ \\/ ___|  :\n" \
  " | | | \\___ \\  :\n" \
  " | |_| |___) | :\n" \
  "  \\___/|____/  : Self-written operating system in c and asm\n" \
  "...............: version " VERSION_STRING "\n"
