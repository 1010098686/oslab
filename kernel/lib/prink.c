#include"x86.h"
#include"common.h"
#include"device.h"

int vsprintf(char*,int,const char*,void*);

void printk(const char* format,...)
{
	static char buf[256];
	void* args = (void**)&format+1;
	int len = vsprintf(buf,256,format,args);
	int i;
	for(i=0;i<len;++i)
		putchar(buf[i]);
}
