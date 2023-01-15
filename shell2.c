#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

typedef struct args
{
    char *command[10];
    struct args *next;
    struct args *prev;
} args;

void ctrlcHandler(int dummy)
{
    (void)dummy;
    printf("You typed Control-C!\n");
    return;
}

/*
should be called after forking
*/
void pipesHandler(int numPipes) { (void)numPipes; }

int main()
{
    signal(SIGINT, ctrlcHandler);

    char command[1024];
    char *token;
    char *outfile;
    int i, fd, ampersand, redirect, status;
    int redirect_error = 0;
    int append         = 0;
    int numPipes       = 0;

    while (1)
    {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        /*initializating linked list for piping*/
        args *root = (args *)malloc(sizeof(args)), *current;
        root->next = NULL;
        current    = root;

        /* parse command line */
        i     = 0;
        token = strtok(command, " ");
        while (token != NULL)
        {
            current->command[i] = token;
            token               = strtok(NULL, " ");
            i++;

            if (token != NULL && !strcmp(token, "|"))
            {
                token = strtok(NULL, " ");
                numPipes++;
                current->command[i] = NULL;

                args *next    = (args *)malloc(sizeof(args));
                current->next = next;
                current       = current->next;
                current->next = NULL;

                i = 0;
            }
        }

        current->command[i] = NULL;

        /* Is command empty */
        if (current->command[0] == NULL)
        {
            continue;
        }

        if (!strcmp(current->command[0], "quit"))
        {
            exit(0);
        }

        if (!strcmp(current->command[0], "cd"))
        {
            if (chdir(current->command[1]))
            {
                printf("cd: %s: No such file or directory\n", current->command[1]);
            }
        }
        /* Does command line end with & */
        if (!strcmp(current->command[i - 1], "&"))
        {
            ampersand               = 1;
            current->command[i - 1] = NULL;
        }
        else
        {
            ampersand = 0;
        }
        if (!strcmp(current->command[i - 2], ">"))
        {
            redirect                = 1;
            redirect_error          = 0;
            current->command[i - 2] = NULL;
            outfile                 = current->command[i - 1];
        }
        else if (!strcmp(current->command[i - 2], "2>"))
        {
            redirect                = 1;
            redirect_error          = 1;
            current->command[i - 2] = NULL;
            outfile                 = current->command[i - 1];
        }
        else if (!strcmp(current->command[i - 2], ">>"))
        {
            redirect                = 1;
            redirect_error          = 0;
            append                  = 1;
            current->command[i - 2] = NULL;
            outfile                 = current->command[i - 1];
        }
        else
        {
            redirect = 0;
        }
        /* for commands not part of the shell command language */

        if (fork() == 0)
        {
            /* redirection of IO ? */
            if (redirect)
            {
                if (append)
                {
                    fd = open(outfile, O_WRONLY | O_APPEND);
                    close(STDOUT_FILENO);
                    dup2(fd, 1);
                }
                else
                {
                    int flag = redirect_error ? STDERR_FILENO : STDOUT_FILENO;
                    fd       = creat(outfile, 0660);
                    close(flag);
                    dup(fd);
                    /* stdout is now redirected */
                }

                close(fd);
            }

            if (numPipes > 1)
            {
                int firstPipe[2];
                int secondPipe[2];
                int swich = 0;

                pipe(firstPipe);
                if (numPipes > 1)
                {
                    pipe(secondPipe);
                }
                numPipes--;
                swich     = 1;
                status    = 1;
                pid_t pid = fork();
                if (pid == 0)
                {
                    dup2(firstPipe[1], 1);
                    close(firstPipe[0]);
                    if (numPipes > 0)
                    {
                        close(secondPipe[0]);
                        close(secondPipe[1]);
                    }

                    execvp(current->command[0], current->command);
                    exit(0);
                }
                if (pid != 0)
                {
                    waitpid(pid, &status, 0);
                    close(firstPipe[1]);
                    current = current->next;
                }

                while (numPipes > 0)
                {
                    pid = fork();
                    if (pid == 0)
                    {
                        if (swich % 2 == 1)
                        {
                            dup2(firstPipe[0], 0);
                            dup2(secondPipe[1], 1);
                        }
                        if (swich % 2 == 0)
                        {
                            dup2(firstPipe[1], 1);
                            dup2(secondPipe[0], 0);
                        }

                        execvp(current->command[0], current->command);
                        exit(0);
                    }
                    if (pid != 0)
                    {
                        waitpid(pid, &status, 0);
                        if (swich % 2 == 1)
                        {
                            close(firstPipe[0]);
                            close(secondPipe[1]);
                            pipe(firstPipe);
                        }
                        if (swich % 2 == 0)
                        {
                            close(firstPipe[1]);
                            close(secondPipe[0]);
                            pipe(secondPipe);
                        }

                        current = current->next;
                        swich++;
                        numPipes--;
                    }
                }

                if (swich % 2 == 1)
                {
                    dup2(firstPipe[0], 0);
                }
                if (swich % 2 == 0)
                {
                    dup2(secondPipe[0], 0);
                }

                execvp(current->command[0], current->command);
                exit(0);
            }
            else
            {
                execvp(current->command[0], current->command);
            }
            redirect_error = 0;
            append         = 0;
            redirect       = 0;
        }

        /* parent continues here */
        if (ampersand == 0)
        {
            wait(&status);
        }
    }
}
