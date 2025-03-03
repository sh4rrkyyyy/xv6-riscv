#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 128
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
  } else if (pid == 0) {
    close(pfd[1]);
    char buf[BUF_SIZE];
    int len;
    char *ptr = buf;
    while ((len = read(pfd[0], &buf, BUF_SIZE)) > 0) {
      while (len > 0) {
        int ret = write(1, ptr, len);
        if (ret < 0) {
          fprintf(stderr, "Error: write error\n");
          exit(1);
        }
        ptr += ret;
        len -= ret;
      }
    }
    if (len < 0) {
      fprintf(stderr, "Error: read error\n");
      exit(1);
    }
    close(pfd[0]);
    exit(0);
  } else {
    close(pfd[0]);
    for (int i = 1; i < argc; ++i) {
      char *ptr = argv[i];
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
