//
// Created by Yuriko on 2/19/2025.
//

#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>


#include "structs.h"
#include "utils.h"


const char EXIT_COMMAND[] = "exit";
const char ECHO_COMMAND[] = "echo";
const char TYPE_COMMAND[] = "type";
const char PWD_COMMAND[] = "pwd";
const char CD_COMMAND[] = "cd";

const char *BUILTIN_COMMAND_LIST[] = {
    EXIT_COMMAND,
    ECHO_COMMAND,
    TYPE_COMMAND,
    PWD_COMMAND,
    CD_COMMAND
};

const size_t EXIT_COMMAND_LEN = sizeof(EXIT_COMMAND) - 1;
const size_t ECHO_COMMAND_LEN = sizeof(ECHO_COMMAND) - 1;
const size_t TYPE_COMMAND_LEN = sizeof(TYPE_COMMAND) - 1;
const size_t PWD_COMMAND_LEN = sizeof(PWD_COMMAND) - 1;
const size_t CD_COMMAND_LEN = sizeof(CD_COMMAND) - 1;
const size_t BUILTIN_COMMAND_COUNT = sizeof(BUILTIN_COMMAND_LIST) / sizeof(BUILTIN_COMMAND_LIST[0]);

struct TrieNode *autocomplete_trie;

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
        char *home_dir = strdup(getenv("HOME"));
        snprintf(new_dir, strlen(new_dir) + strlen(home_dir), "%s%s", home_dir, new_dir + 1);
        free(home_dir);
    }

    int cd_res = chdir(new_dir);
    if (cd_res < 0) {
        printf("cd: %s: No such file or directory\n", new_dir);
    }
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

bool execute_external_process(char *input) {
    char *argv[MAX_ARGC];
    int argc = 0;

    /* Old implementation */

    // char *token = strtok(input, " ");
    // while (token != NULL && argc < 10) {
    //     argv[argc++] = token;
    //     token = strtok(NULL, " ");
    // }
    // argv[argc] = NULL;

    split_string_with_quotes_and_space(input, argv, &argc, &MAX_ARGC);
    /* Test Command:
     * cat '/tmp/foo/f   65' '/tmp/foo/f   31' '/tmp/foo/f   37'
     * should result in no file
     * should not return command not found
     */

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

void initialize_autocomplete() {
    autocomplete_trie = initialize_trie_node();
    for (int i = 0; i< BUILTIN_COMMAND_COUNT; i++) {
        add_to_trie_node(autocomplete_trie, BUILTIN_COMMAND_LIST[i]);
    }
}

constexpr int max_autocomplete = 5;

void autocomplete_input_buffer(char *inp_buffer) {
    struct TrieNode *current_node = get_trie_from_word(autocomplete_trie, inp_buffer);
    char**closest_result = get_closest_result(current_node,max_autocomplete );
}