#include "kernel/types.h"
#include "user/user.h"
#include "kernel/procinfo.h"
#define MAXX 8

int main(int argc, char* argv[]) {
    int sz = 1;
    int cnt = 0;
    struct procinfo *pi = (struct procinfo *)malloc(sizeof(struct procinfo) * sz);
    if (pi == 0) {
      fprintf(2, "Error: malloc failed\n");
      exit(1);
    }
    while (cnt < MAXX) {
      cnt++;
      int ret = ps_listinfo(pi, sz);
      if (ret == -2) {
        free(pi);
        sz *= 2;
        pi = (struct procinfo *)malloc(sizeof(struct procinfo) * sz);
        if (pi == 0) {
          fprintf(2, "Error: malloc failed\n");
          exit(1);
        }
      }
      else if (ret == -1) {
        free(pi);
        fprintf(2, "Error: cannot write to incorrect address\n");
        exit(1);
      }
      else {
        printf("pid | name | state | parent_pid | parent_name\n");
        for (int i = 0; i < ret; ++i) {
          printf("%d %s ", pi[i].pid, pi[i].name);
          if (pi[i].state == UNUSED_STATE) {
            printf("%s ", "unused");
          } else if (pi[i].state == USED_STATE) {
            printf("%s ", "used");
          } else if (pi[i].state == SLEEPING_STATE) {
            printf("%s ", "sleeping");
          } else if (pi[i].state == RUNNABLE_STATE) {
            printf("%s ", "runnable");
          } else if (pi[i].state == RUNNING_STATE) {
            printf("%s ", "running");
          } else if (pi[i].state == ZOMBIE_STATE ){
            printf("%s ", "zombie");
          }
          printf("%d %s\n", pi[i].parent_pid, pi[i].parent_name);
            
        } 
        free(pi);
        break;
      }
    }
}
