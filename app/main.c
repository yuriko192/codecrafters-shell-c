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

    char *inp_buffer = malloc((100 + 1) * sizeof(char));
    int i = 0;

    bool is_tab = false;
    char *original_input_buffer = malloc((100 + 1) * sizeof(char));
    int original_input_len = 0;
    char **potential_inputs = NULL;
    int potential_input_idx = 0;
    int old_input_len = 0;

    set_terminal_raw_mode();
    while (true) {
        const char c = getchar();
        if (c == '\t') {
            inp_buffer[i] = '\0';
            if (is_tab) {
                potential_input_idx++;
            } else {
                is_tab = true;

                strcpy(original_input_buffer, inp_buffer);
                original_input_len = i;

                potential_inputs = autocomplete_input_buffer(&inp_buffer);
                potential_input_idx = 0;

                old_input_len = 0;
            }


            if (potential_inputs == NULL) {
                continue;
            }

            if (potential_inputs[potential_input_idx] == NULL) {
                potential_input_idx = 0;
            }

            char *curr_potential_input = potential_inputs[potential_input_idx];
            int curr_input_len = strlen(curr_potential_input);
            int len_difference = old_input_len - curr_input_len+1;

            i = original_input_len + curr_input_len;
            strcpy(inp_buffer, original_input_buffer);
            strcat(inp_buffer, curr_potential_input);
            inp_buffer[i] = '\0';

            old_input_len = curr_input_len;
            printf("\r$ %s ", inp_buffer);

            /* clean input prompt */
            if (len_difference > 0) {
                for (int j = 0; j < len_difference; ++j) {
                    printf(" ");
                }
                for (int j = 0; j < len_difference; ++j) {
                    printf("\b");
                }
            }

            fflush(stdout);
            continue;
        }

        if (is_tab) {
            free_remaining_autocomplete_buffer(potential_inputs, -1);
            is_tab = false;
            inp_buffer[i] = ' ';
            i++;
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
        i++;
    }

    restore_terminal_mode();
    free(original_input_buffer);
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
