#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Error: fork error\n");
        exit(1);
    }
    if (pid > 0) {
        int ppid = getpid();
        printf("Parent's pid: %d, child's pid: %d\n", ppid, pid);
        int status, cpid;
        cpid = wait(&status);
        printf("Identifier of completed process is %d, status is %d\n", cpid, status);
        exit(0);
    }
    else {
        sleep(100);
        exit(1);
    }
}