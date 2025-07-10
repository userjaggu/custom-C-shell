#include "headers.h"

void seek_para(char* command) {
    char *flags = NULL;
    char *search = NULL;
    char *target_directory = NULL;

    char *command_copy = strdup(command);
    command_copy = command_copy+4;
    char *token = strtok(command_copy, " ");

    if (token != NULL && token[0] == '-') {
        flags = token;
        token = strtok(NULL, " ");
    }

    if (token != NULL) {
        search = token;
        token = strtok(NULL, " ");
    }

    if (token != NULL) {
        target_directory = token;
    }

    if (target_directory == NULL) {
        target_directory = ".";
    }
    
    printf("Directory: %s, Search: %s\n", target_directory, search);

    seek(flags, search, target_directory);
    // free(command_copy);
}

void seek(char *flags, char *search, char *target_directory) {
    int only_dirs = 0;
    int only_files = 0;
    int flag_e = 0;

    if (flags != NULL) {
        for (size_t i = 1; i < strlen(flags); i++) { 
            switch (flags[i]) {
                case 'd':
                    only_dirs = 1;
                    break;
                case 'f':
                    only_files = 1;
                    break;
                case 'e':
                    flag_e = 1;
                    break;
                default:
                    fprintf(stderr, "Invalid flags!\n");
                    return;
            }
        }

        if (only_dirs && only_files) {
            fprintf(stderr, "Invalid flags!\n");
            return;
        }
    }

    search_directory(target_directory, search, only_dirs, only_files, flag_e);
}

void search_directory(char *dir_path, char *search, int only_dirs, int only_files, int flag_e) {
    DIR *dir;
    struct dirent *entry;
    char path[BUFFER_SIZE];
    struct stat statbuf;

    if ((dir = opendir(dir_path)) == NULL) {
        perror("opendir");
        return;
    }

    int match_found = 0;
    int found_file = 0;
    int found_dir = 0;
    char matched_path[BUFFER_SIZE] = {0};

    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        int is_dir = S_ISDIR(statbuf.st_mode);
        int is_file = S_ISREG(statbuf.st_mode);

        if ((only_dirs && !is_dir) || (only_files && !is_file)) {
            continue;
        }

        if (strcmp(entry->d_name, search) == 0) {
            print_relative_path(path, is_dir, is_file);
            match_found = 1;

            if (is_file) found_file = 1;
            if (is_dir) found_dir = 1;

            if (flag_e) {
                strncpy(matched_path, path, sizeof(matched_path) - 1);
                matched_path[sizeof(matched_path) - 1] = '\0'; 
            }
        }

        if (is_dir && entry->d_name[0] != '.') {
            search_directory(path, search, only_dirs, only_files, flag_e);
        }
    }

    closedir(dir);

    if (flag_e && match_found) {
        handle_e_flag(matched_path, only_dirs, only_files);
    }

    if (!match_found) {
        printf("No match found!\n");
    }
}

void print_relative_path(char *path, int is_dir, int is_file) {
    if (is_dir) {
        printf(COLOR_BLUE "%s" COLOR_RESET "\n", path);
    } else if (is_file) {
        printf(COLOR_GREEN "%s" COLOR_RESET "\n", path);
    }
}

void handle_e_flag(char *path, int only_dirs, int only_files) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror("stat");
        printf("Missing permissions for task!\n");
        return;
    }

    if ((only_dirs && !S_ISDIR(statbuf.st_mode)) || (only_files && !S_ISREG(statbuf.st_mode))) {
        printf("No match found!\n");
        return;
    }

    if (S_ISDIR(statbuf.st_mode)) {
        if (access(path, X_OK) != 0) {
            printf("Missing permissions for task!\n");
        } else {
            if (chdir(path) != 0) {
                perror("chdir");
            }
        }
    } else if (S_ISREG(statbuf.st_mode)) {
        if (access(path, R_OK) != 0) {
            printf("Missing permissions for task!\n");
        } else {
            FILE *file = fopen(path, "r");
            if (file == NULL) {
                perror("fopen");
            } else {
                char buffer[256];
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    printf("%s", buffer);
                }
                fclose(file);
            }
        }
    }
}
