//
// Created by Yuriko on 2/19/2025.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

const int MAX_ARGC = 10;

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

        /* is_executable */
        if (access(full_path, X_OK) == 0) {
            free(path_dirs);
            return full_path;
        }

        dir = strtok(NULL, ":");
    }

    free(path_dirs);
    return NULL;
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
                    result[*result_start_idx] = malloc((curr_str_len + 1) * sizeof(char));
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
