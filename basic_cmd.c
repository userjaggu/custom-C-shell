#include "headers.h"

int exec_basic(char* command)
{   
    add_to_log(command);
    // printf("%s\n",command);
    char *args[100];
    int i = 0;
    
    args[i] = strtok(command, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }

    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // In the child process
        if (execvp(args[0], args) < 0) {
            return 0;
        }
    } else {
        // In the parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 1)
            {
                printf("Wrong Command");
            }
        }
        else {
            printf("Command execution failed\n");
         }
    }
    return successful;
}