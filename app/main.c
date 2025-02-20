#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "builtins.h"
#include "shell.h"

void get_input(char **input) {
    printf("$ ");
    fflush(stdout);

    set_terminal_raw_mode();
    char *inp_buffer = malloc((100 + 1) * sizeof(char));
    int i = 0;
    while (true) {
        const char c = getchar();
        if (c == '\t') {
            inp_buffer[i] = '\0';
            char **potential_inputs = autocomplete_input_buffer(&inp_buffer);
            if (potential_inputs == NULL) {
                continue;
            }

            i += strlen(potential_inputs[0]);
            strcat(inp_buffer, potential_inputs[0]);
            inp_buffer[i] = '\0';

            printf("\r$ %s", inp_buffer);
            fflush(stdout);

            free_remaining_autocomplete_buffer(potential_inputs, -1);
            continue;
        }

        if (c == '\b' || c == 127) {
            if (i == 0) {
                continue;
            }
            i--;
            inp_buffer[i] = '\0';
            printf("\r$ %s \b", inp_buffer);
            fflush(stdout);
            continue;
        }

        printf("%c", c);
        fflush(stdout);
        if (c == '\n') {
            inp_buffer[i] = '\0';
            break;
        }

        inp_buffer[i] = c;
        i += 1;
    }

    restore_terminal_mode();
    *input = inp_buffer;
}


int main() {
    setbuf(stdout, NULL);
    initialize_autocomplete();


    while (1) {
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
