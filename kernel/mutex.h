struct file *mutexalloc(void);
void mutexclose(struct file *f);

struct mutex {
  struct sleeplock lock;
  struct spinlock splock;
  int owner_pid;
};
