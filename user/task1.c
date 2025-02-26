#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
   int pid = fork();
   if (pid < 0) {
       fprintf(2, "Error: fork error\n");
   }
   else if (pid > 0) {
       int status;
       wait(&status);
       exit(0);
   }
   else {
       sleep(100);
       exit(1);
   }
}
