#ifndef __X86_PCB_H__
#define __X86_PCB_H__

#include "memory.h"
#include "semaphore.h"

#define STATE_RUNNING 0
#define STATE_BLOCKED 1
#define STATE_RUNNABLE 2
#define STATE_NEW 3
#define STATE_DEAD 4

#define MAX_PCB 10
#define STACK_SIZE 4096
#define MOST_FILE 5

typedef struct
{
	uint32_t gs,fs,es,ds;
	uint32_t edi,esi,ebp,xxx,ebx,edx,ecx,eax;
	int32_t irq;
	uint32_t eip,cs,eflags,esp,ss;
}stackframe;

typedef struct PCB
{
//	uint8_t kstack[STACK_SIZE];
//	struct TrapFrame* tf;
	stackframe regs;
	int state;
	int time_count;
	int sleep_time;
	unsigned int pid;
	struct Semaphore* sem;
    struct File* f[MOST_FILE];
	struct PCB* next;
}PCB;

extern PCB pcb_table[MAX_PCB];
extern PCB* block_head,*block_tail;
extern PCB* runnable_head,*runnable_tail;
extern PCB* free;
extern PCB* current;
extern PCB* idle_pcb;

inline void init_pcb_table();
inline PCB* new_pcb();
inline void block_insert(PCB* pcb);
inline void runnable_insert(PCB* pcb);
inline void block_delete(PCB* pcb);
inline void runnable_delete(PCB* pcb);
inline void free_pcb(PCB* pcb);

#endif
