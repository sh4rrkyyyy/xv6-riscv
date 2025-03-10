#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 128

void print(int fd, char *ptr, int len) {
  while (len > 0) {
    int ret = write(fd, ptr, len);
    if (ret < 0) {
      perror("Error: write error");
      exit(1);
    }
    ptr += ret;
    len -= ret;
  }
}
int main(int argc, char *argv[]) {
  int pfd[2];
  if (pipe(pfd) < 0) {
    perror("Error: cannot create pipe");
    exit(1);
  }
  int pid = fork();
  if (pid < 0) {
    perror("Error: fork error");
    exit(1);
  } else if (pid == 0) {
    if (close(pfd[1]) < 0) {
      perror("Error: cannot close write-end in child");
      exit(1);
    }
    char buf[BUF_SIZE];
    int len;
    while ((len = read(pfd[0], buf, BUF_SIZE)) > 0) {
      char *ptr = buf;
      print(1, ptr, len);
    }
    if (len < 0) {
      perror("Error: read error");
      exit(1);
    }
    if(close(pfd[0]) < 0) {
      perror("Error: cannot close read-end in child");
      exit(1);
    }
    exit(0);
  } else {
    if (close(pfd[0]) < 0) {
      perror("Error: cannot close read-end in parent");
    }
    for (int i = 1; i < argc; ++i) {
      char *ptr = argv[i];
      int len = strlen(argv[i]);
      print(pfd[1], ptr, len);
      if (write(pfd[1], "\n", 1) != 1) {
        perror("Error: write new line failed");
        exit(1);
      }
    }
    int ret = close(pfd[1]);
    if (ret < 0) {
      perror("Error: cannot close write-end in parent");
      exit(1);
    }
    if (wait(0) < 0) {
      perror("Error: wait failed");
      exit(1);
    }
    exit(0);
  }
}
