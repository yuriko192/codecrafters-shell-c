#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

const int MAX_ARGC = 10;

const char EXIT_COMMAND[] = "exit";
const char ECHO_COMMAND[] = "echo";
const char TYPE_COMMAND[] = "type";
const char PWD_COMMAND[] = "pwd";
const char CD_COMMAND[] = "cd";

const size_t EXIT_COMMAND_LEN = sizeof(EXIT_COMMAND) - 1;
const size_t ECHO_COMMAND_LEN = sizeof(ECHO_COMMAND) - 1;
const size_t TYPE_COMMAND_LEN = sizeof(TYPE_COMMAND) - 1;
const size_t PWD_COMMAND_LEN = sizeof(PWD_COMMAND) - 1;
const size_t CD_COMMAND_LEN = sizeof(CD_COMMAND) - 1;

const char *BUILTIN_COMMAND_LIST[] = {
    EXIT_COMMAND,
    ECHO_COMMAND,
    TYPE_COMMAND,
    PWD_COMMAND,
    CD_COMMAND
};

const size_t BUILTIN_COMMAND_COUNT = sizeof(BUILTIN_COMMAND_LIST) / sizeof(BUILTIN_COMMAND_LIST[0]);

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

char *get_executable_fullpath(char *inp_cmd) {
    char *path_env = getenv("PATH");
    if (path_env == NULL) {
        return NULL;
    }

    char *path_dirs = strdup(path_env);
    static char full_path[1024];

    char *dir = strtok(path_dirs, ":");
    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, inp_cmd);

        if (access(full_path, X_OK) == 0) {
            // is_executable
            free(path_dirs);
            return full_path;
        }

        dir = strtok(NULL, ":");
    }

    free(path_dirs);
    return NULL;
}

int is_valid_builtin_command(char *inp_cmd) {
    for (size_t i = 0; i < BUILTIN_COMMAND_COUNT; i++) {
        if (strcmp(inp_cmd, BUILTIN_COMMAND_LIST[i]) == 0)
            return 1;
    }
    return 0;
}

void execute_type_command(char *inp_cmd) {
    if (is_valid_builtin_command(inp_cmd)) {
        printf("%s is a shell builtin\n", inp_cmd);
        return;
    }

    char *execPath = get_executable_fullpath(inp_cmd);
    if (execPath != NULL) {
        printf("%s is %s\n", inp_cmd, execPath);
        return;
    }

    printf("%s: not found\n", inp_cmd);
}

void split_string_with_quotes_and_space(char *inp_cmd, char **result, int *result_start_idx,
                                        const int *max_size) {
    /*
     * unhandled case:
     * - outside of quote case: quote inside of string (ex: `asd"bcd`)
     */
    char *second_window = inp_cmd;
    char quote_style = 0;

    char *start_pointer = second_window;

    bool was_space = true;
    bool is_inside_quote = false;
    bool prev_group_is_inside_quote = false;
    for (second_window; *second_window != '\0'; second_window++) {
        if (*second_window == '\'' || *second_window == '"') {
            if (!is_inside_quote) {
                /* start of quote, mark as start of current group */
                start_pointer = second_window + 1;
                is_inside_quote = !is_inside_quote;
                quote_style = *second_window;
                continue;
            } else {
                if (quote_style == *second_window) {
                    /* end of quote, mark as end of current group
                     * append the content of group to result array
                     */
                    prev_group_is_inside_quote = true;

                    int curr_str_len = second_window - start_pointer;
                    result[*result_start_idx] = malloc((curr_str_len + 1) * sizeof(char));
                    strncpy(result[*result_start_idx], start_pointer, curr_str_len);
                    (*result_start_idx) += 1;

                    if (*result_start_idx == *max_size) {
                        return;
                    }

                    is_inside_quote = !is_inside_quote;
                    quote_style = 0;
                    continue;
                }
            }
        }

        if (*second_window == ' ') {
            if (!is_inside_quote) {
                if (was_space) {
                    continue;
                } else {
                    /* outside of quotes
                     * start of space, end of current string group
                     * append string content to result
                     */
                    if (prev_group_is_inside_quote) {
                        continue;
                    }

                    int curr_str_len = second_window - start_pointer;
                    result[*result_start_idx] = malloc((curr_str_len+1) * sizeof(char));
                    strncpy(result[*result_start_idx], start_pointer, curr_str_len);
                    (*result_start_idx) += 1;

                    if (*result_start_idx == *max_size) {
                        return;
                    }
                }
                was_space = true;
            }
        } else {
            /* outside of quotes
             * end of space, mark as start of current string group
             */
            if (was_space && !is_inside_quote) {
                prev_group_is_inside_quote = false;
                start_pointer = second_window;
            }
            was_space = false;
        }
    }

    if (prev_group_is_inside_quote) {
        return;
    }

    int curr_str_len = second_window - start_pointer;
    result[*result_start_idx] = malloc((curr_str_len + 1) * sizeof(char));
    strncpy(result[*result_start_idx], start_pointer, curr_str_len);
    (*result_start_idx) += 1;

    if (*result_start_idx == *max_size) {
        return;
    }

    result[*result_start_idx] = NULL;
};


void trim_space_with_quotes(char *inp_cmd) {
    char *first_window = inp_cmd;
    char *second_window = inp_cmd;
    char quote_style = 0;

    bool was_space = true;
    bool is_inside_quote = false;
    for (second_window; *second_window != '\0'; second_window++) {
        if (*second_window == '\'' || *second_window == '"') {
            if (!is_inside_quote) {
                is_inside_quote = !is_inside_quote;
                quote_style = *second_window;
                continue;
            } else {
                if (quote_style == *second_window) {
                    is_inside_quote = !is_inside_quote;
                    quote_style = 0;
                    continue;
                }
            }
        }

        if (*second_window == ' ') {
            if (!is_inside_quote) {
                if (was_space) {
                    continue;
                }
                was_space = true;
            }
        } else {
            was_space = false;
        }

        *first_window = *second_window;
        first_window++;
    }
    *first_window = '\0';
};

void execute_echo_command(char *inp_cmd) {
    trim_space_with_quotes(inp_cmd);
    printf("%s\n", inp_cmd);
}


void execute_pwd_command() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void execute_cd_command(char *new_dir) {
    if (strncmp(new_dir, "~", 1) == 0) {
        const char *home_dir = getenv("HOME");
        snprintf(new_dir, strlen(new_dir) + strlen(home_dir), "%s%s", home_dir, new_dir + 1);
    }

    int cd_res = chdir(new_dir);
    if (cd_res < 0) {
        printf("cd: %s: No such file or directory\n", new_dir);
    }
}

bool execute_external_process(char *input) {
    char *argv[MAX_ARGC];
    int argc = 0;

    split_string_with_quotes_and_space(input, argv, &argc, &MAX_ARGC);
    char *execPath = get_executable_fullpath(argv[0]);
    if (execPath == NULL) {
        return false;
    }

    int pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return false;
    }

    if (pid == 0) {
        execv(execPath, argv);
        perror("execv");
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return true;
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
