#include <stdio.h>
#include <string.h>

const char EXIT_COMMAND[] = "exit 0";

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Wait for user input
  char input[100];

  while(1){
    printf("$ ");
    fgets(input, 100, stdin);

    input[strlen(input) - 1] = '\0';

    if (strcomp(input, EXIT_COMMAND)==0)
    {
      return 0;
    }
    

    printf("%s: command not found\n", input);
  }
  

  return 0;
}
