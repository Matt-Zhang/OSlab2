#include "msg.h"
#include "thread.h"
#include "x86.h"
#include "assert.h"
#include "debug.h"
#include "string.h"

extern PCB* pcb_current;
static Semaphore message_buffer, process_mailbox[MAX_THREAD][MAX_THREAD];
static ListHead head;
static Mailbox mailbox[MAX_MAIL];
int mail_iter = -1;

void init_msg()
{
	new_sem(&message_buffer, 1);
	int i, j;
	for (i = 0; i < MAX_THREAD; i++) {
		for(j = 0; j < MAX_THREAD; j++) {
			new_sem(&process_mailbox[i][j], 0);
		}
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

void out_mailbox(Message *m, pid_t src, pid_t dst)
{
	ListHead *temp = NULL;
	Mailbox *mail = NULL;
	list_foreach(temp, &head) {
		mail = list_entry(temp, Mailbox, link);
		if (mail->message.src == src && mail->message.dest == dst && mail->use == 1) {
			memcpy(m, &(mail->message), sizeof(Message));
			list_del(&(mail->link));
			mail->use = 0;
			break;
		}
	}
}
void out_mailbox_any(Message *m, pid_t dst)
{
	ListHead *temp = NULL;
	Mailbox *mail = NULL;
	P(&message_buffer);
	list_foreach(temp, &head) {
		mail = list_entry(temp, Mailbox, link);
		if (mail->message.dest == dst && mail->use == 1) {
			V(&message_buffer);
			P(&process_mailbox[mail->message.src][dst]);
			P(&message_buffer);
			memcpy(m, &(mail->message), sizeof(Message));
			list_del(&(mail->link));
			mail->use = 0;
			break;
		}
	}
	printk("%d receive from ANY(%d), type: %d\n", dst, mail->message.src, mail->message.type);
	V(&message_buffer);
}
/* We assume we will not sti() in the whole interrupt process which sound reasonable in nanos */
/* Because a pid will not send to itself, process_mailbox[a][a] shows whether the dest_a is empty */
void send(pid_t dst, Message *m)
{
	m->dest = dst;
	if(pcb_current->state == INTERRUPTED) {
		m->src = MSG_HWINTR;//NOINTR;
		printk("0(%d) send to %d and type: %d\n", pcb_current->pid, dst, m->type);
	}
	else{
		m->src = pcb_current->pid;
		printk("%d send to %d and type: %d\n", pcb_current->pid, dst, m->type);
	}
		P(&message_buffer);INTR;
		in_mailbox(m);INTR;
		V(&message_buffer);INTR;
		V(&process_mailbox[dst][dst]);INTR;
		V(&process_mailbox[m->src][dst]);INTR;
}

void receive(pid_t src, Message *m)
{
	assert(pcb_current->state != INTERRUPTED);
	int dst = pcb_current->pid; 
	P(&process_mailbox[dst][dst]);INTR;
	if(src != ANY) {
		P(&process_mailbox[src][dst]);INTR;
		P(&message_buffer);INTR;
		out_mailbox(m, src, dst);INTR;
		printk("%d receive from %d and type:%d\n", dst, src, m->type);
		V(&message_buffer);INTR;
	}
	else {
		out_mailbox_any(m, dst);
	}
}

