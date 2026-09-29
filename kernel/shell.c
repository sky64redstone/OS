#include "shell.h"

#include "kernel/kio.h"
#include "drivers/vga/text.h"
#include "kernel/version.h"

#define SHELL_LINE_SIZE 128

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

static void shell_command_help(const char* arguments);
static void shell_command_clear(const char* arguments);
static void shell_command_echo(const char* arguments);
static void shell_command_version(const char* arguments);

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
