
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

static struct {
  uint64 seed;
  struct spinlock lock;
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
    for (int i = 0; i < n; ++i) {
      char smb = 0;
      if (either_copyout(user_dst, dst + i, &smb, 1) < 0) {
        return -1;
      }
    }
    return n;
  } else if (minor == MURANDOM) {
    acquire(&dev.lock);
    uint64 seed = dev.seed;
    for (int i = 0; i < n; ++i) {
      seed = gen(seed);
      char byte = seed >> 56;
      if (either_copyout(user_dst, dst + i, &byte, 1) < 0) {
        release(&dev.lock);
        return -1;
      }
    }
    dev.seed = seed;
    release(&dev.lock);
    return n;
  } else if (minor == MNULLSTAT) {
    if (n != sizeof(uint64)) {
      return -1;
    }
    acquire(&dev.lock);
    uint64 cnt_bytes = dev.cnt_bytes;
    release(&dev.lock);
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
    acquire(&dev.lock);
    dev.seed = seed ? seed : 1;
    release(&dev.lock);
    return n;
  } else if (minor == MNULLSTAT) {
    acquire(&dev.lock);
    dev.cnt_bytes += n;
    release(&dev.lock);
    return n;
  }
  return -1;
}

void devinit(void) {
  initlock(&dev.lock, "dev");
  devsw[DEV].read = devread;
  devsw[DEV].write = devwrite;
}
