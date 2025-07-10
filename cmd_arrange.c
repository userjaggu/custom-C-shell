#include "headers.h"

char *remove_extra_spaces(char *str) {
    char *dest = str;
    char *data = str;
    while (*data != '\0') {
        while (isspace((unsigned char)*data) && isspace((unsigned char)*(data + 1))) {
            data++;
        }
        *dest++ = *data++;
    }
    *dest = '\0';
    return str;
}

// Function to trim leading and trailing spaces
char *trim_spaces(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;
}

char **arrange_commands(char *command_bunch, int *commands_count) {
    char *delimiters = ";";
    char **commands = NULL;
    int count = 0;
    char *command = command_bunch;
    char *next_command;
    
    while (*command) {
        next_command = strpbrk(command, delimiters);
        if (next_command) {
            char separator = *next_command;
            *next_command = '\0';

            char *trimmed_command = trim_spaces(command);
            char *final_command = remove_extra_spaces(trimmed_command);

            commands = realloc(commands, (count + 1) * sizeof(char *));
            commands[count] = malloc(strlen(final_command) + 3); // +3 for separator, space, and null terminator
            sprintf(commands[count], "%s%c", final_command, separator);

            command = next_command + 1;
            count++;
        } else {
            break;
        }
    }

    if (*command) {
        char *trimmed_command = trim_spaces(command);
        char *final_command = remove_extra_spaces(trimmed_command);

        commands = realloc(commands, (count + 1) * sizeof(char *));
        commands[count] = malloc(strlen(final_command) + 1); // +1 for null terminator
        strcpy(commands[count], final_command);
        count++;
    }

    *commands_count = count;

    return commands;
}

char* classify_command(char** conseq_commands, int* process_type, int i)
{
    if(conseq_commands[i][strlen(conseq_commands[i])-1] == '&')
    {
        *process_type = background;
        conseq_commands[i][strlen(conseq_commands[i]) - 1] = '\0';
    }
    else if(conseq_commands[i][strlen(conseq_commands[i])-1] == ';')
    {
        conseq_commands[i][strlen(conseq_commands[i]) - 1] = '\0';
    }

    return conseq_commands[i];
}



