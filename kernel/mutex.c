
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "mutex.h"
#include "proc.h"

extern struct file ftable[NFILE];
extern int printf(char *fmt, ...);

struct file *mutexalloc(void) {
  struct file *f;
  struct mutex *mtx;
  f = filealloc();
  if (f == 0) {
    printf("filealloc cannot allocate file (in mutexalloc)\n");
    return 0;
  }
  mtx = (struct mutex *)kalloc();
  if (mtx == 0) {
    printf("kalloc cannot allocate memory for mutex (in mutexalloc)\n");
    fileclose(f);
    return 0;
  }
  initsleeplock(&mtx->lock, "mutex");
  initlock(&mtx->splock, "mutex");
  mtx->owner_pid = 0;
  f->mutex = mtx;
  f->type = FD_MUTEX;
  f->ref = 1;
  return f;
}


void mutexclose(struct file *f) {
  if (f->type == FD_MUTEX) {
    struct mutex *mtx = f->mutex;
    releasesleep(&mtx->lock);
    kfree(mtx);
    printf("memory for mutex freed (in mutexclose)\n");
  }
  f->type = FD_NONE;
}