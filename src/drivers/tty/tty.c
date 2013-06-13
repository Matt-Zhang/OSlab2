//#include "module.h"
#include "debug.h"
#include "msg.h"
#include "drivers/tty.h"
#include "drivers/hal.h"
//#include "drivers/proc.h"
#include "x86.h"
#include "irq.h"

void init_tty(void) {
	add_irq_handle(1, send_keymsg);
	init_console();
}

void
ttyd(void) {
	Message m;
	int i;

	lock();
	for (i = 0; i < NR_TTY; i ++) {
		hal_register(ttynames[i], TTY, i);
	}
	unlock();

	while (1) {
		receive(ANY, &m);
		/* The message will be sended when keybord interrupt and clock interrupt */
		/* No send or receive in this part */
		if (m.src == MSG_HWINTR) {
			switch (m.type) {
				case MSG_TTY_GETKEY:
					readkey();
					break;
				case MSG_TTY_UPDATE:
					update_banner();
					break;
			}
		}
		/* Handle the dev_rw function, get one order and then retrun back */
		else {
			DevMessage *msg;
			switch(m.type) {
				case MSG_DEVRD:
					read_request((DevMessage*)&m);
					break;
				case MSG_DEVWR:
					msg = (DevMessage*)&m;
					if (msg->dev_id >= 0 && msg->dev_id < NR_TTY) {
						int i;
						/* Prepare the buffer */
						for (i = 0; i < msg->count; i ++) {
							consl_writec(&ttys[msg->dev_id], ((char*)msg->buf)[i]);
						}
						/* Write according to the buffer */
						consl_sync(&ttys[msg->dev_id]);
					}
					m.type = msg->count;
					send(m.src, &m);
					break;
			}
		}
	}
}

