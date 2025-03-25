#include "kernel/types.h"
#include "user/user.h"

void proc_print(int argc, char *argv[], int mutex) {
  for (int i = 0; i < argc; ++i) {
    for (int j = 0; j < strlen(argv[i]); ++j) {
      mutex_lock(mutex);
      printf("pid: %d, arg: %d, char: %s\n", getpid(), i, (char[]){argv[i][j], 0});
      mutex_unlock(mutex);
    }
  }
}
int main(int argc, char *argv[]) {
  int mutex = mutex_create();
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Fork error\n");
  }
  if (pid == 0) {
    proc_print(argc, argv, mutex);
    exit(0);
  }
  else {
    proc_print(argc, argv, mutex);
    wait(0);
  }
  
}
