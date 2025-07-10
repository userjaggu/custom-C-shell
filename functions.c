#include "headers.h"

int execute(char* command_bunch, int* count, char* home_dir, char* hop_prev_dir)
{
    int commands_count = 0;
    char **conseq_commands = arrange_commands(command_bunch, &commands_count);
    for (int i=0; i<commands_count; i++)
    {
        int process_type = foreground;
        char* command = classify_command(conseq_commands, &process_type, i);

        if(process_type == foreground)
        {
            exec_iso_cmd(command, home_dir, hop_prev_dir);
        }
        else{
            pid_t pid = fork();
            if(pid == 0)
            {
                printf("Process : %d \n", getpid());
                exec_iso_cmd(command, home_dir, hop_prev_dir);
                return 0;
            }
        }
    }
    return 0;
}

char* get_name()
{
    char *username = (char*) malloc (sizeof(char) * 100);

    // Get the username
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        strcpy(username, pw->pw_name);
    }

    strcat(username, "@");

    // Get the system name
    struct utsname sysinfo;
    if (uname(&sysinfo) == 0) {
        strcat(username, sysinfo.nodename);
    }

    return username;
}

char* get_home_dir()
{
    char *cwd = (char*) malloc (sizeof(char) * std_path_sz);
    if (getcwd(cwd, std_path_sz) != NULL) {
        return cwd;
    }
    exit(0);
}

void cut_string(char *str, int length_to_cut) {
    int original_length = strlen(str);
    if (length_to_cut >= original_length) {
        bzero(str, original_length);
    } else {
        for (int i = length_to_cut; i <= original_length; i++) {
            str[i - length_to_cut] = str[i];
        }
    }
}

char* fetch_dir(char* home_dir)
{
    int home_len = strlen(home_dir);
    char* present_dir = get_home_dir();

    if(strncmp(present_dir, home_dir, home_len) == 0)
    {
        // Add ~ in the beginning
        char *new = (char*) malloc (sizeof(char) * std_path_sz);
        bzero(new, std_path_sz);
        new[0] = '~';
        cut_string(present_dir, home_len);
        strcat(new, present_dir);
        return new;
    }
    else
    {
        return present_dir;
    }
}