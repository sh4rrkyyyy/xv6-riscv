#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static char digits[] = "0123456789abcdef";

void print_byte(unsigned char byte) {
  char s[3];
  s[0] = digits[byte >> 4];
  s[1] = digits[byte & 0xF];
  s[2] = 0;
  printf("%s ", s);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "hexprint: more or less than 2 arguments\n");
    exit(1);
  }
  int fd = open(argv[1], 0);
  if (fd < 0) {
    fprintf(2, "hexprint: open failed\n");
    exit(1);
  }
  int bytes = atoi(argv[2]);
  unsigned char *buf = (unsigned char *)malloc(bytes);
  if (buf == 0) {
    fprintf(2, "hexprint: malloc failed\n");
    close(fd);
    exit(1);
  }
  int n = read(fd, buf, bytes);
  for (int i = 0; i < n; i++)
    print_byte(buf[i]);
  
  printf("\n");
  free(buf);
  close(fd);
  exit(0);
}
