#include"lib.h"

int buf[10];

void uentry(void)
{
	int buf1[5]={1,2,3,4,5};
	int buf2[5]={6,7,8,9,10};
	int ans[10]={-1};
	
	int fd=open("test");
	write(fd,buf1,20);
	write(fd,buf2,20);
	close(fd);

	fd=open("test");
	read(fd,ans,40);
	int i;
	for(i=0;i<10;++i) printf("%d\n",ans[i]);
	exit(0);
}
