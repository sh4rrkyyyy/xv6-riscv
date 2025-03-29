#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "procinfo.h"

extern struct proc proc[NPROC];
extern struct spinlock wait_lock;

uint64 sys_ps_listinfo(void) {
    
    struct proc *p;
    struct procinfo pi;
    int cnt = 0;
    uint64 ptr;
    int lim;
    
    argaddr(0, &ptr);
    argint(1, &lim);
    if (!ptr) {
      for (p = proc; p < &proc[NPROC]; ++p) {
          acquire(&p->lock);
          if (p->state != UNUSED && p->state != USED) {
              cnt++;
          }
          release(&p->lock);
      }
      return cnt;
    }
    for (p = proc; p < &proc[NPROC]; ++p) {
        acquire(&p->lock);
        if (p->state != UNUSED && p->state != USED) {
            if (++cnt > lim) {
              release(&p->lock);
              return -2;
            }
            pi.pid = p->pid;
            safestrcpy(pi.name, p->name, sizeof(pi.name));
            if (p->state == SLEEPING) {
              pi.state = SLEEPING_STATE;
            }
            else if (p->state == RUNNABLE) {
              pi.state = RUNNABLE_STATE;
            }
            else if (p->state == RUNNING) {
              pi.state = RUNNING_STATE;
            }
            else {
              pi.state = ZOMBIE_STATE;
            }
            
            acquire(&wait_lock);
            if (!p->parent) {
                safestrcpy(pi.parent_name, "none", sizeof(pi.parent_name));
                pi.parent_pid = -1;
            }
            else {
                struct proc *parent = p->parent;
                acquire(&parent->lock);
                safestrcpy(pi.parent_name, parent->name, sizeof(pi.parent_name)); 
                pi.parent_pid = parent->pid;
                release(&parent->lock);
            }
            release(&wait_lock);
            if (copyout(myproc()->pagetable, ptr, (char *)&(pi), sizeof(pi)) < 0) {
                release(&p->lock);
                return -1;
            }
            ptr += sizeof(pi);
        }
        release(&p->lock);
    }
    return cnt;
}