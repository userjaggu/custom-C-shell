#include "headers.h"

void exec_iso_cmd(char *command, char *home_dir, char *hop_prev_dir)
{
    int action = 0;
     int time_arg;
    if (strncmp(command, "hop", 3) == 0)
    {
        execute_activity(command);
        setup_signal_handlers();
        add_to_log(command);
        action = exec_hop(command, home_dir, hop_prev_dir);
        // execute_activity(command);
        // setup_signal_handlers();
    }
    else if (strncmp(command, "reveal", 6) == 0)
    {
        execute_activity(command);
        setup_signal_handlers();
        add_to_log(command);
        action = exec_reveal(command);
        //  execute_activity(command);
        // setup_signal_handlers();
    }

    else if (strncmp(command, "proclore", 8) == 0)
    {
        execute_activity(command);
        setup_signal_handlers();
        add_to_log(command);
        char *str = strtok(command, " ");
        str = strtok(NULL, " ");
        int pid = 0;

        if (str == NULL)
        {
            pid = getpid();
        }
        else
        {
            pid = atoi(str);
        }
        action = run_proclore(pid);
        //  execute_activity(command);
        // setup_signal_handlers();
        
    }
    else if (strncmp(command, "log", 3) == 0)
    {
        execute_activity(command);
        setup_signal_handlers();
        if (strncmp(command, "log_purge", 9) == 0)
        {
            // printf("jaggu");
            process_command2(command);
            // printf("ko");
        }
        else if (strncmp(command, "log_execute ", 12) == 0)
        {
            process_command3(command, home_dir, hop_prev_dir);
        }
        else
            process_command1(command);
        //    execute_activity(command);
        //     setup_signal_handlers();
    }
    else if (strncmp(command, "seek", 4) == 0)
    {
        execute_activity(command);
        setup_signal_handlers();
        seek_para(command);
        
    }
    else if (strchr(command, '|') != NULL || strchr(command, '<') != NULL || strchr(command, '>') != NULL)
    {   
        execute_activity(command);
        setup_signal_handlers();
        execute_piped_commands(command);
        
    }
    else if (strcmp(command, "activities") == 0)
    {
        setup_signal_handlers();
        display_activities();
    }
    else if (strncmp(command, "ping", 4) == 0)
    {
        int pid, signal;

        if (sscanf(command, "ping %d %d", &pid, &signal) == 2)
        {
            setup_signal_handlers();
            ping_command(pid, signal);
        }
        else
        {
            printf("Invalid ping command. Usage: ping <pid> <signal_number>\n");
        }
    }
    else if (strncmp(command, "fg ", 3) == 0)
    {
        int pid = atoi(command + 3); // Extract PID
        fg_command(pid);
    }
    else if (strncmp(command, "bg ", 3) == 0)
    {
        int pid = atoi(command + 3); // Extract PID
        bg_command(pid);
    }
   
   else if (sscanf(command, "neonate -n %d", &time_arg) == 1 && time_arg > 0) {
        run_neonate(time_arg);
    } 

    else
    {    
         execute_activity(command);
         setup_signal_handlers();
        action = exec_basic(command);
         
    }
}
