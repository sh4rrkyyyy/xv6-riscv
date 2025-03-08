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
    
    for (p = proc; p < &proc[NPROC]; ++p) {
        acquire(&p->lock);
        if (p->state != UNUSED) {
            cnt++;
        }
        release(&p->lock);
    }
    if (!ptr) {
        return cnt;
    }
    if (cnt > lim) {
        return -2;
    }
    cnt = 0;
    for (p = proc; p < &proc[NPROC]; ++p) {
        acquire(&p->lock);
        if (p->state != UNUSED) {
            pi.pid = p->pid;
            strncpy(pi.name, p->name, 16);
            
            if (p->state == USED) {
                strncpy(pi.state, "USED", 16);
            }
            else if (p->state == SLEEPING) {
                strncpy(pi.state, "SLEEPING", 16);
            }
            else if (p->state == RUNNABLE) {
                strncpy(pi.state, "RUNNABLE", 16);
            }
            else if (p->state == RUNNING) {
                strncpy(pi.state, "RUNNING", 16);
            }
            else {
                strncpy(pi.state, "ZOMBIE", 16);
            }
            
            acquire(&wait_lock);
            if (!p->parent) {
                strncpy(pi.parent_name, "none", 16);
                pi.parent_pid = -1;
            }
            else {
                strncpy(pi.parent_name, p->parent->name, 16); 
                pi.parent_pid = p->parent->pid;
            }
            release(&wait_lock);
            if (copyout(myproc()->pagetable, ptr, (char *)&(pi), sizeof(pi)) < 0) {
                release(&p->lock);
                return -1;
            }
            ptr += sizeof(pi);
            cnt++;
        }
        release(&p->lock);
    }
    return cnt;
}