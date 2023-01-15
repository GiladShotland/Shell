#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

int main()
{
    int i, ampersand;
    char command[1024];
    char *token;
    char *argv[10];

    while (1)
    {
        printf("hello: ");
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';  // replace \n with \0

        /* parse command line */
        token = strtok(command, " ");
        for (i = 0; token != NULL; i++)
        {
            argv[i] = token;
            token   = strtok(NULL, " ");
        }
        argv[i] = NULL;

        /* Is command empty */
        if (argv[0] == NULL)
        {
            continue;
        }

        /* Does command line end with & */
        if (!strcmp(argv[i - 1], "&"))
        {
            ampersand   = 1;
            argv[i - 1] = NULL;
        }
        else
        {
            ampersand = 0;
        }

        /* for commands not part of the shell command language */

        if (fork() == 0)
        {
            execvp(argv[0], argv);
        }
        /* parent continues here */
        if (ampersand == 0)
        {
            wait(NULL);
        }
    }
}
