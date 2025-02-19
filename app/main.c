#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "builtins.h"



char *get_string_until_delimiter(char *inp, char delimiter) {
    size_t i = 0;
    for (i; i < strlen(inp); i++) {
        if (inp[i] == delimiter) {
            break;
        }
    }

    char *result = malloc((i + 1) * sizeof(char));
    strncpy(result, inp, i);
    return result;
}

bool contains_quotes(char *inp) {
    for (size_t i = 0; i < strlen(inp); i++) {
        if (inp[i] == '\'' || inp[i] == '"') {
            return true;
        }
    }
    return false;
}


int main() {
    setbuf(stdout, NULL);

    char input[100];

    while (1) {
        printf("$ ");
        fgets(input, 100, stdin);

        input[strlen(input) - 1] = '\0';

        if (strncmp(input, EXIT_COMMAND, EXIT_COMMAND_LEN) == 0) {
            return 0;
        }

        if (strncmp(input, ECHO_COMMAND, ECHO_COMMAND_LEN) == 0) {
            execute_echo_command(input + ECHO_COMMAND_LEN + 1);
            continue;
        }

        if (strncmp(input, TYPE_COMMAND, TYPE_COMMAND_LEN) == 0) {
            execute_type_command(input + TYPE_COMMAND_LEN + 1);
            continue;
        }

        if (strncmp(input, PWD_COMMAND, PWD_COMMAND_LEN) == 0) {
            execute_pwd_command();
            continue;
        }

        if (strncmp(input, CD_COMMAND, CD_COMMAND_LEN) == 0) {
            execute_cd_command(input + CD_COMMAND_LEN + 1);
            continue;
        }

        if (execute_external_process(input)) {
            continue;
        }

        printf("%s: command not found\n", input);
    }

    return 0;
}
