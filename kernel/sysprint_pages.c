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
#define fPrint 0
#define fClear 1

extern struct proc proc[NPROC];
void print_pages(unsigned long pt_idx, pagetable_t pt, int mask, int idx,
                 int level) {
  char flags[8] = {fR(pt_idx), fW(pt_idx), fX(pt_idx), fU(pt_idx),
                   fG(pt_idx), fA(pt_idx), fD(pt_idx), '\0'};

  if (((mask == 2 || mask == 3) && (fA(pt_idx) == 'A')) ||
      ((mask == 2 || mask == 1) && (fD(pt_idx) == 'D')) || (mask == 0)) {
    if (level == 2) {
      printf(".........");
    }
    if (level == 3) {
      printf("..................");
    }
    if (idx < 10) {
      printf("0x00%d -> %p %s\n", idx, pt, flags);
    } else if (idx < 100) {
      printf("0x0%d -> %p %s\n", idx, pt, flags);
    } else {
      printf("0x%d -> %p %s\n", idx, pt, flags);
    }
  }
}

int walking(uint64 ptr, int sz, int mask, bool flag) {
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
  if (flag == fPrint) {
    printf("PAGETABLE %p\n", pt0);
  }
  for (int i = 0; i < 512; ++i) {
    bool f0 = false;
    if (!(pt0[i] & PTE_V)) {
      continue;
    }
    pagetable_t pt1 = (pagetable_t)PTE2PA(pt0[i]);
    for (int j = 0; j < 512; ++j) {
      bool f1 = false;
      if (!(pt1[j] & PTE_V)) {
        continue;
      }
      pagetable_t pt2 = (pagetable_t)PTE2PA(pt1[j]);

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
        if (flag == fPrint) {
          if (f0 == false) {
            f0 = true;
            print_pages(pt0[i], pt1, mask, i, 1);
          }
          if (f1 == false) {
            f1 = true;
            print_pages(pt1[j], pt2, mask, j, 2);
          }
          if (f2 == false) {
            f2 = true;
            print_pages(pt2[k], pt3, mask, k, 3);
          }
        } else {
          pt0[i] &= ~(unsigned long)PTE_A;
          pt1[j] &= ~(unsigned long)PTE_A;
          pt2[k] &= ~(unsigned long)PTE_A;
          pt0[i] &= ~(unsigned long)PTE_D;
          pt1[j] &= ~(unsigned long)PTE_D;
          pt2[k] &= ~(unsigned long)PTE_D;
        }
      }
    }
  }
  return 0;
}

uint64 sys_print_pages(void) {
  uint64 ptr;
  int sz;
  int mask;

  argaddr(0, &ptr);
  argint(1, &sz);
  argint(2, &mask);
  return walking(ptr, sz, mask, fPrint);
}

uint64 sys_clear_flags(void) {
  uint64 ptr;
  int sz;
  int mask;

  argaddr(0, &ptr);
  argint(1, &sz);
  argint(2, &mask);
  return walking(ptr, sz, mask, fClear);
}
