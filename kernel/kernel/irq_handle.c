#include "x86.h"
#include "device.h"

void do_syscall(struct TrapFrame*);
void schedule(void);
int draw_string(char*, int);

unsigned int getKeyCode()
{
	unsigned int code = in_byte(0x60);
	unsigned int val = in_byte(0x61);
	out_byte(0x61,val|0x80);
	out_byte(0x61,val);
	return code;
}

void
irq_handle(struct TrapFrame *tf) {
    /*
     * 中断处理程序
     */
	asm volatile("movw %%cx,%%ds"::"c"(KSEL(SEG_KDATA)));
	asm volatile("movw %%cx,%%es"::"c"(KSEL(SEG_KDATA)));
	asm volatile("movw %%cx,%%fs"::"c"(KSEL(SEG_KDATA)));
	asm volatile("movw %%cx,%%gs"::"c"(KSEL(SEG_KDATA)));
    switch(current->regs.irq) {
		case 0x2e:break;
		case 0x21:
				  {
					  int code = getKeyCode();
					  draw_string((char*)&code,1);
					  break;
				  }
		case 0x20:
			{
				if(current!=NULL && current->time_count>0)
					current->time_count--;
				struct PCB* head=block_head;
				while(head!=NULL)
				{
					if(head->sleep_time>0)
						--head->sleep_time;
					PCB* temp = head->next;
					if(head->sleep_time==0)
					{
						block_delete(head);
						runnable_insert(head);
						head->state=STATE_RUNNABLE;
					}
					head=temp;
				}
				if(current!=NULL&&current->time_count==0)
				{
					if(current!=idle_pcb)
					{
						current->time_count=10;
						runnable_insert(current);
					}
					current=NULL;
					schedule();
				}
				break;
			}
		case 0x80:do_syscall(tf);break;
        default:
				  {
					  printk("%d\n",current->regs.irq);
					  assert(0);
				  }
    }
}

