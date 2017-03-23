#include "x86.h"
#include "device.h"

#define SECTSIZE 512

SegDesc gdt[NR_SEGMENTS];       // the new GDT
TSS tss;

void readsect(void*,int);

void
init_seg() { // setup kernel segements
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
    gdt[SEG_TSS].s = 0;
	set_gdt(gdt, sizeof(gdt));

    /*
	 * 初始化TSS
	 */
	tss.ss0 = KSEL(SEG_KDATA);
	tss.esp0 = 0x200000;
	asm volatile("ltr %0" :: "r"((uint16_t)KSEL(SEG_TSS)));
	
	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

	lldt(0);
}


void
enter_user_space(int entry) {
    /*
     * Before enter user space 
     * you should set the right segment registers here
     * and use 'iret' to jump to ring3
     * 进入用户空间
     */
	asm volatile("pushl %0"::"b"(USEL(SEG_UDATA)));
	asm volatile("pushl %0"::"b"(0x100000));
	asm volatile("pushl $0x202");
	asm volatile("pushl %0"::"b"(USEL(SEG_UCODE)));
	asm volatile("pushl %0"::"a"(entry));
	asm volatile("movw %%ax,%%ds":: "a" (USEL(SEG_UDATA)));
	asm volatile("iret");
}

void
load_umain(void) {
    /*
     * Load your app here
     * 加载用户程序
     */
	PCB* pcb=new_pcb();
	if(pcb==NULL) assert(0);
	pcb->state=STATE_RUNNING;
	pcb->regs.ss=USEL(SEG_UDATA);
	pcb->regs.esp=0x100000;
	pcb->regs.eflags=0x202;
	pcb->regs.cs=USEL(SEG_UCODE);
	pcb->regs.ds=USEL(SEG_UDATA);
	pcb->regs.es=USEL(SEG_UDATA);
	pcb->regs.fs=USEL(SEG_UDATA);
	pcb->regs.gs=USEL(SEG_UDATA);
	struct ELFHeader* elf;
	struct ProgramHeader* ph=NULL;
	struct File* fd = k_open("umain");
	int size = fd->filesz;
	uint8_t buf[40*512];
	k_read(fd,buf,size*512);
	k_close(fd);
	elf=(void*)buf;
	int i=0;
	for(i=0;i<elf->phnum;++i)
	{
		ph=(void*)buf+elf->phoff+i*elf->phentsize;
		if(ph->type==1)
		{
			uint8_t* addr = (uint8_t*)0x200000+ph->vaddr;
			int j=0;
			for(j=0;j<ph->filesz;++j)
			{
				addr[j]=buf[ph->off+j];
			}
			for(j=0;j<ph->memsz-ph->filesz;++j)
			{
				addr[ph->filesz+j]=0;
			}
		}
	}
	gdt[SEG_UCODE] = SEG(STA_X|STA_R,0x200000,0xffffffff,DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_R,0x200000,0xffffffff,DPL_USER);
	pcb->regs.eip=elf->entry;
	runnable_insert(pcb);
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); 
}

void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte2(0x1F2, 1);
	out_byte2(0x1F3, offset);
	out_byte2(0x1F4, offset >> 8);
	out_byte2(0x1F5, offset >> 16);
	out_byte2(0x1F6, (offset >> 24) | 0xE0);
	out_byte2(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

