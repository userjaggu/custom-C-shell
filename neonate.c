#include "headers.h"

pid_t get_most_recent_pid() {
    pid_t pid = -1;
    FILE *file = fopen("/proc/sys/kernel/ns_last_pid", "r");
    if (file) {
        fscanf(file, "%d", &pid);
        fclose(file);
    }
    return pid;
}

int is_key_pressed() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void run_neonate(int time_arg) {
    while (1) {
        pid_t recent_pid = get_most_recent_pid();
        if (recent_pid != -1) {
            printf("%d\n", recent_pid);
        } else {
            fprintf(stderr, "Failed to get the recent PID\n");
            exit(1);
        }

        // Waiting for the time_arg duration
        for (int i = 0; i < time_arg * 10; ++i) {
            usleep(100000); // Sleep for 100ms
            if (is_key_pressed()) {
                char key = getchar();
                if (key == 'x') {
                    printf("user terminated the command by pressing 'x'.\n");
                    exit(0);
                }
            }
        }
    }
}
