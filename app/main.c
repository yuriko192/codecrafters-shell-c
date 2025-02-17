#include <stdio.h>
#include <string.h>

const char EXIT_COMMAND[] = "exit";
const char ECHO_COMMAND[] = "echo";
const char TYPE_COMMAND[] = "type";

const int EXIT_COMMAND_LEN = strlen(EXIT_COMMAND);
const int ECHO_COMMAND_LEN = strlen(ECHO_COMMAND);
const int TYPE_COMMAND_LEN = strlen(TYPE_COMMAND);

const char *BUILTIN_COMMAND_LIST[] = {
  EXIT_COMMAND,
  ECHO_COMMAND,
  TYPE_COMMAND
};

const size_t BUILTIN_COMMAND_COUNT = sizeof(BUILTIN_COMMAND_LIST) / sizeof(BUILTIN_COMMAND_LIST[0]);


int is_valid_builtin_command(char *inp_cmd)
{
  for (size_t i = 0; i < BUILTIN_COMMAND_COUNT; i++)
  {
    if (strcmp(inp_cmd, BUILTIN_COMMAND_LIST[i]) == 0) return 1;
  }
  return 0;
}

void execute_type_command(char *inp_cmd)
{
  if (is_valid_builtin_command(inp_cmd))
  {
    printf("%s is a shell builtin\n", inp_cmd);
    return;
  }

  printf("%s: not found\n", inp_cmd);
}

int main()
{
  // Flush after every printf
  setbuf(stdout, NULL);

  // Wait for user input
  char input[100];

  while (1)
  {
    printf("$ ");
    fgets(input, 100, stdin);

    input[strlen(input) - 1] = '\0';

    if (strncmp(input, EXIT_COMMAND, EXIT_COMMAND_LEN) == 0)
    {
      return 0;
    }

    if (strncmp(input, ECHO_COMMAND, ECHO_COMMAND_LEN) == 0)
    {
      printf("%s\n", input + ECHO_COMMAND_LEN + 1);
      continue;
    }

    if (strncmp(input, TYPE_COMMAND, TYPE_COMMAND_LEN) == 0)
    {
      execute_type_command(input + TYPE_COMMAND_LEN + 1);
      continue;
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
