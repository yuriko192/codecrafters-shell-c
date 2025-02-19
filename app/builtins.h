//
// Created by Yuriko on 2/19/2025.
//

#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdlib.h>
#include <stdbool.h>


extern const char EXIT_COMMAND[];
extern const char ECHO_COMMAND[];
extern const char TYPE_COMMAND[];
extern const char PWD_COMMAND[];
extern const char CD_COMMAND[];
extern const char *BUILTIN_COMMAND_LIST[];

extern const size_t EXIT_COMMAND_LEN;
extern const size_t ECHO_COMMAND_LEN;
extern const size_t TYPE_COMMAND_LEN;
extern const size_t PWD_COMMAND_LEN;
extern const size_t CD_COMMAND_LEN;
extern const size_t BUILTIN_COMMAND_COUNT;

void execute_echo_command(char *inp_cmd);

void execute_pwd_command();

void execute_cd_command(char *new_dir);

void execute_type_command(char *inp_cmd);

bool execute_external_process(char *input);

#endif //BUILTINS_H
