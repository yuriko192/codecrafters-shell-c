//
// Created by Yuriko on 2/19/2025.
//

#ifndef UTILS_H
#define UTILS_H

extern const int MAX_ARGC;

void trim_space_with_quotes(char *inp_cmd);
char *get_executable_fullpath(char *inp_cmd);
void split_string_with_quotes_and_space(char *inp_cmd, char **result, int *result_start_idx,
                                        const int *max_size);

#endif //UTILS_H
