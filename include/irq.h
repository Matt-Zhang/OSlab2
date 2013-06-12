#ifndef __IRQ_H__
#define __IRQ_H__

#define NR_EVENT 5
#define NR_EVENT_FUNC 30
#define PORT_PIC_MS    0x20
#define PORT_PIC_SL    0xA0
void init_idt(void);
void init_irq();
void init_i8529();

void add_irq_handle(int irq_id, void (*func)(void));

struct Irq_Pool{
	void (*func)(void);
	struct Irq_Pool *next;
};
typedef struct Irq_Pool Irq_Pool;

// interrupt and exception entries
void irq0();
void irq1();
void irq14();
void vec0();
void vec1();
void vec2();
void vec3();
void vec4();
void vec5();
void vec6();
void vec7();
void vec8();
void vec9();
void vec10();
void vec11();
void vec12();
void vec13();
void vec14();
void vecsys();
void irqall();

#endif
