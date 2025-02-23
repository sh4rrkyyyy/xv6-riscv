#include "kernel/types.h"
#include "user/user.h"
#define MAX_BUF_SIZE 100
int main(int argc, char *argv[]) {
  printf(add(100, -100) == 0 ? "Test 1 OK\n" : "Test 1 failed\n"); 
  printf(add(0, -1) == -1 ? "Test 2 OK\n" : "Test 2 failed\n");
  printf(add(-100, -100) == -200 ? "Test 3 OK\n" : "Test 3 failed\n");
  printf(add(43210, 12345) == 55555 ? "Test 4 OK\n" : "Test 4 failed\n");
  printf(add(132, -100111) == -99979 ? "Test 5 OK\n" : "Test 5 failed\n");
  exit(0);
}
