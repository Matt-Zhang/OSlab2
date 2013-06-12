#include "msg.h"
#include "thread.h"
#include "x86.h"
#include "assert.h"

extern PCB* pcb_current;
static ListHead listhead;
static Semaphore message_buffer, mailbox_semq[MAX_THREAD];



void init_msg()
{
	new_sem(message_buffer, 1);
	int i;
	for (i = 0; i < MAX_THREAD; i++) {
		new_sem(process_mailbox[i], 0);
	}
	list_init(&listhead);
}
/* We assume we will not sti() in the whole interrupt process which sound reasonable in nanos */
void send(pid_t dst, Message *m)
{
	m->dest = dest;
	if(pcb_current->state == INTERRUPTED) {
		m->src = MSG_HWINTR;
		add_mailbox(m);	
		/* equal to V */
		mailbox_semq[m->dest].count++;	
		if(mailbox_semq[m->dest].count <= 0) {
			assert(!list_empty(&sem->queue));
			PCB *pcb = list_entry(sem->queue.next, PCB, semq);
			list_del(sem->queue.next);
			wakeup(pcb);
		}
	}
	else{
		m->src = pcb_current->pid;
		P(message_buffer);
		add_mailbox(m);
		V(message_buffer);
		V(mailbox_semq[m->dest]);
	}
	
}

void receive(pid_t dst, Message *m)
{
	if(dst != ANY) {
		P(mailbox_semq[dst];
		P(message_buffer);
		remove,copy;
		V(message_buffer);
	}
	else {
		Message *message	
	}
}

