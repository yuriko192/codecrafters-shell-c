#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

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
    CD_COMMAND};

const size_t BUILTIN_COMMAND_COUNT = sizeof(BUILTIN_COMMAND_LIST) / sizeof(BUILTIN_COMMAND_LIST[0]);

bool contains_quotes(char *inp)
{
    for (size_t i = 0; i < strlen(inp); i++)
    {
        if (inp[i] == '\'' || inp[i] == '\"' || inp[i] == '`')
        {
            return true;
        }
    }
    return false;
}

void **split_by_quotes(char *inp, char *argv[], int *argc)
{

    char buffer[256];
    int quoteCounter = 0;
    int j = 0;

    for (size_t i = 0; i < strlen(inp); i++, j++)
    {
        if (inp[i] != '\'')
        {
            buffer[j] = inp[i];
            continue;
        }

        quoteCounter++;
        if (quoteCounter % 2 == 1)
        {
            j = -1;
            continue;
        }

        buffer[j + 1] = '\0';
        argv[*argc] = strdup(buffer);
        printf("argc: %d, argv[argc]: %s\n", *argc, argv[*argc]);
        (*argc) += 1;
    }
}

char *get_executable_fullpath(char *inp_cmd)
{
    char *path_dirs = strdup(getenv("PATH"));
    static char full_path[1024];

    char *dir = strtok(path_dirs, ":");
    while (dir != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, inp_cmd);

        if (access(full_path, X_OK) == 0)
        {
            // is_executable
            free(path_dirs);
            return full_path;
        }

        dir = strtok(NULL, ":");
    }

    free(path_dirs);
    return NULL;
}

int is_valid_builtin_command(char *inp_cmd)
{
    for (size_t i = 0; i < BUILTIN_COMMAND_COUNT; i++)
    {
        if (strcmp(inp_cmd, BUILTIN_COMMAND_LIST[i]) == 0)
            return 1;
    }
    return 0;
}

void execute_type_command(char *inp_cmd)
{
    if (is_valid_builtin_command(inp_cmd))
    {
        printf("%s is a shell builtin\n", inp_cmd);
        return;
    }

    char *execPath = get_executable_fullpath(inp_cmd);
    if (execPath != NULL)
    {
        printf("%s is %s\n", inp_cmd, execPath);
        return;
    }

    printf("%s: not found\n", inp_cmd);
}

void execute_echo_command(char *inp_cmd)
{
    char *inputs[10];
    int argc = 0;
    split_by_quotes(inp_cmd, inputs, &argc);
    printf("%s\n", inputs[0]);
}

void execute_pwd_command()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void execute_cd_command(char *new_dir)
{

    if (strncmp(new_dir, "~", 1) == 0)
    {
        const char *home_dir = getenv("HOME");
        snprintf(new_dir, strlen(new_dir) + strlen(home_dir), "%s%s", home_dir, new_dir + 1);
    }

    int cd_res = chdir(new_dir);
    if (cd_res < 0)
    {
        printf("cd: %s: No such file or directory\n", new_dir);
    }
}

int execute_external_process(char *input)
{
    char *remaining_inp = input;
    char *argv[10];
    int argc = 0;

    char *token = strtok_r(input, " ", &remaining_inp);
    argv[argc] = token;
    argc++;

    if (contains_quotes(remaining_inp))
    {
        split_by_quotes(remaining_inp, argv, &argc);
    }
    else
    {
        while ((token = strtok_r(NULL, " ", &remaining_inp)) && argc < 10)
        {
            argv[argc] = token;
            argc++;
        }
    }

    argv[argc] = NULL;

    char *execPath = get_executable_fullpath(argv[0]);
    if (execPath == NULL)
    {
        return -1;
    }

    int pid = fork();
    if (pid == -1)
    {
        perror("fork failed");
        return -1;
    }

    if (pid == 0)
    {
        execv(execPath, argv);
        perror("execv");
        exit(1);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}

int main()
{
    setbuf(stdout, NULL);

    char input[100];

    while (1)
    {
        printf("$ ");
        fgets(input, 100, stdin);

        input[strlen(input) - 1] = '\0';

        if (strncmp(input, EXIT_COMMAND, EXIT_COMMAND_LEN) == 0)
        {
            return 0;
        }

        if (strncmp(input, ECHO_COMMAND, ECHO_COMMAND_LEN) == 0)
        {
            execute_echo_command(input + ECHO_COMMAND_LEN + 1);
            continue;
        }

        if (strncmp(input, TYPE_COMMAND, TYPE_COMMAND_LEN) == 0)
        {
            execute_type_command(input + TYPE_COMMAND_LEN + 1);
            continue;
        }

        if (strncmp(input, PWD_COMMAND, PWD_COMMAND_LEN) == 0)
        {
            execute_pwd_command();
            continue;
        }

        if (strncmp(input, CD_COMMAND, CD_COMMAND_LEN) == 0)
        {
            execute_cd_command(input + CD_COMMAND_LEN + 1);
            continue;
        }

        int pid = execute_external_process(input);
        if (pid == 0)
        {
            continue;
        }

        printf("%s: command not found\n", input);
    }

    return 0;
}
