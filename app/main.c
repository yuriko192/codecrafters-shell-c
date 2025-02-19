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

void get_input(char **input) {
    // fgets(inp_buffer, 100, stdin);
    char *inp_buffer = malloc((100 + 1) * sizeof(char));
    int i = 0;
    while (true) {
        const char c = getchar();
        if (c == '\n') {
            inp_buffer[i] = '\0';
            break;
        }
        if (c == '\t') {
            char ** potential_inputs = autocomplete_input_buffer(&inp_buffer);
            i+=strlen(potential_inputs[0])+1;
            printf("%s ",potential_inputs[0]);
            strcat(inp_buffer,potential_inputs[0]);
            strcat(inp_buffer," ");

            free_remaining_autocomplete_buffer(potential_inputs, 0);
            continue;
        }

        // printf("%c", c);
        inp_buffer[i] = c;
        i += 1;
    }

    // printf("\n");
    *input = inp_buffer;
}


int main() {
    setbuf(stdout, NULL);
    initialize_autocomplete();


    while (1) {
        printf("$ ");

        char *input;
        get_input(&input);

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
