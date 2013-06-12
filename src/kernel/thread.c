#include "x86.h"
#include "thread.h"
#include "list.h"
#include "debug.h"

static PCB pcb[MAX_THREAD];
PCB *pcb_current = NULL;
static int count_pcb = 0;
/* Only the one running on cpu is RUNNING when it is not in Interrupt, 
 * other runable process are INTERRUPTED , the running process in interrupt is INTERRUPTED */
void
schedule()
{
	if(!pcb_current){
		pcb_current = pcb;
	}
	else{
		ListHead *temp = NULL;NOINTR;
		PCB *pcb_next = NULL;NOINTR;
		list_foreach(temp, &(pcb_current->link)){
			pcb_next = list_entry(temp, PCB, link);	NOINTR;	
			if (pcb_next->state != STOPPED) {
				pcb_current = pcb_next;
				return;
			}
		}
		/* If no other PCB can be used, we have to return the original PCB */
		assert(pcb_current->state != STOPPED);
		
	}

	
}
/* In this lab, you can't create_kthread with IF=1, otherwise no more thread would be create */
PCB*
create_kthread(void *entry)
{
	assert(count_pcb < MAX_THREAD);	
	TrapFrame *tf = (TrapFrame*) (pcb[count_pcb].kstack +STACK_SIZE - 1 - sizeof(TrapFrame)); 
	pcb[count_pcb].tf[0] = tf; 
	pcb[count_pcb].tf[0]->eip = (uint32_t) entry;
	pcb[count_pcb].tf[0]->ds = KSEL(SEG_KDATA);
	pcb[count_pcb].tf[0]->es = KSEL(SEG_KDATA);
	pcb[count_pcb].tf[0]->cs = KSEL(SEG_KCODE);
	pcb[count_pcb].tf[0]->eflags = FL_IF|0x0000002; //only if and a bit forced to be 1
	pcb[count_pcb].state = STOPPED;
	pcb[count_pcb].pid = count_pcb + 1;
	pcb[count_pcb].count_tf = 1;                 /* Need to return the original tf of the first run process */
	pcb[count_pcb].count_lock = 0;
	list_init(&(pcb[count_pcb].link));
	list_init(&(pcb[count_pcb].semq));
//	if(pcb_current != NULL)
//		list_add_before(&(pcb_current->link), &(pcb[count_pcb].link));
//	else if(count_pcb != 0 && (pcb_current == NULL))
//	else
		list_add_before(&(pcb[0].link), &(pcb[count_pcb].link));
//	else
//		list_add_before(&(pcb[0].link), &(pcb[0].link));
	count_pcb++;
	return &(pcb[count_pcb - 1]);
}


void 
sleep()
{
	pcb_current->state = STOPPED;NOINTR;
//	printk("SLEEP: %d\n", pcb_current->pid);
	unlock();INTR;
	/* if no pcb can be runned, an assertation will be raised in schedule();*/ 
	interrupt();
}

void 
wakeup(PCB *pcb)
{
	pcb->state = INTERRUPTED;NOINTR;
//	if(pcb_current == NULL)
//		sti();
//	printk("WAKEUP %d\n", pcb->pid);NOINTR;
}
	

void
new_sem(Semaphore *sem, int value) {
    sem->count = value;
    list_init(&sem->queue);
}

void
P(Semaphore *sem) {
	lock();
    sem->count --;NOINTR;
    if (sem->count < 0) {
        list_add_before(&sem->queue, &pcb_current->semq);NOINTR;
        sleep();
		return;
    }
	unlock();
}

void V(Semaphore *sem) {
	lock();NOINTR;
    sem->count ++;NOINTR;
    if (sem->count <= 0) {
        assert(!list_empty(&sem->queue));
        PCB *pcb = list_entry(sem->queue.next, PCB, semq);
        list_del(sem->queue.next);
        wakeup(pcb); 
    }
	unlock();INTR;
}
