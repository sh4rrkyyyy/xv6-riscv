enum State { UNUSED_STATE, USED_STATE, SLEEPING_STATE, RUNNABLE_STATE, RUNNING_STATE, ZOMBIE_STATE };

struct procinfo {
  int pid;
  char name[16];
  enum State state;
  char parent_name[16];
  int parent_pid;
};
