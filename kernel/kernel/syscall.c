#include<sys/syscall.h>
#include"x86.h"
#include"device.h"
#include"common.h"

static int row=0;
static int col=0;
extern SegDesc gdt[NR_SEGMENTS];
extern TSS tss;

int draw_string(char*,int);
void schedule(void);
void idle(void);

int getbase()
{
	int index=current->regs.ds>>3;
	SegDesc seg=gdt[index];
	return (uint32_t)seg.base_15_0 + (uint32_t)((uint32_t)(seg.base_23_16)<<16) + (uint32_t)((uint32_t)(seg.base_31_24)<<24);
}

void do_syscall(struct TrapFrame* tf)
{
	switch(current->regs.eax)
	{
		case SYS_write:
		{
			char* str = (char*)current->regs.ecx+getbase();
			int len = current->regs.edx;
			current->regs.eax= draw_string(str,len);
			break;
		}
		case SYS_wait4:
		{
			current->sleep_time=current->regs.ebx;
			current->state=STATE_BLOCKED;
			block_insert(current);
			current=NULL;
			schedule();
			break;
		}
		case SYS_exit:
		{
			current->state=STATE_DEAD;
			free_pcb(current);
			current=NULL;
			schedule();
			break;
		}
		case SYS_fork:
		{
			gdt[SEG_UCODE2] = SEG(STA_X|STA_R,0x400000,0xffffffff,DPL_USER);
			gdt[SEG_UDATA2] = SEG(STA_W,0x400000,0xffffffff,DPL_USER);
			set_gdt(gdt,sizeof(gdt));
			uint8_t* src=(uint8_t*)0x200000;
			uint8_t* dst=(uint8_t*)0x400000;
			int i;
			for(i=0;i<0x200000;++i)
				dst[i]=src[i];
			PCB* pcb = new_pcb();
			if(pcb==NULL)
			{
				current->regs.eax=-1;
				break;
			}
			pcb->time_count = current->time_count/2;
			current->time_count=current->time_count/2;
			pcb->regs.edi=current->regs.edi;
			pcb->regs.esi=current->regs.esi;
			pcb->regs.ebp=current->regs.ebp;
			pcb->regs.xxx=current->regs.xxx;
			pcb->regs.ebx=current->regs.ebx;
			pcb->regs.edx=current->regs.edx;
			pcb->regs.ecx=current->regs.ecx;
			pcb->regs.eax=current->regs.eax;
			pcb->regs.eip=current->regs.eip;
			pcb->regs.cs=USEL(SEG_UCODE2);
			pcb->regs.eflags=current->regs.eflags;
			pcb->regs.esp=current->regs.esp;
			pcb->regs.ss=USEL(SEG_UDATA2);
			pcb->regs.gs=USEL(SEG_UDATA2);
			pcb->regs.fs=USEL(SEG_UDATA2);
			pcb->regs.es=USEL(SEG_UDATA2);
			pcb->regs.ds=USEL(SEG_UDATA2);
			pcb->sleep_time=0;
			pcb->state=STATE_RUNNABLE;
			pcb->regs.eax=0;
			current->regs.eax=pcb->pid;
			pcb->sem = current->sem;
			int k=0;
			for(k=0;k<MOST_FILE;++k) pcb->f[k] = current->f[k];
			runnable_insert(pcb);
			break;
		}
		case 5:
		{
			create_sem(current->regs.ebx);
			break;
		}
		case 7:
		{
			P((Semaphore**)(&(current->sem)));
			break;
		}
		case 8:
		{
			V((Semaphore**)(&(current->sem)));
			break;
		}
		case 6:
		{
			int i;
			for(i=0;i<MAX_PCB;++i)
			{
				if(pcb_table[i].sem!=NULL && (&pcb_table[i])!=idle_pcb)
				{
					free_pcb(&pcb_table[i]);
				}
			}
			current=NULL;
			schedule();
			break;
		}
		case 9:
		{
			char* filename = (char*)current->regs.ebx+getbase();
			struct File* f=k_open(filename);
			if(f==NULL) f=creat(filename);
			int i=0;
			for(;i<MOST_FILE;++i)
			{
				if(current->f[i]==NULL)
				{
					current->f[i]=f;
					current->regs.eax=i;
					break;
				}
			}
			break;
		}
		case 10:
		{
			int fd = current->regs.ebx;
			void* buf = (void*)current->regs.ecx+getbase();
			int count = current->regs.edx;
			struct File* file= current->f[fd];
			current->regs.eax= k_read(file,buf,count);
			break;
		}
		case 11:
		{
			int fd = current->regs.ebx;
			void* buf = (void*)current->regs.ecx+getbase();
			int count=current->regs.edx;
			struct File* file = current->f[fd];
			current->regs.eax=k_write(file,buf,count);
			break;
		}
		case 12:
		{
			int fd = current->regs.ebx;
			struct File* file = current->f[fd];
			int ans=k_close(file);
			current->f[fd]=NULL;
			current->regs.eax=ans;
			break;
		}
		default:
		{
			printk("irq=%d",current->regs.eax);
			assert(0);
		}
	}
}

int draw_string(char* str,int len)
{
	int i ;
	for(i=0;i<len;++i)
	{
		if(str[i]=='\n')
		{
			col=0;
			++row;
		}
		else
		{
			unsigned short data = (0x0c<<8)|(*(str+i));
			unsigned short* addr = (unsigned short*)(0xb8000+160*row + 2*col);
			col++;
			if(col>=80)
			{
				col=0;
				row++;
			}
			*addr=data;
		}
	}
	return len;
}

void schedule(void)
{
	if(current!=NULL) return;
	if(runnable_head!=NULL)
	{
		current=runnable_head;
		runnable_head=runnable_head->next;
		current->next=NULL;
		current->state=STATE_RUNNING;
		tss.esp0=(uint32_t)(((uint8_t*)(&(current->regs)))+sizeof(current->regs));
	}
	else 
	{
		current=idle_pcb;
		if(block_head==NULL)
			printk("^");
	}
}
