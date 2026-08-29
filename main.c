#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "wish.h"

int main(int argc, char *argv[])
{
    FILE *in = stdin;
    char line[1000];
    char paths[100][1000];
    int path_count = 1;

    strcpy(paths[0], "/bin");

    if (argc > 2)
    {
        fprintf(stderr, "An error has occurred\n");
        return 1;
    }

    if (argc == 2)
    {
        in = fopen(argv[1], "r");
        if (in == NULL)
        {
            fprintf(stderr, "An error has occurred\n");
            return 1;
        }
    }

    while (1)
    {
        char *argv_tokens[100];
        int token_count = 0;
        char *token;

        if (in == stdin)
        {
            printf("wish> ");
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), in) == NULL)
        {
            break;
        }

        int background = strchr(line, '&') != NULL;
        pid_t children[100];
        int child_total = 0;

        int should_exit = 0;
        char *remaining_line;
        char *segment = strtok_r(line, "&", &remaining_line);

        while (segment)
        {
            token_count = 0;
            token = strtok(segment, " \t\n");
            while (token != NULL && token_count < 99)
            {
                argv_tokens[token_count++] = token;
                token = strtok(NULL, " \t\n");
            }
            argv_tokens[token_count] = NULL;

            if (token_count == 0)
            {
                segment = strtok_r(NULL, "&", &remaining_line);
                continue;
            }

            if (strcmp(argv_tokens[token_count - 1], ">") == 0)
            {
                fprintf(stderr, "An error has occurred\n");
                segment = strtok_r(NULL, "&", &remaining_line);
                continue;
            }

            char *redirect_file = NULL;
            int redirect_error = 0;

            for (int i = 0; i < token_count; i++)
            {
                char *redirect_ptr = strchr(argv_tokens[i], '>');
                if (!redirect_ptr)
                    continue;

                if (redirect_file != NULL)
                {
                    redirect_error = 1;
                    break;
                }

                if (redirect_ptr == argv_tokens[i] && argv_tokens[i][1] == '\0')
                {
                    if (i == 0 || i != token_count - 2)
                    {
                        redirect_error = 1;
                        break;
                    }
                    redirect_file = argv_tokens[i + 1];
                    argv_tokens[i] = NULL;
                }
                else
                {
                    *redirect_ptr = '\0';
                    if (*(redirect_ptr + 1) == '\0')
                    {
                        redirect_error = 1;
                        break;
                    }
                    redirect_file = redirect_ptr + 1;
                }
            }

            if (redirect_error)
            {
                fprintf(stderr, "An error has occurred\n");
                segment = strtok_r(NULL, "&", &remaining_line);
                continue;
            }

            if (strcmp(argv_tokens[0], "exit") == 0)
            {
                if (token_count != 1)
                {
                    fprintf(stderr, "An error has occurred\n");
                    segment = strtok_r(NULL, "&", &remaining_line);
                    continue;
                }
                should_exit = 1;
                break;
            }

            if (strcmp(argv_tokens[0], "cd") == 0)
            {
                if (token_count != 2)
                {
                    fprintf(stderr, "An error has occurred\n");
                    segment = strtok_r(NULL, "&", &remaining_line);
                    continue;
                }
                if (chdir(argv_tokens[1]) != 0)
                {
                    fprintf(stderr, "An error has occurred\n");
                }
                segment = strtok_r(NULL, "&", &remaining_line);
                continue;
            }

            if (strcmp(argv_tokens[0], "path") == 0)
            {
                path_count = 0;

                for (int i = 1; i < token_count && path_count < 100; i++)
                {
                    strncpy(paths[path_count], argv_tokens[i],
                            sizeof(paths[path_count]) - 1);
                    paths[path_count][sizeof(paths[path_count]) - 1] = '\0';
                    path_count++;
                }
                segment = strtok_r(NULL, "&", &remaining_line);
                continue;
            }

            if (background)
            {
                pid_t pid = fork();
                if (pid == 0)
                {
                    run_external(argv_tokens, redirect_file, paths, path_count);
                    exit(0);
                }
                else if (pid > 0)
                {
                    children[child_total++] = pid;
                }
                else
                {
                    fprintf(stderr, "An error has occurred\n");
                }
            }
            else
            {
                if (run_external(argv_tokens, redirect_file,
                                 paths, path_count) != 0)
                {
                    fprintf(stderr, "An error has occurred\n");
                }
            }

            segment = strtok_r(NULL, "&", &remaining_line);
        }

        for (int i = 0; background && i < child_total; i++)
        {
            waitpid(children[i], NULL, 0);
        }

        if (should_exit)
        {
            break;
        }
    }

    if (in != stdin)
    {
        fclose(in);
    }

    return 0;
}
