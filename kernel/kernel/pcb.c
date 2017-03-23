#include "x86.h"
#include "common.h"

PCB pcb_table[MAX_PCB];
PCB* block_head,*block_tail;
PCB* runnable_head,*runnable_tail;
PCB* free;
PCB* current;
PCB* idle_pcb;

void init_pcb_table()
{
	int i;
	for(i=0;i<MAX_PCB-1;++i)
	{
		pcb_table[i].next=&pcb_table[i+1];
		pcb_table[i].pid=i+1;
		pcb_table[i].state=STATE_NEW;
		pcb_table[i].time_count=0;
		pcb_table[i].sleep_time=0;
		pcb_table[i].sem=NULL;
		int j=0;
		for(;j<MOST_FILE;++j) pcb_table[i].f[j]=NULL;
	}
	pcb_table[MAX_PCB-1].next=NULL;
	pcb_table[MAX_PCB-1].pid=MAX_PCB;
	pcb_table[MAX_PCB-1].state=STATE_NEW;
	pcb_table[MAX_PCB-1].time_count=0;
	pcb_table[MAX_PCB-1].sleep_time=0;
	pcb_table[MAX_PCB-1].sem=NULL;
	int j=0;
	for(;j<MOST_FILE;++j) pcb_table[MAX_PCB-1].f[j]=NULL;

	block_head=block_tail=runnable_head=runnable_tail=NULL;
	free=pcb_table;
	current=NULL;
	idle_pcb=NULL;
}

PCB* new_pcb()
{
	if(free==NULL)
		return NULL;
	else 
	{
		PCB* new=free;
		free=free->next;
		new->next=NULL;
		new->time_count=10;
		new->sleep_time=0;
		return new;
	}
}

void block_insert(PCB* pcb)
{
	pcb->next=NULL;
	if(block_head==NULL)
	{
		block_head=block_tail=pcb;
	}
	else
	{
		block_tail->next=pcb;
		block_tail=pcb;
	}
}

void runnable_insert(PCB* pcb)
{
	pcb->next=NULL;
	if(runnable_head==NULL)
	{
		runnable_head=runnable_tail=pcb;
	}
	else
	{
		runnable_tail->next=pcb;
		runnable_tail=pcb;
	}
}

void block_delete(PCB* pcb)
{
	if(block_head==NULL) return ;
	if(pcb==block_head)
	{
		block_head=block_head->next;
		return ;
	}
	PCB* head=block_head;
	while(head->next!=pcb) head=head->next;
	PCB* del=head->next;
	if(del==block_tail)
		block_tail=head;
	head->next=del->next;
	del->next=NULL;
	return;
}

void runnable_delete(PCB* pcb)
{
	if(runnable_head==NULL) return;
	if(pcb==runnable_head)
	{
		runnable_head=runnable_head->next;
		return ;
	}
	PCB* head=runnable_head;
	while(head->next!=pcb) head=head->next;
	PCB* del=head->next;
	if(del==runnable_tail)
		runnable_tail=head;
	head->next=del->next;
	del->next=NULL;
	return;
}

void free_pcb(PCB* pcb)
{
	pcb->next=free;
	pcb->state=STATE_NEW;
	free=pcb;
}
