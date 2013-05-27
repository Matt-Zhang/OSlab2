#include "kernel.h"
#include "x86.h"
#include "vm.h"
#include "irq.h"
#include "thread.h"
#include "common.h"



void
os_init(void) {
	init_seg();
	init_debug();
	init_idt();
	init_i8259();
	set_switch_intr_handler(switch_event);
	printk("The OS is now working!\n");
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

