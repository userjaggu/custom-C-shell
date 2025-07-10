#include "headers.h"

#define BUFFER_SIZE 1024

int print_usage()
{
    fprintf(stderr, "Usage: proclore [pid]\n");
    return EXIT_FAILURE;
}

int print_process_info(pid_t pid)
{
    char path[BUFFER_SIZE];

    printf("pid : %d\n", pid);

    if (print_process_status(pid) != 0)
    {
        return EXIT_FAILURE;
    }

    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *file = fopen(path, "r");
    if (file)
    {
        int ppid, pgrp;
        if (fscanf(file, "%*d %*s %*c %d %d", &ppid, &pgrp) == 2)
        {
            printf("Process Group : %d\n", pgrp);
        }
        else
        {
            perror("Error reading process group");
            fclose(file);
            return EXIT_FAILURE;
        }
        fclose(file);
    }
    else
    {
        perror("Error opening /proc/stat");
        return EXIT_FAILURE;
    }

    snprintf(path, sizeof(path), "/proc/%d/statm", pid);
    file = fopen(path, "r");
    if (file)
    {
        unsigned long vm_size;
        if (fscanf(file, "%lu", &vm_size) == 1)
        {
            printf("Virtual memory : %lu\n", vm_size * getpagesize() / 1024);
        }
        else
        {
            perror("Error reading virtual memory");
            fclose(file);
            return EXIT_FAILURE;
        }
        fclose(file);
    }
    else
    {
        perror("Error opening /proc/statm");
        return EXIT_FAILURE;
    }

    if (print_executable_path(pid) != 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int print_process_status(pid_t pid)
{
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *file = fopen(path, "r");
    if (file)
    {
        char line[BUFFER_SIZE];
        while (fgets(line, sizeof(line), file))
        {
            if (strncmp(line, "State:", 6) == 0)
            {
                char state;
                if (sscanf(line, "State: %c", &state) == 1)
                {
                    switch (state)
                    {
                    case 'R':
                        printf("process status : R\n");
                        break;
                    case 'S':
                        printf("process status : S\n");
                        break;
                    case 'Z':
                        printf("process status : Z\n");
                        break;
                    default:
                        printf("process status : Unknown\n");
                        break;
                    }
                }
                fclose(file);
                return EXIT_SUCCESS;
            }
        }
        fclose(file);
    }
    else
    {
        perror("Error opening process status file");
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}

int print_executable_path(pid_t pid)
{
    char path[BUFFER_SIZE];
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    char exe_path[BUFFER_SIZE];
    ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
    if (len != -1)
    {
        exe_path[len] = '\0';
        printf("executable path : %s\n", exe_path);
        return EXIT_SUCCESS;
    }
    else
    {
        perror("Error reading executable path");
        return EXIT_FAILURE;
    }
}

int run_proclore(int pid)
{
    return print_process_info(pid);
}
