
struct procinfo {
  int pid;
  char name[16];
  char state[10];
  char parent_name[16];
  int parent_pid;
};
