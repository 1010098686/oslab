#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "pcb.h"

#define MAX_SEM 10
typedef struct
{
	int val;
	struct PCB* list;
}Semaphore;

extern Semaphore sem_table[MAX_SEM];
extern int free_sem;

void P(Semaphore** sem);
void V(Semaphore** sem);
void init_sem_table();
void create_sem(int n);

#endif
