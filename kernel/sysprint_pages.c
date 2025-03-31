#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define fR(x) ((x & PTE_R) ? 'R' : '_')
#define fW(x) ((x & PTE_W) ? 'W' : '_')
#define fX(x) ((x & PTE_X) ? 'X' : '_')
#define fU(x) ((x & PTE_U) ? 'U' : '_')
#define fG(x) ((x & PTE_G) ? 'G' : '_')
#define fA(x) ((x & PTE_A) ? 'A' : '_')
#define fD(x) ((x & PTE_D) ? 'D' : '_')
#define bool int
#define false 0
#define true 1

extern struct proc proc[NPROC];

uint64 sys_print_pages(void) {
  uint64 ptr;
  int sz;
  int mask;

  argaddr(0, &ptr);
  argint(1, &sz);
  argint(2, &mask);
  if (sz < 0) {
    return -1;
  }
  if (mask < 0 || mask > 4) {
    return -1;
  }
  bool first_cond = false;
  if (ptr == 0 || sz == 0) {
    first_cond = true;
  }
  pagetable_t pt0;
  int psz;
  acquire(&myproc()->lock);
  pt0 = myproc()->pagetable;
  psz = myproc()->sz;
  release(&myproc()->lock);
  if (ptr >= psz && first_cond == false) {
    return -1;
  }
  printf("PAGETABLE %p\n", pt0);
  for (int i = 0; i < 512; ++i) {
    bool f0 = false;
    if (!(pt0[i] & PTE_V)) {
      continue;
    }
    pagetable_t pt1 = (pagetable_t)PTE2PA(pt0[i]);
    char flags0[8] = {fR(pt0[i]), fW(pt0[i]), fX(pt0[i]), fU(pt0[i]),
                      fG(pt0[i]), fA(pt0[i]), fD(pt0[i]), '\0'};
    for (int j = 0; j < 512; ++j) {
      bool f1 = false;
      if (!(pt1[j] & PTE_V)) {
        continue;
      }
      pagetable_t pt2 = (pagetable_t)PTE2PA(pt1[j]);
      
      char flags1[8] = {fR(pt1[j]), fW(pt1[j]), fX(pt1[j]), fU(pt1[j]),
                        fG(pt1[j]), fA(pt1[j]), fD(pt1[j]), '\0'};
      for (int k = 0; k < 512; ++k) {
        
        uint64 va = 0;
        va = (((uint64)i << 30) | ((uint64)j << 21) | ((uint64)k << 12));
      
        if (!first_cond && (va >= ptr + sz || va + PGSIZE <= ptr)) {
          continue;
        }
        bool f2 = false;
        if (!(pt2[k] & PTE_V)) {
          continue;
        }
        pagetable_t pt3 = (pagetable_t)PTE2PA(pt2[k]);
    
        char flags2[8] = {fR(pt2[k]), fW(pt2[k]), fX(pt2[k]), fU(pt2[k]),
                          fG(pt2[k]), fA(pt2[k]), fD(pt2[k]), '\0'};
        if (f0 == false) {
          f0 = true;
          if (((mask == 2 || mask == 3) && (fA(pt0[i]) == 'A')) ||
              ((mask == 2 || mask == 1) && (fD(pt0[i]) == 'D')) ||
              (mask == 0)) {
            if (i < 10) {
              printf("0x00%d -> %p %s\n", i, pt1, flags0);
            } else if (i < 100) {
              printf("0x0%d -> %p %s\n", i, pt1, flags0);
            } else {
              printf("0x%d -> %p %s\n", i, pt1, flags0);
            }
          }
        }
        if (f1 == false) {
          f1 = true;
          if (((mask == 2 || mask == 3) && (fA(pt1[j]) == 'A')) ||
              ((mask == 2 || mask == 1) && (fD(pt1[j]) == 'D')) ||
              (mask == 0)) {
            if (j < 10) {
              printf(".........0x00%d -> %p %s\n", j, pt2, flags1);
            } else if (j < 100) {
              printf(".........0x0%d -> %p %s\n", j, pt2, flags1);
            } else {
              printf(".........0x%d -> %p %s\n", j, pt2, flags1);
            }
          }
        }
        if (f2 == false) {
          f2 = true;
          if (((mask == 2 || mask == 3) && (fA(pt2[k]) == 'A')) ||
              ((mask == 2 || mask == 1) && (fD(pt2[k]) == 'D')) ||
              (mask == 0)) {
            if (k < 10) {
              printf("..................0x00%d -> %p %s\n", k, pt3, flags2);
            } else if (k < 100) {
              printf("..................0x0%d -> %p %s\n", k, pt3, flags2);
            } else {
              printf("..................0x%d -> %p %s\n", k, pt3, flags2);
            }
          }
        }
      }
    }
  }
  return 0;
}
