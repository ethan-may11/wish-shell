#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include "wish.h"

int run_external(char *argv_tokens[], char *redirect_file,
                 char paths[][1000], int path_count)
{
    int i;
    int command_found = 0;
    char command_path[1000];
    pid_t child_pid;
    int child_status;

    for (i = 0; i < path_count; i++)
    {
        if (strlen(paths[i]) + strlen(argv_tokens[0]) + 2 >
            sizeof(command_path))
        {
            continue;
        }
        strcpy(command_path, paths[i]);
        strcat(command_path, "/");
        strcat(command_path, argv_tokens[0]);

        if (access(command_path, X_OK) == 0)
        {
            command_found = 1;
            break;
        }
    }

    if (!command_found)
    {
        return 1;
    }

    child_pid = fork();
    if (child_pid < 0)
    {
        return 1;
    }
    else if (child_pid == 0)
    {
        if (redirect_file != NULL)
        {
            int fd = open(redirect_file,
                          O_WRONLY | O_CREAT | O_TRUNC,
                          S_IRWXU);
            if (fd < 0)
            {
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        execv(command_path, argv_tokens);
        exit(1);
    }
    else
    {
        waitpid(child_pid, &child_status, 0);
    }

    return 0;
}
