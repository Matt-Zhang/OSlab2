#include "msg.h"
#include "thread.h"
#include "x86.h"
#include "assert.h"
#include "debug.h"
#include "string.h"

extern PCB* pcb_current;
static Semaphore message_buffer, process_mailbox[MAX_THREAD], empty;
static ListHead head;
Mailbox mailbox[MAX_MAIL];
int mail_iter = -1;

void init_msg()
{
	new_sem(&message_buffer, 1);
	new_sem(&empty, 0);
	int i;
	for (i = 0; i < MAX_THREAD; i++) {
		new_sem(&process_mailbox[i], 0);
	}
	for (i = 0; i < MAX_MAIL; i++) {
		mailbox[i].use = 0;
	}
	list_init(&head);
}

void in_mailbox(Message *m)
{
	do{
		mail_iter = (mail_iter + 1) % MAX_MAIL;
	}
	while (mailbox[mail_iter].use == 1); 
	Mailbox *mail = &mailbox[mail_iter];
	mail->use = 1;
	memcpy(&(mail->message), m, sizeof(Message));
	list_init(&(mail->link));
	list_add_before(&head, &(mail->link));
}

void out_mailbox(Message *m, pid_t dst)
{
	ListHead *temp = NULL;
	Mailbox *mail = NULL;
	list_foreach(temp, &head) {
		mail = list_entry(temp, Mailbox, link);
		if (mail->message.dest == dst) {
			memcpy(&(mail->message), m, sizeof(Message));
			list_del(&(mail->link));
			mail->use = 0;
		}
	}
}
/* We assume we will not sti() in the whole interrupt process which sound reasonable in nanos */
void send(pid_t dst, Message *m)
{
	printk("%d send to %d and type: %d\n", pcb_current->pid, dst, m->type);
	m->dest = dst;
	if(pcb_current->state == INTERRUPTED) {
		m->src = MSG_HWINTR;NOINTR;
		in_mailbox(m);	NOINTR;
		/* equal to V */
		process_mailbox[m->dest].count++;NOINTR;	
		if(process_mailbox[m->dest].count <= 0) {
			PCB *pcb = list_entry(process_mailbox[m->dest].queue.next, PCB, semq); NOINTR;
			list_del(process_mailbox[m->dest].queue.next);NOINTR;
			wakeup(pcb);NOINTR;
		}
	}
	else{
		m->src = pcb_current->pid;
		P(&message_buffer);INTR;
		in_mailbox(m);INTR;
		V(&empty);INTR;
		assert(empty.count <= MAX_MAIL);INTR;
		V(&message_buffer);INTR;
		V(&process_mailbox[m->dest]);INTR;
	}
	
}

void receive(pid_t dst, Message *m)
{
	assert(pcb_current->state != INTERRUPTED);
	P(&empty);INTR;
	if(dst != ANY) {
		P(&process_mailbox[dst]);INTR;
		P(&message_buffer);INTR;
		out_mailbox(m, dst);INTR;
		printk("%d receive from %d\n", pcb_current->pid, dst);
		V(&message_buffer);INTR;
	}
	else {
		Mailbox *mail = list_entry(head.next, Mailbox, link);
		int dest = mail->message.dest;INTR;
		P(&process_mailbox[dest]);INTR;
		P(&message_buffer);INTR;
		out_mailbox(m, dest);INTR;
		printk("%d receive from ANY(%d)\n", pcb_current->pid, dest);
		V(&message_buffer);
	}
	printk("m.type: %d\n", m->type);
}

