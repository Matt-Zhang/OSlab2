#include "x86.h"
#include "kernel.h"
#include "debug.h"
//static void (*do_timer)(void);
static void (*do_switch)(TrapFrame*);


//void
//set_timer_intr_handler( void (*ptr)(void) ) {
//	do_timer = ptr;
//}
void
set_switch_intr_handler( void (*ptr)(TrapFrame *) ) {
	do_switch = ptr;
}

void irq_handle(TrapFrame *tf) {
	int irq = tf->irq;
	assert(irq >= 0);

	if (irq < 1000) {
		// exception
		if(irq == 128){
			do_switch(tf);
		}
		else {
			cli();
			printk("Unexpected exception #%d\n", irq);
			printk(" errorcode %x\n", tf->err);
			printk(" location  %d:%x, esp %x\n", tf->cs, tf->eip, tf);
			panic("unexpected exception");
		}
	} 
	else if (irq >= 1000) {
		if (irq == 1000) {		
			do_switch(tf);
		}
		// external interrupt
	}	
}

