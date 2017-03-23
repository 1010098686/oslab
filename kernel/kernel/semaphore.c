#include"x86.h"

void schedule(void);

Semaphore sem_table[MAX_SEM];
int free_sem;

void enqueue(PCB** list,PCB* pcb)
{
	pcb->next=NULL;
	if(*(list)==NULL) *(list)=pcb;
	else 
	{
		PCB* head = *(list);
		while(head->next!=NULL) head=head->next;
		head->next=pcb;
	}
}

PCB* dequeue(PCB** list)
{
	if(*(list)==NULL) return NULL;
	else 
	{
		PCB* temp = *(list);
		*(list)=(*(list))->next;
		temp->next=NULL;
		return temp;
	}
}

void init_sem_table()
{
	int i;
	for(i=0;i<MAX_SEM;++i)
	{
		sem_table[i].val=1;
		sem_table[i].list=NULL;
	}
	free_sem=0;
}

void create_sem(int n)
{
	sem_table[free_sem].val=n;
	current->sem = (struct Semaphore*)(&(sem_table[free_sem++]));
}

void P(Semaphore** sem)
{
	(*sem)->val--;
	if((*sem)->val<0)
	{
		enqueue(&((*sem)->list),current);
		current=NULL;
		schedule();
	}
}

void V(Semaphore** sem)
{
	(*sem)->val++;
	if((*sem)->val<=0)
	{
		PCB* pcb = dequeue(&((*sem)->list));
		runnable_insert(pcb);
	}
}
