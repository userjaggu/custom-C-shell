#include "headers.h"

int exec_hop(char* command, char* home_dir, char* hop_prev_dir)
{
    cut_string(command, 4);

    char* path = (char*) malloc (sizeof(char) * 100);
    bzero(path, 100);

    char* token = strtok(command, "/");
    while(token != NULL)
    {
        rearrange(path, token, home_dir, hop_prev_dir);
        token = strtok(NULL, "/");
    }

    printf("Final Path : %s\n", path);
                
    if(chdir(path) == 0)
        return successful;
    return -1;
}

void rearrange(char* path, char* token, char* home_dir, char* hop_prev_dir)
{  
    if(strcmp(token, ".") == 0)
    {
        char *cwd = (char*) malloc (sizeof(char) * std_path_sz);
        getcwd(cwd, std_path_sz);
        strcpy(path, cwd);
    }
    else if(strcmp(token, "..") == 0)
    {
        if(strlen(path) == 0)
        {
            char *cwd = (char*) malloc (sizeof(char) * std_path_sz);
            getcwd(cwd, std_path_sz);
            strcpy(path, cwd);
        }
        remove_slash(path);
    }
    else if(strncmp(token, "~", 1) == 0)
    {
        strcpy(path, home_dir);
    }
    else if(strncmp(token, "-", 1) == 0)
    {
        printf("%s\n", hop_prev_dir);
        strcpy(path, hop_prev_dir);
    }
    else
    {
        strcat(path, "/");
        strcat(path, token);
    }
}

void remove_slash(char* path)
{
    for (int i = strlen(path) - 2; i > -1; i--)
    {
        if (path[i] == '/')
        {
            path[i] = '\0';
            break;
        }
        path[i] = '\0';
    }
}




