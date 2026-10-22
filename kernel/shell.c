#include <stdint.h>

#include "shell.h"

#include "kernel/kio.h"
#include "mm/page.h"
#include "drivers/vga/text.h"
#include "kernel/version.h"

#define SHELL_LINE_SIZE 128

/* TODO global panic impl */
#define kpanic(...)     \
  kprintf(__VA_ARGS__); \
  while (1)             \
    asm volatile ("cli; hlt")


struct shell_command {
  const char* name;
  const char* description;
  void (*function)(const char* arguments);
};

static char shell_line[SHELL_LINE_SIZE];
static unsigned int shell_line_length;

static void shell_print_prompt() {
  kprint("[root]# ");
}

static int shell_string_equal(
  const char* left,
  const char* right
) {
  while (*left != '\0' && *right != '\0') {
    if (*left != *right) {
      return 0;
    }

    left++;
    right++;
  }

  return *left == '\0' && *right == '\0';
}

static char* shell_skip_spaces(char* string) {
  while (*string == ' ' || *string == '\t') {
    string++;
  }

  return string;
}

static void test_paging();
static void test_frame_alloc();

static void shell_command_help(const char* arguments);
static void shell_command_clear(const char* arguments);
static void shell_command_echo(const char* arguments);
static void shell_command_version(const char* arguments);
static void shell_command_test(const char* arguments);
static void shell_command_frame_regions(const char* arguments);

static const struct shell_command shell_commands[] = {
  {
    "help",
    "list available commands",
    shell_command_help
  },
  {
    "clear",
    "clear the screen",
    shell_command_clear
  },
  {
    "echo",
    "print text",
    shell_command_echo
  },
  {
    "version",
    "show kernel version",
    shell_command_version
  },
  {
    "test",
    "test various kernel functions",
    shell_command_test
  },
  {
    "regions",
    "print all memory frame regions",
    shell_command_frame_regions
  }
};

#define SHELL_COMMAND_COUNT \
  (sizeof(shell_commands) / sizeof(shell_commands[0]))

static void shell_command_help(const char* arguments) {
  (void)arguments;

  kprint("Available commands:\n");

  for (unsigned int i = 0; i < SHELL_COMMAND_COUNT; i++) {
    kprintf(
      "  %s - %s\n",
      shell_commands[i].name,
      shell_commands[i].description
    );
  }
}

static void shell_command_clear(const char* arguments) {
  (void)arguments;

  vga_clear_screen();
}

static void shell_command_echo(const char* arguments) {
  kprint(arguments);
  kput('\n');
}

static void shell_command_version(const char* arguments) {
  (void)arguments;

  kprint(str_welcome);
}

static void shell_command_test(const char* arguments) {
  if (shell_string_equal(arguments, "paging")) {
    test_paging();
    return;
  }
  if (shell_string_equal(arguments, "frame_alloc")) {
    test_frame_alloc();
    return;
  }

  kprint(
    "Usage: test FUNCTION\n"
    "\n"
    "Supported values:\n"
    "paging - Test kernel paging and memory translation\n"
    "frame_alloc - Test allocating and freeing frames\n"
  );
}

static void shell_command_frame_regions(const char* arguments) {
  (void)arguments;
  frame_print_regions();
}

static void shell_execute() {
  char* command;
  char* arguments;
  unsigned int i;

  shell_line[shell_line_length] = '\0';

  command = shell_skip_spaces(shell_line);

  if (*command == '\0') {
    return;
  }

  arguments = command;

  while (*arguments != '\0' &&
         *arguments != ' ' &&
         *arguments != '\t') {
    arguments++;
  }

  if (*arguments != '\0') {
    *arguments = '\0';
    arguments++;
    arguments = shell_skip_spaces(arguments);
  }

  for (i = 0; i < SHELL_COMMAND_COUNT; i++) {
    if (shell_string_equal(command, shell_commands[i].name)) {
      shell_commands[i].function(arguments);
      return;
    }
  }

  kprintf(
    "Unknown command: %s\n",
    command
  );
}

void shell_init() {
  shell_line_length = 0;
  shell_line[0] = '\0';

  shell_print_prompt();
}

void shell_input(char character) {
  if (character == '\n') {
    kput('\n');

    shell_execute();

    shell_line_length = 0;
    shell_line[0] = '\0';

    shell_print_prompt();
    return;
  }

  if (character == '\b') {
    if (shell_line_length == 0) {
      return;
    }

    shell_line_length--;
    shell_line[shell_line_length] = '\0';

    kput('\b');
    return;
  }

  if (character == '\t') {
    return;
  }

  if (character < 0x20 || character > 0x7E) {
    return;
  }

  if (shell_line_length >= SHELL_LINE_SIZE - 1) {
    return;
  }

  shell_line[shell_line_length] = character;
  shell_line_length++;

  shell_line[shell_line_length] = '\0';

  kput(character);
}

/*
 * Test code region
 */

static void test_paging() {
  uint32_t physical = frame_alloc();

  if (physical == FRAME_ALLOC_FAILED) {
    /* FIXME a shell command shouldn't panic on purpose */
    kpanic("Failed to allocate a frame\n");
  }

  if (!paging_map_page(0x40000000, physical, PAGE_WRITE)) {
    frame_free(physical);
    kpanic("Failed to map a page\n");
  }

  volatile uint32_t* test_address =
    (volatile uint32_t*)(uintptr_t)0x40000000;

  *test_address = 0x12345678;

  if (*test_address != 0x12345678) {
    kpanic("Test data not consistent\n");
  }

  uint32_t translated;

  if (!paging_translate(0x40000000, &translated)) {
    kpanic("Translation failed\n");
  }

  if (translated != physical) {
    kpanic("Translation doesn't match physical address\n");
  }

  kprint("All tests passed!\n");
}

static void test_frame_alloc() {
  /* TODO: alloc 2 frames, free the first, then print */
  kprint("\nREGIONS==============\n");
  frame_print_regions();
  kprint("=====================\n");

  uint32_t frame = frame_alloc();
  kprintf("Allocated frame %x\n", frame);

  kprint("\nREGIONS==============\n");
  frame_print_regions();
  kprint("=====================\n");

  int success = frame_free(frame);
  if (!success) {
    kprint("Failed to free frame\n");
    return;
  }
  kprintf("Freed frame %x\n", frame);

  kprint("\nREGIONS==============\n");
  frame_print_regions();
  kprint("=====================\n");
}
