#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        fprintf(stderr, "Error: cannot create pipe\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: fork error\n");
        exit(1);
    }
    else if (pid == 0) {
        close(pfd[1]);
        close(0);

        dup(pfd[0]);
        close(pfd[0]);
        char *argv[] = {"wc", NULL};
        execv("/usr/bin/wc", argv);
        fprintf(stderr, "Error: exec failed\n");
        exit(1);
    }
    else {
        close(pfd[0]);
        for (int i = 1; i < argc; ++i) {
            char* ptr = argv[i];
            int len = strlen(argv[i]);
            while (len > 0) {
                int ret = write(pfd[1], ptr, len);
                if (ret < 0) {
                    fprintf(stderr, "Error: write error\n");
                    exit(1);
                }
                ptr += ret;
                len -= ret;
            }
            write(pfd[1], argv[i], strlen(argv[i]));

            write(pfd[1], "\n", 1);
        }
        int ret = close(pfd[1]);
        if (ret < 0) {
            fprintf(stderr, "Error: cannot close channel\n");
            exit(1);
        }
        wait(0);
        exit(0);
    }
}
