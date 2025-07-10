#include "headers.h"

int main(){
    char *user = get_name();
    char *home_dir = get_home_dir();
    char *hop_prev_dir = get_home_dir();
    int bg_count = 0;
    
    int exit_code = 0;

    while(1){
        if(exit_code == 1)
        {
            break;
        }

        char *prev = get_home_dir();

        char* working_dir = fetch_dir(home_dir);
        // printf("jaggu");
        printf("\033[0;32m%s : %s > \033[0m", user, working_dir);
        // printf("jag");

        char* command = (char*) malloc (sizeof(char) * std_cmd_sz);
        fgets(command, std_cmd_sz, stdin);
        command[strlen(command) - 1] = '\0';
        execute(command, &bg_count, home_dir, hop_prev_dir);
        strcpy(hop_prev_dir, prev);

        free(command);
    }

}