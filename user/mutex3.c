#include "kernel/types.h"
#include "user/user.h"

void test_1() {
  int mutex = mutex_create();
  int a[100];
  int ret = read(mutex, a, 1);
  printf(ret == -1 ? "Test 1: OK\n" : "Test 1: Failed\n");
}

void test_2() {
  int mutex = mutex_create();
  int a[100];
  int ret = write(mutex, a, 1);
  printf(ret == -1 ? "Test 2: OK\n" : "Test 2: Failed\n");
}
void test_3() {
  int mutex = mutex_create();
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Fork error\n");
  }
  if (pid == 0) {
    int ret = mutex_close(mutex);
    printf(ret == -1 ? "Test 3: OK\n" : "Test 3: Failed\n");
    exit(0);
  }
  else {
    mutex_lock(mutex);
    wait(0);
  }
}

void test_4() {
  int mutex = mutex_create();
  mutex_lock(mutex);
  int ret = mutex_close(mutex);
  printf(ret == 0 ? "Test 4: OK\n" : "Test 4: Failed\n");
}
void test_5() {
  int mutex = mutex_create();
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Fork error\n");
  }
  if (pid == 0) {
    mutex_lock(mutex);
    sleep(1);
    exit(0);
  }
  else {
    wait(0);
    printf("Test 5: OK\n");
  }
}
void test_6() {
  int mutex = mutex_create();
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "Fork error\n");
  }
  if (pid == 0) {
    mutex_lock(mutex);
    exit(0);
  }
  else {
    int ret = mutex_unlock(mutex);
    printf(ret == -1 ? "Test 6: OK\n" : "Test 6: Failed\n");
    wait(0);
  }
}
void test_7() {
  int mutex = mutex_create();
  int ret = mutex_close(mutex);
  printf(ret == 0 ? "Test 7: OK\n" : "Test 7: Failed\n"); 
}

void test_8() {
  int mutex = mutex_create();
  int ret = close(mutex);
  printf(ret == 0 ? "Test 8: OK\n" : "Test 8: Failed\n"); 
}

int main(int argc, char *argv[]) {
  test_1();
  test_2();
  test_3();
  test_4();
  test_5();
  test_6();
  test_7();
  test_8();
}
