#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_BUF_SIZE 100
int main(int argc, char *argv[]) {
  char c;
  int i;
  int pos = -1;
  uint8 flag = 0x0;
  char buf[MAX_BUF_SIZE];
  for (i = 0;;) {
    if (i >= sizeof(buf)) {
      write(2, "Buffer overflow\n", 16);
      exit(1);
    }
    int cc = read(0, &c, 1);
    if (cc < 1) {
      write(2, "Read fail\n", 10);
      exit(1);
    }
    if (c == ' ') {
      if (flag == 0x0) {
        pos = i;
        flag = 0x1;
      } else {
        write(2, "Incorrect format: More than one space\n", 38);
        exit(1);
      }
    }
    buf[i++] = c;
    if (c == '\n' || c == '\r')
      break;
  }
  buf[i - 1] = '\0';
  if (pos == -1) {
    write(2, "Incorrect format: no space\n", 27);
    exit(1);
  }
  printf("|%s|\n", buf);
  buf[pos] = '\0';
  int num1 = atoi(buf);
  int num2 = atoi(buf + pos + 1);
  printf("%d\n", num1 + num2);
}
