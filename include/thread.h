#ifndef __THREAD_H__
#define __THREAD_H__
#include "x86.h"
#include "common.h"

extern PCB* pcb_current;

PCB *schedule(void);
void switch_event(struct TrapFrame* tf);
struct PCB *create_kthread(void *entry);


static inline void 
lock(void)
{
	cli();
	pcb_current -> count_lock++;
}

static inline void 
unlock(void)
{
	assert(pcb_current -> count_lock >= 0);
	pcb_current -> count_lock--;
	if(pcb_current -> count_lock == 0)
		sti();
}

void sleep(void);
void wakeup(struct PCB *pcb);
void new_sem(Semaphore *sem, int value); 
void P(Semaphore *sem);
void V(Semaphore *sem);

#endif
