//
// Created by Yuriko on 2/19/2025.
//

#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/stat.h>


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
    // memset(argv, 0, sizeof(argv));

    split_input_args(input, argv, &argc, &MAX_ARGC);
    argv[argc] = 0;

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

const bool is_autocomplete_path = false;
void initialize_autocomplete() {
    autocomplete_trie = initialize_trie_node();
    for (int i = 0; i < BUILTIN_COMMAND_COUNT; i++) {
        add_to_trie_node(autocomplete_trie, BUILTIN_COMMAND_LIST[i]);
    }

    if (! is_autocomplete_path) {
        return;
    }

    char *path_env = getenv("PATH");
    if (path_env == NULL) {
        return;
    }

    char *path_dirs = strdup(path_env);
    char *dir = strtok(path_dirs, ":");
    static char full_path[1024];

    while (dir != NULL) {
        // printf("Starting init: %s\n", dir);
        DIR *dir_content = opendir(dir);

        if (dir_content == NULL) {
            dir = strtok(NULL, ":");
            continue;
        }

        struct dirent *de; // Pointer for directory entry

        // printf("Starting reading dir content: %s\n", dir);
        while ((de = readdir(dir_content)) != NULL) {
            snprintf(full_path, sizeof(full_path), "%s/%s", dir, de->d_name);

            struct stat path_stat;
            if (stat(full_path, &path_stat) != 0) {
                continue;
            }
            if (S_ISDIR(path_stat.st_mode)) {
                continue;
            }

            /* is_executable */
            if (access(full_path, X_OK) == 0) {
                add_to_trie_node(autocomplete_trie, de->d_name);
            }
        }
        closedir(dir_content);
        // printf("Initialization Success: %s\n", dir);
        dir = strtok(NULL, ":");
    }

    free(path_dirs);
}

const int max_autocomplete = 20;

char **autocomplete_input_buffer(char **inp_buffer) {
    struct TrieNode *current_node = get_trie_from_word(autocomplete_trie, *inp_buffer);
    if (current_node == NULL) {
        return NULL;
    }

    char **closest_result = get_closest_result(current_node, max_autocomplete);
    return closest_result;
}

void free_remaining_autocomplete_buffer(char **buffer, int except_i) {
    int i;
    for (i = 0; buffer[i] != NULL; i++) {
        if (i == except_i) {
            continue;
        }

        free(buffer[i]);
    }
    free(buffer);
}
