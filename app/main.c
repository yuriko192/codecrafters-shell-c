#include <stdio.h>
#include <string.h>

const char EXIT_COMMAND[] = "exit";
const int EXIT_COMMAND_LEN = strlen(EXIT_COMMAND);
const char ECHO_COMMAND[] = "echo";
const int ECHO_COMMAND_LEN = strlen(ECHO_COMMAND);
const char TYPE_COMMAND[] = "type";
const int TYPE_COMMAND_LEN = strlen(TYPE_COMMAND);

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
      char *inp_cmd = input + TYPE_COMMAND_LEN + 1;
      if (strcmp(inp_cmd, ECHO_COMMAND) == 0 ||
         strcmp(inp_cmd, EXIT_COMMAND) == 0){
        printf("%s is a shell builtin\n", inp_cmd);
        continue;
      }
    }

    printf("%s: command not found\n", input);
  }

  return 0;
}
