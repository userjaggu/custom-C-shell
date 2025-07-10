#include "headers.h"

// #define LOG_FILE "/home/jagadeesh/Videos/C-Shell-OSN-Project-main(1)/C-Shell-OSN-Project-main/command_log.txt"
// #define MAX_LOG_ENTRIES 15
// #define MAX_COMMAND_LENGTH 1024

void read_log(char log[][MAX_COMMAND_LENGTH], int *log_count) {
    FILE *file = fopen(LOG_FILE, "r");
    if (!file) {
        *log_count = 0;
        return;
    }
    
    char line[MAX_COMMAND_LENGTH];
    *log_count = 0;

    while (fgets(line, sizeof(line), file) && *log_count < MAX_LOG_ENTRIES) {
        line[strcspn(line, "\n")] = 0;
        strcpy(log[*log_count], line);
        (*log_count)++;
    }

    fclose(file);
}

void write_log(char log[][MAX_COMMAND_LENGTH], int log_count) {
    FILE *file = fopen(LOG_FILE, "w");
    if (!file) {
        perror("Unable to open log file for writing");
        exit(1);
    }

    for (int i = 0; i < log_count; i++) {
        fprintf(file, "%s\n", log[i]);
    }

    fclose(file);
}

void add_to_log(char *command) {
    char log[MAX_LOG_ENTRIES][MAX_COMMAND_LENGTH];
    int log_count;

    read_log(log, &log_count);

    if (log_count > 0 && strcmp(log[log_count - 1], command) == 0) {
        return;
    }

    if (log_count == MAX_LOG_ENTRIES) {
        for (int i = 1; i < MAX_LOG_ENTRIES; i++) {
            strcpy(log[i - 1], log[i]);
        }
        log_count--;
    }

    strcpy(log[log_count], command);
    log_count++;
    write_log(log, log_count);
}

void log_commands() {
    char log[MAX_LOG_ENTRIES][MAX_COMMAND_LENGTH];
    int log_count;

    read_log(log, &log_count);

    for (int i = log_count - 1; i >= 0; i--) {
        printf("%s\n", log[i]);
    }
}

void log_purge() {
    if (remove(LOG_FILE) != 0) {
        perror("Error deleting log file");
    }
    else{
        printf("log_purge is succesful");
    }
}


void trim_whitespace(char *str) {
    char *end;

    while (isspace(*str)) str++;

    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
}




void log_execute(int index,char *home_dir,char *hop_prev_dir) {
    char log[MAX_LOG_ENTRIES][MAX_COMMAND_LENGTH];
    int log_count;

    read_log(log, &log_count);

    if (index < 0 || index >= log_count) {
        printf("Index out of range.\n");
        return;
    }

    int actual_index = log_count - index - 1;
    exec_iso_cmd(log[actual_index],home_dir,hop_prev_dir);
}

void process_command1(char *command_string) {
    char command[MAX_COMMAND_LENGTH];
    strcpy(command, command_string);

    char *token = strtok(command, "&;");
    while (token != NULL) {
        trim_whitespace(token);

        if (strlen(token) > 0) {
           
            if (strcmp(token, "log") == 0) {
                log_commands();
            } 
            else {
                if (strstr(token, "log") == NULL) {
                    add_to_log(token);
                }
                system(token);
            }
        }

        token = strtok(NULL, "&;");
    }
}


void process_command2(char *command_string) {
    FILE * fptr=fopen(LOG_FILE,"w");
    fclose(fptr);
}


void process_command3(char *command_string,char* home_dir,char *hop_prev_dir) {
    char command[MAX_COMMAND_LENGTH];
    strcpy(command, command_string);

    char *token = strtok(command, "&;");
    while (token != NULL) {
        trim_whitespace(token);
        // printf("roar");
        if (strlen(token) > 0) {
          
            if (strncmp(token, "log_execute ", 12) == 0) {
                int index = atoi(token + 12);
                // printf("!@#$");
                log_execute(index,home_dir,hop_prev_dir);
            } 
            else {
                if (strstr(token, "log") == NULL) {
                    add_to_log(token);
                }
                system(token);
            }
        }

        token = strtok(NULL, "&;");
    }
}
