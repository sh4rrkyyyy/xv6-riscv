#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int pfd[2];
    if (pipe(pfd) < 0) {
        fprintf(2, "Error: cannot create pipe\n");
        exit(1);
    }
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork error\n");
        exit(1);
    }
    else if (pid == 0) {
      if (close(pfd[1]) < 0) {
        fprintf(2, "Error: cannot close write-end in child\n");
        exit(1);
      }
      if (close(0) < 0) {
        fprintf(2, "Error: cannot close stdin in child\n");
        exit(1); 
      }
      if (dup(pfd[0]) < 0) {
        fprintf(2, "Error: dup failed\n");
        exit(1);  
      }
      if (close(pfd[0]) < 0) {
        fprintf(2, "Error: cannot close read-end in child\n");
      }
        char *argv[] = {"/wc", 0};
        exec("/wc", argv);
        fprintf(2, "Error: exec failed\n");
        exit(1);
    }
    else {
        if (close(pfd[0])) {
          fprintf(2, "Error: cannot close read-end in parent\n");
        }
        for (int i = 1; i < argc; ++i) {
            char* ptr = argv[i];
            int len = strlen(argv[i]);
            while (len > 0) {
                int ret = write(pfd[1], ptr, len);
                if (ret < 0) {
                    fprintf(2, "Error: write error\n");
                    exit(1);
                }
                ptr += ret;
                len -= ret;
            }
            if (write(pfd[1], "\n", 1) != 1) {
              fprintf(2, "Error: write new line failed\n");
              exit(1);
            }
        }
        int ret = close(pfd[1]);
        if (ret < 0) {
            fprintf(2, "Error: cannot close write-end in parent\n");
            exit(1);
        }
        if (wait(0) < 0) {
          fprintf(2, "Error: wait failed\n");
          exit(1);
        }
        exit(0);
    }
}
