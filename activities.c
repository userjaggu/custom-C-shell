#include "headers.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_PROCESSES 1024

//     signal(SIGINT, sigint_handler);
//  signal(SIGTSTP, sigtstp_handler);

// Structure to store information about each process
typedef struct {
    int pid;
    char command[256];
    char state[16];  // "Running" or "Stopped"
} ProcessInfo;

ProcessInfo process_list[MAX_PROCESSES];
int process_count = 0;
pid_t fg_pid = -1;  // Foreground process ID

// Function to add a new process to the list
void add_process(int pid, char *command) {
    if (process_count >= MAX_PROCESSES) {
        fprintf(stderr, "Process list is full!\n");
        return;
    }

    process_list[process_count].pid = pid;
    strcpy(process_list[process_count].command, command);
    strcpy(process_list[process_count].state, "Running");  // Default to running
    process_count++;

    
    // printf("Added process: PID=%d, Command=%s\n", pid, command);
}

// Function to update the state of processes
void update_process_state() {
    for (int i = 0; i < process_count; i++) {
        int status;
        pid_t result = waitpid(process_list[i].pid, &status, WNOHANG | WUNTRACED);

        if (result == 0) {
            // Process is still running
            strcpy(process_list[i].state, "Running");
        } else if (result > 0) {
            if (WIFSTOPPED(status)) {
                strcpy(process_list[i].state, "Stopped");
            } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
                // Process has exited, mark as terminated
                strcpy(process_list[i].state, "Terminated");
            }
        }
    }
}
                        
// Comparison function for qsort to sort processes lexicographically by command name
int compare_processes(const void *a, const void *b) {
    ProcessInfo *p1 = (ProcessInfo *)a;
    ProcessInfo *p2 = (ProcessInfo *)b;
    return strcmp(p1->command, p2->command);
}

// Function to sort the process list
void sort_processes() {
    qsort(process_list, process_count, sizeof(ProcessInfo), compare_processes);
}

// Function to display the list of processes
void display_activities() {
    // Update process states before displaying
    update_process_state();

    // Sort the processes lexicographically by command name
    sort_processes();

    // Print the list of processes
    for (int i = 0; i < process_count; i++) {
        printf("%d : %s - %s\n", process_list[i].pid, process_list[i].command, process_list[i].state);
    }
}

// Function to execute a command using fork and exec
void execute_activity(char *command) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the command
        char *argv[64];
        char *token = strtok(command, " \n");
        int argc = 0;

        while (token != NULL) {
            argv[argc++] = token;
            token = strtok(NULL, " \n");
        }
        argv[argc] = NULL;

        if (execvp(argv[0], argv) < 0) {
            // perror("Execution failed");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        // Parent process: add the child to the process list
        char original_command[256];
        strcpy(original_command, command);  // Copy the command to avoid strtok modifications
        add_process(pid, original_command);

        // Set foreground process
        fg_pid = pid;
    } else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
}

// Signal handler for Ctrl-C (SIGINT)
void sigint_handler(int sig) {
    if (fg_pid > 0) {
        kill(fg_pid, SIGINT);  // Send SIGINT to the foreground process
        printf("Sent SIGINT to process %d\n", fg_pid);
    }
}

// Signal handler for Ctrl-Z (SIGTSTP)
void sigtstp_handler(int sig) {
    if (fg_pid > 0) {
        kill(fg_pid, SIGTSTP);  // Send SIGTSTP to the foreground process
        printf("Sent SIGTSTP to process %d\n", fg_pid);
        for (int i = 0; i < process_count; i++) {
            if (process_list[i].pid == fg_pid) {
                strcpy(process_list[i].state, "Stopped");
                break;
            }
        }
        fg_pid = -1;  // Clear foreground process
    }
}

// Function to handle the ping command to send signals
void ping_command(int pid, int signal) {
    signal %= 32;  // Signal modulo 32 as per specification

    if (kill(pid, signal) == -1) {
        if (errno == ESRCH) {
            printf("No such process found\n");
        } else {
            perror("Error sending signal");
        }
    } else {
        printf("Sent signal %d to process %d\n", signal, pid);
        if (signal == SIGTSTP) {
            for (int i = 0; i < process_count; i++) {
                if (process_list[i].pid == pid) {
                    strcpy(process_list[i].state, "Stopped");
                    break;
                }
            }
        }
    }
}

void setup_signal_handlers() {
    // Install signal handlers for Ctrl-C and Ctrl-Z
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}


// Function to handle the fg command
void fg_command(int pid) {
    int found = 0;
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid) {
            if (strcmp(process_list[i].state, "Stopped") == 0 || strcmp(process_list[i].state, "Running") == 0) {
                found = 1;
                printf("Bringing process %d (%s) to foreground\n", pid, process_list[i].command);
                fg_pid = pid;
                kill(pid, SIGCONT);  // Continue if the process is stopped
                waitpid(pid, NULL, 0);  // Wait for the process to finish in the foreground
                strcpy(process_list[i].state, "Running");  // Mark as running
                break;
            }
        }
    }
    if (!found) {
        printf("No such process found\n");
    }
}

// Function to handle the bg command
void bg_command(int pid) {
    int found = 0;
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid) {
            if (strcmp(process_list[i].state, "Stopped") == 0) {
                found = 1;
                printf("Resuming process %d (%s) in background\n", pid, process_list[i].command);
                kill(pid, SIGCONT);  // Continue the stopped process
                strcpy(process_list[i].state, "Running");
                break;
            }
        }
    }
    if (!found) {
        printf("No such process found\n");
    }
}
