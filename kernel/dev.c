#include "types.h"
#include "riscv.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "defs.h"
#include "file.h"

#define MNULL 0
#define MZERO 1
#define MURANDOM 2
#define MNULLSTAT 3
#define BUFSIZE 256

static struct {
  uint64 seed;
  struct spinlock urandom_lock;
  struct spinlock nullstat_lock;
  uint64 cnt_bytes;
} dev = {.seed = 1};

uint64 gen(uint64 seed) { return 214013 * seed + 2531011; }

int devread(int minor, int user_dst, uint64 dst, int n) {
  if (n < 0) {
    return -1;
  }
  if (minor == MNULL) {
    return 0;
  } else if (minor == MZERO) {
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);
    int cpy_cnt = 0;
    while (cpy_cnt < n) {
      int cnt = (n - cpy_cnt > BUFSIZE ? BUFSIZE : (n - cpy_cnt));
      if (either_copyout(user_dst, dst + cpy_cnt, buf, cnt) < 0) {
        return -1;
      }
      cpy_cnt += cnt;
    }
    return n;
  } else if (minor == MURANDOM) {
    acquire(&dev.urandom_lock);
    uint64 seed = dev.seed;
    for (int i = 0; i < n; ++i) {
      seed = gen(seed);
      char byte = seed >> 56;
      if (either_copyout(user_dst, dst + i, &byte, 1) < 0) {
        release(&dev.urandom_lock);
        return -1;
      }
    }
    dev.seed = seed;
    release(&dev.urandom_lock);
    return n;
  } else if (minor == MNULLSTAT) {
    if (n != sizeof(uint64)) {
      return -1;
    }
    acquire(&dev.nullstat_lock);
    uint64 cnt_bytes = dev.cnt_bytes;
    release(&dev.nullstat_lock);
    if (either_copyout(user_dst, dst, &cnt_bytes, n) < 0) {
      return -1;
    }
    return n;

  }
  return -1;
}

int devwrite(int minor, int user_src, uint64 src, int n) { 
  if (n < 0) {
    return -1;
  }
  if (minor == MNULL) {
    return n;
  } else if (minor == MZERO) {
    return -1;
  } else if (minor == MURANDOM) {
    if (n != sizeof(uint64)) {
      return -1;
    }
    uint64 seed;
    if (either_copyin(&seed, user_src, src, n) < 0) {
      return -1;
    }
    acquire(&dev.urandom_lock);
    dev.seed = seed ? seed : 1;
    release(&dev.urandom_lock);
    return n;
  } else if (minor == MNULLSTAT) {
    acquire(&dev.nullstat_lock);
    dev.cnt_bytes += n;
    release(&dev.nullstat_lock);
    return n;
  }
  return -1;
}

void devinit(void) {
  initlock(&dev.urandom_lock, "dev_urandom");
  initlock(&dev.nullstat_lock, "dev_nullstat");
  devsw[DEV].read = devread;
  devsw[DEV].write = devwrite;
}
