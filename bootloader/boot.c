#include "boot.h"
#include<elf.h>
#include<stddef.h>

#define SECTSIZE 512

void draw_string(int,char*,int);
void waitdisc(void);
void readsect(void*,int);

void bootmain(void)
{
	/* 这里是加载磁盘程序的代码 */
	struct ELFHeader *elf;
	struct ProgramHeader* ph=NULL;
	unsigned char buf[4096];
	unsigned char ide[512];
	readsect(buf,1);
	elf=(void*)buf;
	int i=0;
	for(i=0;i<elf->phnum;++i)
	{
		ph=(void*)buf+elf->phoff+i*elf->phentsize;
		if(ph->type == PT_LOAD)
		{
           //read from disk
			unsigned char* addr = ph->vaddr;
			int j=0;
			for(j=0;j<ph->filesz;++j)
			{
				unsigned sector = (ph->off + j+512)>>9;
				readsect(ide,sector);
				addr[j] = ide[(ph->off+j+512)&511];
			}
			for(j=0;j<ph->memsz-ph->filesz;++j)
			{
				addr[ph->filesz+j]=0;
			}
		}
	}
	((void(*)(void))elf->entry)();
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

