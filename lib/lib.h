#ifndef __lib_h__
#define __lib_h__

void printf(const char *format,...);
int fork(void);
void sleep(int time);
void exit(int status);
void createSem(int n);
void destroySem();
void lockSem();
void unlockSem();
int open(char* filename);
int read(int fd,void* buf,int count);
int write(int fd,void*buf,int count);
int close(int fd);

#endif
