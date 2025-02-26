
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork error\n");
    }
    if (pid > 0) {
        int ppid = getpid();
        printf("Parent's pid: %d, child's pid: %d\n", ppid, pid);
        int status, cpid;
        kill(pid);
        cpid = wait(&status);
        printf("Identifier of process %d is %d, status is %d\n", pid, cpid, status);
        exit(0);
    }
    else {
        while(1){
            sleep(100);
        }
    }
}
