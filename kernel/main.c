#include "common.h"
#include "x86.h"
#include "device.h"

extern TSS tss;

void idle(void)
{
	asm volatile ("movl %0,%%esp": :"r"((uint32_t)(((uint8_t *)(&(current->regs)))+sizeof(current->regs)-8)));
	enable_interrupt();
	while(1)
		wait_for_interrupt();
}

void init_idle(void)
{
	PCB* pcb = new_pcb();
	if(pcb==NULL) assert(0);
	current=pcb;
	pcb->time_count =1 ;
	idle_pcb=pcb;
	//tss.esp0=(uint32_t)(((uint8_t*)(&(current->regs)))+sizeof(current->regs));
}

void
kentry(void) {
	init_serial();      //初始化串口输出
    init_idt();
	init_intr();
    init_seg();
	init_pcb_table();
	init_idle();
	init_sem_table();
	init_timer();
	init_disk();
    load_umain();
    idle();
	assert(0);
}
