#include "x86.h"
#include "kernel.h"
#include "debug.h"
#include "thread.h"
#include "irq.h"

static Irq_Pool *event_handle[NR_EVENT];
static Irq_Pool event_function[NR_EVENT_FUNC];
static int function_count = 0;
TrapFrame *tf_return;
extern PCB *pcb_current;
//void schedule();

void
init_irq()
{
	int i;
	for (i = 0; i < NR_EVENT; i++) {
		event_handle[i] = NULL;
	}
}


void
add_irq_handle(int irq_id, void (*func)(void))
{
	assert(irq_id < NR_EVENT);
	/* Insert into the function list of every irq_id */
	Irq_Pool *ptr = &event_function[function_count ++]; /* Take one function from the pool */
	ptr->func = func;
	ptr->next = event_handle[irq_id];
	event_handle[irq_id] = ptr;
}
/* It is NOINTR in irq_handle */
void 
irq_handle(TrapFrame *tf) {
	/* only for the first process the system ever run. Thanks to the branch prediction */
	if(pcb_current == NULL){
		schedule();
	}
	else{
		pcb_current->tf[pcb_current->count_tf ++] = tf;
		if(pcb_current->state != STOPPED) {
			pcb_current->state = INTERRUPTED;
		}
		assert(pcb_current->count_tf < NR_TF);
		int irq = tf->irq;
		assert(irq >= 0);
		/* int $0x80 */
		if(irq == 128){
			schedule();
		}
		else if(irq < 1000) {		
			cli();
			printk("Unexpected exception #%d\n", irq);
			printk(" errorcode %x\n", tf->err);
			printk(" location  %d:%x, esp %x\n", tf->cs, tf->eip, tf);
			panic("unexpected exception");

		}
		else {
			Irq_Pool *ptr = event_handle[irq - 1000];
			while(ptr != NULL){
				ptr->func();
				ptr = ptr->next;
			}
			if(irq == 1001)
				printk("IRQ = %d\n", irq);  
		}
	}
	pcb_current->state = RUNNING;
	tf_return = pcb_current->tf[ --pcb_current->count_tf];
}

