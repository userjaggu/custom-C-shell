#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>


#define std_cmd_sz 100
#define std_path_sz 200
#define correct_cmd 1
#define incorrect_cmd 0
#define foreground 1
#define background 0
#define successful 1
#define Queue_Size 15
#define FAILURE -1
#define SUCCESS 1
#define BUFFER_SIZE 1024
#define PATH_MAX 4096
#define MAX_CMD_LEN 1024
#define MAX_PIPES 64

#define COLOR_RESET "\x1b[0m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_WHITE "\x1b[37m"

#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET "\033[0m"


#define MAX_COMMAND_LENGTH 1024
#define LOG_FILE "/home/jagadeesh/Videos/C-Shell-OSN-Project-main(1)/C-Shell-OSN-Project-main/command_log.txt"
#define MAX_LOG_ENTRIES 15

// --- Functions ---
int execute(char* command_bunch, int *count, char* home_dir, char* hop_prev_dir);
char* get_name();
char* get_home_dir();
char* fetch_dir(char* home_dir);
void cut_string(char *str, int length_to_cut);
char **arrange_commands(char *command_bunch, int *commands_count);
char *trim_spaces(char *str);
char *remove_extra_spaces(char *str);
char* classify_command(char** conseq_commands, int* process_type, int i);
void exec_iso_cmd(char* command, char* home_dir, char* hop_prev_dir);
int exec_hello();
int exec_cmd_list();
// int clear_scr();
int exec_basic(char* command);
int exec_hop(char* command, char* home_dir, char* hop_prev_dir);
void rearrange(char* path, char* token, char* home_dir, char* hop_prev_dir);
void remove_slash(char* path);
int compare_dirents(struct dirent** a,struct dirent** b);

// reveal command
void print_permissions(mode_t mode);
void print_long_format(struct dirent *entry,char *path);
void read_files(char* path, int long_format, int show_hidden);
int exec_reveal(char *command);
char* resolve_path(char* path);

// proclore command
int print_usage();
int print_process_info(pid_t pid);
int print_process_status(pid_t pid);
int print_executable_path(pid_t pid);
int run_proclore(int pid);

// log command functions
void read_log(char log[][MAX_COMMAND_LENGTH], int *log_count);
void write_log(char log[][MAX_COMMAND_LENGTH], int log_count);
void add_to_log(char *command);
void log_commands();
void log_purge();
void log_execute(int index,char *home_dir,char* hop_prev_dir);
void trim_whitespace(char *str);
void process_command1(char *command_string);
void process_command2(char *command_string);
void process_command3(char *command_string,char *home_dir,char* hop_prev_dir);


// seek command
void seek_para(char* command);
void seek( char *flags,char *search,char *target_directory);
void search_directory(char *dir_path,char *search, int only_dirs, int only_files, int flag_e);
void print_relative_path(char *path,int is_dirs, int is_file);
void handle_e_flag(char *path, int only_dirs, int only_files);
// int is_accessible(char *path, int is_dir);


// input_output_handling function < >> > |
void execute_single_command(char *cmd);
void execute_piped_commands(char *cmd);
void process_command(char *cmd);



// activities + signal handler
void add_process(int pid,char *command);
void update_process_state();
int compare_processes(const void *a,const void *b);
void sort_processes();
void display_activities();
void execute_activity(char *command);

void sigint_handler(int sig);
void ping_command(int pid, int signal);
void setup_signal_handlers();
void fg_command(int pid);
void bg_command(int pid);


// neonate
pid_t get_most_recent_pid();
int is_key_pressed();
void run_neonate(int time_arg);
