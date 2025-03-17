#include "kernel/types.h"
#include "user/user.h"
#include "kernel/procinfo.h"
#define MAXX 8

int main(int argc, char* argv[]) {
    int sz = 1;
    int cnt = 0;
    struct procinfo *pi = (struct procinfo *)malloc(sizeof(struct procinfo) * sz);
    while (cnt < MAXX) {
      cnt++;
      int ret = ps_listinfo(pi, sz);
      if (ret == -2) {
        free(pi);
        sz *= 2;
        pi = (struct procinfo *)malloc(sizeof(struct procinfo) * sz);
      }
      else if (ret == -1) {
        free(pi);
        fprintf(2, "Error: cannot write to incorrect address\n");
        exit(1);
      }
      else {
        printf("pid | name | state | parent_pid | parent_name\n");
        for (int i = 0; i < ret; ++i) {
            printf("%d %s %s %d %s\n", pi[i].pid, pi[i].name, pi[i].state, pi[i].parent_pid, pi[i].parent_name);
        } 
        free(pi);
        break;
      }
    }
}
