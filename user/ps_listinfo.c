#include "kernel/types.h"
#include "user/user.h"
#include "kernel/procinfo.h"
int main(int argc, char *argv[]) {
    struct procinfo a[32];
    printf(ps_listinfo(a, 32) == 3 ? "Test 1 OK\n" : "Test 1 failed\n");
    printf(ps_listinfo((struct procinfo *)0x42, 10) == -1 ? "Test 2 OK\n" : "Test 2 failed\n");
    printf(ps_listinfo(a, 2) == -2 ? "Test 3 OK\n" : "Test 3 failed\n");
    printf(ps_listinfo(0, 32) == 3 ? "Test 4 OK\n" : "Test 4 failed\n");
    struct procinfo b[2];
    printf(ps_listinfo(b, 2) == -2 ? "Test 5 OK\n" : "Test 5 failed\n");
    

}
