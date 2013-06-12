#ifndef __MSG_H__
#define __MSG_H__
#include "thread.h"
#include "common.h"
#include "debug.h"

#define ANY 0
#define MSG_SZ 1000
#define MSG_HWINTR -1 /* Message from interrupt */
struct Message {
    int type; // 消息的类型
    pid_t src, dest; // 消息发送者和接收者的pid
    char payload[MSG_SZ];
};
typedef struct Message Message;

void init_msg();
void send(pid_t dst, Message *m);
void receive(pid_t dst, Message *m);












#endif
