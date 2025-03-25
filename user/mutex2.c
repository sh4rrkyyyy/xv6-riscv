#include "kernel/types.h"
#include "user/user.h"

void proc_print(int argc, char *argv[], int mutex) {
  for (int i = 1; i < argc; ++i) {
    for (int j = 0; j < strlen(argv[i]); ++j) {
      mutex_lock(mutex);
      printf("pid: %d, arg: %d, char: %s\n", getpid(), i, (char[]){argv[i][j], 0});
      mutex_unlock(mutex);
    }
  }
}
void proc_print_nosync(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    for (int j = 0; j < strlen(argv[i]); ++j) {
      printf("pid: %d, arg: %d, char: %s\n", getpid(), i, (char[]){argv[i][j], 0});
    }
  }
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "Too few arguments\n");
    exit(1);
  }
  printf("PRINT WITHOUT MUTEX\n");
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Fork error\n");
  }
  if (pid == 0) {
    proc_print_nosync(argc, argv);
    exit(0);
  }
  else {
    proc_print_nosync(argc, argv);
    wait(0);
  }
  printf("\n");
  sleep(10);
  printf("PRINT WITH MUTEX\n"); 
  int mutex = mutex_create();
  pid = fork();
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
  mutex_close(mutex);
  
}
