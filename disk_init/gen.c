#include<stdio.h>
#include<assert.h>
#include<string.h>
#include"fs.h"

unsigned char buffer[sizeof(struct bitmap)+sizeof(struct dir)+nr_entries*sizeof(struct inode)];

struct bitmap* k_bitmap;
struct dir* k_dir;
struct inode* k_inode;

int getsize(FILE* fp)
{
	fseek(fp,0,SEEK_END);
	return ftell(fp);
}

void init()
{
	k_bitmap=(struct bitmap*)buffer;
	k_dir = (struct dir*)(buffer+sizeof(struct bitmap));
	k_inode = (struct inode*)(buffer+sizeof(struct bitmap)+sizeof(struct dir));

	int i=0;
	for(;i<BITMAP_SIZE;++i) k_bitmap->mask[i]=0;
	k_dir->index=-1;
	int j=0;
	for(i=0;i<nr_entries;++i)
		for(j=0;j<nr_blocks;++j)
			k_inode[i].data_block_offsets[j]=-1;
}

int main()
{
	init();

	FILE* fp = fopen("../app/umain","rb");
	assert(fp!=NULL);
	int size=getsize(fp);
	fclose(fp);
	int blocks=(size%512==0)?(size/512):(size/512+1);
	
	int i;
	for(i=0;i<DATA_START-1+blocks;++i) k_bitmap->mask[i]=1;
	
	k_dir->index++;
	strcpy(k_dir->entries[0].filename,"umain");
	k_dir->entries[0].file_size=blocks;
	k_dir->entries[0].inode_offset=0;

	for(i=0;i<blocks;++i)
		k_inode[0].data_block_offsets[i]=DATA_START+i;

	int buffer_size = sizeof(struct bitmap)+sizeof(struct dir)+nr_entries*sizeof(struct inode);
	FILE* out = fopen("buffer","wb+");
	assert(out!=NULL);
	fwrite(buffer,buffer_size,1,out);
	fclose(out);
	return 0;
}
