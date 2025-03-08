#include "kernel/types.h"
#include "user/user.h"
#include "kernel/procinfo.h"
int main(int argc, char* argv[]) {
    struct procinfo pi[64];
    int cnt = ps_listinfo(pi, 64);
    printf("pid | name | state | parent_pid | parent_name\n");
    for (int i = 0; i < cnt; ++i) {
        printf("%d %s %s %d %s\n", pi[i].pid, pi[i].name, pi[i].state, pi[i].parent_pid, pi[i].parent_name);
    }
}
