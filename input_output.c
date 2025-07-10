#include "headers.h"


// Function to execute a single command with optional I/O redirection
void execute_single_command(char *cmd) {
    char *argv[64];
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
    int argc = 0;

    // Tokenize the command
    char *token = strtok(cmd, " \n");
    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            if (input_file != NULL) {
                fprintf(stderr, "Error: Multiple input redirections are not allowed.\n");
                return;
            }
            token = strtok(NULL, " \n");
            if (token != NULL) {
                input_file = token;
            } else {
                fprintf(stderr, "Error: No input file specified.\n");
                return;
            }
        } else if (strcmp(token, ">") == 0) {
            if (output_file != NULL) {
                fprintf(stderr, "Error: Multiple output redirections are not allowed.\n");
                return;
            }
            token = strtok(NULL, " \n");
            if (token != NULL) {
                output_file = token;
                append = 0;
            } else {
                fprintf(stderr, "Error: No output file specified.\n");
                return;
            }
        } else if (strcmp(token, ">>") == 0) {
            if (output_file != NULL) {
                fprintf(stderr, "Error: Multiple output redirections are not allowed.\n");
                return;
            }
            token = strtok(NULL, " \n");
            if (token != NULL) {
                output_file = token;
                append = 1;
            } else {
                fprintf(stderr, "Error: No output file specified.\n");
                return;
            }
        } else {
            argv[argc++] = token;
        }
        token = strtok(NULL, " \n");
    }
    argv[argc] = NULL;

    if (argc == 0) return;  // No command to execute

    // Handle input redirection
    if (input_file) {
        int fd = open(input_file, O_RDONLY);
        if (fd < 0) {
            perror("No such input file found!");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    // Handle output redirection
    if (output_file) {
        int fd;
        if (append) {
            fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd < 0) {
            perror("Failed to open output file!");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    // Execute the command
    if (execvp(argv[0], argv) < 0) {
        perror("Execution failed");
        exit(EXIT_FAILURE);
    }
}

// Function to execute a sequence of piped commands with optional I/O redirection
void execute_piped_commands(char *cmd) {
    char *commands[MAX_PIPES];
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
    int num_pipes = 0;

    // Tokenize the command for pipes
    commands[num_pipes++] = strtok(cmd, "|");
    while ((commands[num_pipes++] = strtok(NULL, "|")) != NULL);
    // Remove the last NULL assignment from strtok
    num_pipes--;

    // Error: Invalid pipe usage if a command is missing before or after pipe
    for (int i = 0; i < num_pipes; i++) {
        if (commands[i] == NULL || strlen(commands[i]) == 0) {
            fprintf(stderr, "Invalid use of pipe\n");
            return;
        }
    }

    // Create pipes
    int pipefds[2 * (num_pipes - 1)];
    for (int i = 0; i < num_pipes - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_pipes; i++) {
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            // Redirect input for non-first commands
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }

            // Redirect output for non-last commands
            if (i < num_pipes - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipe file descriptors
            for (int j = 0; j < 2 * (num_pipes - 1); j++) {
                close(pipefds[j]);
            }

            // Execute the current command
            execute_single_command(commands[i]);

            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process closes all pipe fds and waits for all children
    for (int i = 0; i < 2 * (num_pipes - 1); i++) {
        close(pipefds[i]);
    }
    for (int i = 0; i < num_pipes; i++) {
        wait(NULL);
    }
}

// int main() {
//     // Sample command that combines pipes and redirection
//     char command[MAX_CMD_LEN] = "cat < jaggu.txt | wc | cat > newfile.txt";
    
//     // Execute command with pipes and redirection
//     execute_piped_commands(command);

//     return 0;
// }
