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

extern int fdalloc(struct file *f);

uint64 sys_mutex_create(void) {
  struct file *f = mutexalloc();
  if (f == 0) {
    return -1;
  }
  int fd = fdalloc(f);
  if (fd == -1) {
    fileclose(f);
    return -1;
  }
  return fd;
  
}

uint64 sys_mutex_lock(void) {
  int fd;
  argint(0, &fd);
  acquire(&myproc()->lock);
  if (fd < 0 || fd >= NOFILE || myproc()->ofile[fd] == 0 || myproc()->ofile[fd]->type != FD_MUTEX) {
    release(&myproc()->lock);
    return -1;
  }
  struct file *f = myproc()->ofile[fd];
  struct mutex *mtx = f->mutex;
  int pid = myproc()->pid;
  release(&myproc()->lock);
  acquiresleep(&mtx->lock);
  acquire(&mtx->splock);
  mtx->owner_pid = pid;
  release(&mtx->splock);
  return 0;
}

uint64 sys_mutex_unlock(void) {
  int fd;
  argint(0, &fd);
  acquire(&myproc()->lock);
  if (fd < 0 || fd >= NOFILE || myproc()->ofile[fd] == 0 || myproc()->ofile[fd]->type != FD_MUTEX) {
    release(&myproc()->lock);
    return -1;
  }
  struct file *f = myproc()->ofile[fd];
  int pid = myproc()->pid;
  release(&myproc()->lock);
  struct mutex *mtx = f->mutex;
  acquire(&mtx->splock);
  if (mtx->owner_pid != pid) {
    release(&mtx->splock);
    return -1;
  }
  mtx->owner_pid = -1;
  release(&mtx->splock);
  releasesleep(&mtx->lock);
  return 0;
}



