#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "thread.h"
#include "common.h"
#include "drivers/tty.h"
#include "drivers/time.h"
#include "drivers/hal.h"
#include "msg.h"

void test1(){
	while(1){
		printk("1");
		int i = 0;
		for (i = 0; i < 10; i++) {
			wait_intr();
		}
	}
}

void test2(){	
	while(1){
		printk("2");
		int i = 0;
		for (i = 0; i < 10; i++) {
			wait_intr();
		}
	}
}


void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8529();
	init_hal();
	init_timer();
	init_tty();
	init_msg();
	printk("The OS is now working!\n");
	wakeup(create_kthread(ttyd));
	/* Setup four ttys */
	test();
//	wakeup(create_kthread(test1));
//	wakeup(create_kthread(test2));
	sti();
	while (TRUE) {
		wait_intr();
	}

}

void
entry(void) {
	init_kvm();
	void(*volatile next)(void) = os_init;
	asm volatile("addl %0, %%esp" : : ""(KOFFSET));
	next();
	panic("init code should never return");
}

