#include "kernel/types.h"
#include "user/user.h"
#define maskA 1 << 1
#define maskD 1
int cnt = 10;
int cnt2 = 15;
int main(int argc, char *argv[]) {
  printf("INIT PRINT\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  
  char *b = (char *)malloc(10000 * sizeof(char));
  if (b == 0) {
    printf("Error: malloc failed\n");
    exit(1);
  }
  printf("PRINT AFTER ALLOCATION IN HEAP\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");

  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  int c = b[1];
  (void)c;
  printf("PRINT AFTER READ (HEAP)\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  cnt2 = b[1];
  printf("PRINT AFTER READ IN GLOBAL (HEAP)\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  int y = 10;
  b[10] = y; 
  printf("PRINT AFTER WRITE (HEAP)\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
 
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  
  b[10] = cnt2; 
  printf("PRINT AFTER WRITE FROM GLOBAL (HEAP)\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
 
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  free(b);
  
  printf("PRINT AFTER FREE\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
 
 
  char a[2048];
  int var = a[1];
  (void)var;
  printf("PRINT AFTER READ\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");

  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  cnt = a[1];
  printf("PRINT AFTER READ IN GLOBAL\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  int x = 10;
  a[10] = x; 
  printf("PRINT AFTER WRITE\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
 
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  
  a[10] = cnt; 
  printf("PRINT AFTER WRITE FROM GLOBAL\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
 
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("PRINT ONLY PAGES FOR BUFFER\n");
  printf("=============================\n");
  printf("\n");
  print_pages(a, 2048, 0);
  printf("\n");
  
  
  printf("PRINT PAGES WITH FLAG D\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 1);
  printf("\n");
  
  printf("PRINT PAGES WITH FLAG A\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 2);
  printf("\n");
  
  
  printf("PRINT PAGES WITH FLAGS A OR D\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 3);
  printf("\n");
  
  a[10] = cnt;
  x = a[10];
  printf("PRINT PAGES\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR WITH ZERO MASK\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, 0);
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR ONLY FLAGS A\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA);
  print_pages(0, 0, 0);
  printf("\n");
  
  a[10] = cnt;
  x = a[10];
  
  printf("PRINT PAGES\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR ONLY FLAGS D\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  a[10] = cnt;
  x = a[10];
  
  printf("PRINT PAGES\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  printf("CLEAR FLAGS A and D\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, maskA | maskD);
  print_pages(0, 0, 0);
  printf("\n");
  
  
}
