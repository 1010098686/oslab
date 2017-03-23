#include "lib.h"
#include "types.h"
#include<sys/syscall.h>
/*
 * io lib here
 * 库函数写在这
 */
static inline int32_t syscall(int num, int check, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;
	//Generic system call: pass system call number in AX
	//up to five parameters in DX,CX,BX,DI,SI
	//Interrupt kernel with T_SYSCALL
	//
	//The "volatile" tells the assembler not to optimize
	//this instruction away just because we don't use the
	//return value
	//
	//The last clause tells the assembler that this can potentially
	//change the condition and arbitrary memory locations.

	/*Lab2 code here
	  嵌入汇编代码，调用int $0x80
	 */
    asm volatile("int $0x80":"=a"(ret):"a"(num),"b"(a1),"c"(a2),"d"(a3));
	return ret;
}


int strlen(const char* str)
{
	int i=0;
	while(str[i]!=0) i++;
	return i;
}

char num2ch(unsigned int num)
{
	if(num>=0 && num<=9) return '0'+num;
	else return 'a'+num-10;
}

int vsprintf(char* buf,int size,const char* format,void* list)
{
	int index=0;
	int len = strlen(format);
	int i=0;
	while(i<len)
	{
		if(format[i]!='%')
		{
			buf[index]=format[i];
			index++;
			i++;
		}
		else
		{
			switch(format[i+1])
			{
				case 'd':
					{
						int res = *((int*)list);
						list=list+4;
						i=i+2;
						int flag = (res<0);
						int  temp=res;
						int len=0;
						while(temp!=0)
						{
							len++;
							temp=temp/10;
						}
						temp=0;
						if(res==0)
						{
							buf[index]='0';
							index++;
						}
						else if(flag)
						{
							for(temp=len;temp>=1;--temp)
							{
								buf[index+temp]=-(res%10)+'0';
								res=res/10;
							}
							buf[index]='-';
							index=index+len+1;
						}
						else
						{
							for(temp=len-1;temp>=0;--temp)
							{
								buf[index+temp]=res%10+'0';
								res=res/10;
							}
							index=index+len;
						}
						break;
					}
				case 'x':
				        {
				        	unsigned int res = *((int*)list);
							list=list+4;
							i=i+2;
							unsigned int a[8];
							int k;
							for(k=7;k>=0;--k)
							{
								a[k]=(unsigned )(res&0xf);
								res=res>>4;
							}
							k=0;
							while(a[k++]==0 && k<8);
							for(k--;k<8;++k)
							{
								buf[index++]=num2ch(a[k]);
							}
							break;
				        }
				case 'c':
					{
						char ch = *((char*)list);
						list=list+4;
						i=i+2;
						buf[index]=ch;
						++index;
						break;
					}
				case 's':
					{
						char* str=*((char**)list);
						list=list+4;
						i=i+2;
						int len = strlen(str);
						int j=0;
						for(j=0;j<len;++j)
						{
							buf[index]=str[j];
							++index;
						}
						break;
					}
				default:break;
			}
		}
		if(index>=size) return index;
	}
	return index;
}

void printf(const char *format,...){
	static char buf[256];
	void* args = (void**)&format+1;
	int len = vsprintf(buf,256,format,args);
	syscall(SYS_write,0,0,(unsigned int)buf,len,0,0);
}

int fork(void)
{
	return syscall(SYS_fork,0,0,0,0,0,0);
}

void sleep(int time)
{
	syscall(SYS_wait4,0,time,0,0,0,0);
}

void exit(int status)
{
	syscall(SYS_exit,0,status,0,0,0,0);
}

void createSem(int n)
{
	syscall(5,0,n,0,0,0,0);
}

void destroySem()
{
	syscall(6,0,0,0,0,0,0);
}

void lockSem()
{
	syscall(7,0,0,0,0,0,0);
}

void unlockSem()
{
	syscall(8,0,0,0,0,0,0);
}

int open(char* filename)
{
	return syscall(9,0,(uint32_t)filename,0,0,0,0);
}

int read(int fd,void* buf,int count)
{
	return syscall(10,0,fd,(uint32_t)buf,count,0,0);
}

int write(int fd,void* buf,int count)
{
	return syscall(11,0,fd,(uint32_t)buf,count,0,0);
}

int close(int fd)
{
	return syscall(12,0,fd,0,0,0,0);
}
