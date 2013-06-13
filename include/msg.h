#ifndef __MSG_H__
#define __MSG_H__
#include "thread.h"
#include "common.h"
#include "debug.h"

#define MAX_MAIL 100
#define ANY -1
#define MSG_SZ 30
#define MSG_HWINTR 0 /* Message from interrupt */
struct Message {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
    char payload[MSG_SZ];
};
typedef struct Message Message;

struct Mailbox {
	Message message;
	boolean use;
	ListHead link;
};
typedef struct Mailbox Mailbox;
void init_msg();
void send(pid_t dst, Message *m);
void receive(pid_t src, Message *m);












#endif
