#include "kernel/types.h"
#include "user/user.h"
int cnt = 10;
int main(int argc, char *argv[]) {
  printf("INIT PRINT\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, 0);
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

  
  free(b);
  
  printf("PRINT AFTER FREE\n");
  printf("=============================\n");
  printf("\n");
  print_pages(0, 0, 0);
  printf("\n");
  
  
  printf("CLEAR FLAGS\n");
  printf("=============================\n");
  printf("\n");
  clear_flags(0, 0, 0);
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
  clear_flags(0, 0, 0);
  print_pages(0, 0, 0);
  printf("\n");
  
  
  int x = 10;
  a[10] = x; 
  printf("PRINT AFTER WRITE\n");
  printf("=============================\n");
  printf("\n");
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
}
