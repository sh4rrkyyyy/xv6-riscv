struct file *mutexalloc(void);
void mutexclose(struct file *f);

struct mutex {
  struct sleeplock lock;
  int owner_pid;
};
