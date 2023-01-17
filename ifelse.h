#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>
#include "consts.h"






void releaseTree(args *root, int length)
{
    args *prev = root, *current = root;
    for(int i = 0 ; i < length; i++)
    {
        prev = current;
        current = current->next;
        free(prev);
    }        
}

int validateErr(args *command,args *root ,int numPipes)
{
    printf("enter validate");
    if(state == THEN_BLOCK && result == IFERR && strcmp(root->command[0],"else"))
    {
        
        // releaseTree(root,numPipes);
        return IFERR;
    }
    
    return IFOK;
}

int checkIgnore(args *command,args *root ,int numPipes)
{
    if(state = ELSE_BLOCK && result == IFOK && strcmp(root->command[0],"fi"))
    {
        // releaseTree(root,numPipes);
        return IFERR;
    }
}

void logIfElseERR(char *buff)
{
    fprintf(stderr,"syntax err: %s \n",buff);
}

int ifelse(args *command,args *root ,int numPipes)
{
    printf("if else \n");
    if(validateErr(command,root,numPipes) == IFERR)
    {
        printf("ok");
         return IFERR;     
    }
    if(checkIgnore(command,root,numPipes) == IFERR)
    {
         return IFERR;     
    }
    if((state == THEN || state == ELSE ) && strcmp(root->command[0],"then"))
    {
        logIfElseERR("expected then");
        return IFERR;
    }
    
    if(strcmp(root->command[0],"if") == 0)
    {
        if(state != REGULAR)
        {
            logIfElseERR("expected if");
            return IFERR;
        }

        char noIfBuffer[1024];
        strncpy(noIfBuffer, lastCommand + 3 ,sizeof(lastCommand) - 3);
        prevState = system(noIfBuffer);
        result = prevState == 0 ? IFOK : IFERR;
        state = result == IFOK? THEN : ELSE ;
    }
    else if(strcmp(root->command[0],"then") == 0)
    {
        if(state == THEN_BLOCK)
        {
            logIfElseERR("expected else");
            return IFERR;
        } 

        if(state == REGULAR)
        {
            logIfElseERR("expected else");
            return IFERR;
        } 

        if(state == ELSE_BLOCK)
        {
            logIfElseERR("expected fi");
            return IFERR;
        }

        state = THEN_BLOCK; 
    }
    else if(strcmp(root->command[0],"else") == 0)
    {
        if(state == REGULAR)
        {
            logIfElseERR("expected if");
            return IFERR;
        }

        if(state == THEN)
        {
            logIfElseERR("expected then");
            return IFERR;
        }

        state = ELSE_BLOCK;
    }
    else if(strcmp(root->command[0],"fi") == 0)
    {
        if(state == THEN)
        {
            logIfElseERR("expected then");
            return IFERR;
        }

        if(state == REGULAR)
        {
            logIfElseERR("expected IF");
            return IFERR;
        }
        if(state == THEN_BLOCK)
        {
            logIfElseERR("expected else");
            return IFERR;
        }
        state = REGULAR;
        result = IFOK;
    }
    
    return 0;
}


