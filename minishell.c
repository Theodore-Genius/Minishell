//Name: Michael Lamaze  
//Pledge: I pledge my honor that I have abided by the Stevens Honor System

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>
#include <limits.h>

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

void sigint_handler(int sig) {
    interrupted = 1;
}

int main(void) {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        fprintf(stderr, "Error: Cannot register signal handler.\n");
        exit(EXIT_FAILURE);
    }
    char cwd[PATH_MAX];
    char input[1024];
    while (1) {
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            fprintf(stderr, "Error: Cannot get current working directory.\n");
        }
        printf("%s[%s]> %s", BLUE, cwd, DEFAULT);
        fflush(stdout);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (errno == EINTR) {
                clearerr(stdin);
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin.\n");
            continue;
        }
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n')
            input[len - 1] = '\0';
        if (interrupted) {
            interrupted = 0;
            continue;
        }
        if (strlen(input) == 0)
            continue;
        if (strcmp(input, "exit") == 0) {
            exit(EXIT_SUCCESS);
        } else if (strncmp(input, "cd", 2) == 0) {
            strtok(input, " ");
            char *arg = strtok(NULL, " ");
            char *extra = strtok(NULL, " ");
            if (extra != NULL) {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
            } else if (arg == NULL || strcmp(arg, "~") == 0) {
                struct passwd *pw = getpwuid(getuid());
                if (!pw)
                    fprintf(stderr, "Error: Cannot get passwd entry.\n");
                else if (chdir(pw->pw_dir) != 0)
                    fprintf(stderr, "Error: Cannot change directory.\n");
            } else {
                if (chdir(arg) != 0)
                    fprintf(stderr, "Error: Cannot change directory.\n");
            }
        } else if (strcmp(input, "pwd") == 0) {
            if (getcwd(cwd, sizeof(cwd)) == NULL)
                fprintf(stderr, "Error: Cannot get current working directory.\n");
            else
                printf("%s\n", cwd);
        } else if (strcmp(input, "lf") == 0) {
            DIR *d = opendir(".");
            if (d == NULL) {
                fprintf(stderr, "Error: Failed to open directory.\n");
            } else {
                struct dirent *entry;
                while ((entry = readdir(d)) != NULL) {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                        continue;
                    printf("%s\n", entry->d_name);
                }
                closedir(d);
            }
        } else if (strcmp(input, "lp") == 0) {
            DIR *d = opendir("/proc");
            if (!d) {
                fprintf(stderr, "Error: Failed to open /proc.\n");
            } else {
                struct dirent *entry;
                while ((entry = readdir(d)) != NULL) {
                    char *p;
                    long pid = strtol(entry->d_name, &p, 10);
                    if (*p != '\0')
                        continue;
                    char path[256];
                    snprintf(path, sizeof(path), "/proc/%ld/cmdline", pid);
                    FILE *f = fopen(path, "r");
                    if (!f)
                        continue;
                    char cmd[256];
                    if (fgets(cmd, sizeof(cmd), f) != NULL)
                        printf("%ld unknown %s\n", pid, cmd);
                    fclose(f);
                }
                closedir(d);
            }
        } else {
            char *argv[11];
            int argc = 0;
            char *token = strtok(input, " ");
            while (token != NULL && argc < 10) {
                argv[argc++] = token;
                token = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error: fork() failed.\n");
            } else if (pid == 0) {
                if (execvp(argv[0], argv) < 0) {
                    fprintf(stderr, "Error: exec() failed.\n");
                    exit(EXIT_FAILURE);
                }
            } else {
                int status;
                if (waitpid(pid, &status, 0) == -1)
                    fprintf(stderr, "Error: wait() failed.\n");
            }
        }
    }
    return 0;
}
