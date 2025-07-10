#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>



void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void print_long_format(struct dirent *entry, char *path) {
    char full_path[BUFFER_SIZE];
    struct stat fileStat;
    
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
    if (stat(full_path, &fileStat) < 0) {
        perror("stat");
        return;
    }

    print_permissions(fileStat.st_mode);
    printf(" %ld", fileStat.st_nlink);
    
    struct passwd *pwd = getpwuid(fileStat.st_uid);
    struct group *grp = getgrgid(fileStat.st_gid);
    printf(" %s", pwd ? pwd->pw_name : "unknown");
    printf(" %s", grp ? grp->gr_name : "unknown");
    printf(" %ld", fileStat.st_size);
    
    char time_buf[100];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&fileStat.st_mtime));
    printf(" %s", time_buf);
    
    printf(" %s\n", entry->d_name);
}



void read_files(char* path, int long_format, int show_hidden) {
    DIR *dir;
    struct dirent **entries;
    int n;

    dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return;
    }

    n = scandir(path, &entries, NULL, alphasort); 
    if (n < 0) {
        perror("scandir");
        closedir(dir);
        return;
    }

    if (long_format) {
        printf("total %d\n", n);
        for (int i = 0; i < n; i++) {
            if (entries[i]->d_name[0] == '.' && !show_hidden) {
                free(entries[i]);
                continue;
            }
            print_long_format(entries[i], path);
            free(entries[i]);
        }
    } else {
        for (int i = 0; i < n; i++) {
            if (entries[i]->d_name[0] == '.' && !show_hidden) {
                free(entries[i]);
                continue;
            }
            if (entries[i]->d_type == DT_DIR) {
                printf(COLOR_BLUE "%s" COLOR_RESET "\n", entries[i]->d_name);
            } else if (entries[i]->d_type == DT_REG) {
                if (access(entries[i]->d_name, X_OK) == 0) {
                    printf(COLOR_GREEN "%s" COLOR_RESET "\n", entries[i]->d_name);
                } else {
                    printf(COLOR_WHITE "%s" COLOR_RESET "\n", entries[i]->d_name);
                }
            }
            free(entries[i]);
        }
    }
    free(entries);
    closedir(dir);
}


int exec_reveal(char* command) {
    int long_format = 0;
    int show_hidden = 0;
    char* path = NULL;

    char* command_copy = strdup(command);
    char* flags = strtok(command_copy, " ");
    while (flags != NULL) {
        if (flags[0] == '-') {
            if (strchr(flags, 'l')) long_format = 1;
            if (strchr(flags, 'a')) show_hidden = 1;
        } else {
            path = resolve_path(flags);
        }
        flags = strtok(NULL, " ");
    }

    if (path == NULL) {
        path = resolve_path(".");
    }

    read_files(path, long_format, show_hidden);
    free(path);
    free(command_copy);
    return 0; // SUCCESS
}

char* resolve_path(char *path) {
    char *resolved_path = realpath(path, NULL);
    if (resolved_path != NULL) {
    return resolved_path;
    }

    return NULL;
}


