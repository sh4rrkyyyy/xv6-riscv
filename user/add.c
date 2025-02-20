#include "kernel/types.h"
#include "user/user.h"
#define MAX_BUF_SIZE 100
int main(int argc, char *argv[]) {
  char c;
  int i;
  int pos = -1;
  char buf[MAX_BUF_SIZE];
  for (i = 0;;) {
    if (i >= sizeof(buf)) {
      write(2, "Buffer overflow\n", 16);
      exit(1);
    }
    int cc = read(0, &c, 1);
    if (c == ' ')
      pos = i;
    if (cc != 1) {
      write(2, "Read fail\n", 10);
      exit(1);
    }
    buf[i++] = c;
    if (c == '\n' || c == '\r')
      break;
  }
  buf[i - 1] = '\0';
  printf("|%s|\n", buf);
  if (pos == -1) {
    write(2, "Incorrect format: no space\n", 27);
    exit(1);
  }
  buf[pos] = '\0';
  int num1 = atoi(buf);
  int num2 = atoi(buf + pos + 1);
  add(num1, num2);
  exit(0);
}
