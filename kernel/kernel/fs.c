#include"x86.h"
#include"common.h"

struct dir k_dir;
struct bitmap k_bitmap;
struct inode k_inode[nr_entries];
struct File file[MOST_FILE_OPEN];
struct File* free_file=file;
struct File* file_list=NULL;

void readsect(void* dst,int offset);
int writesect(int,void*,int);


int alloc_free_sect()
{
	int i=0;
	for(i=0;i<BITMAP_SIZE;++i)
	{
		if(k_bitmap.mask[i]==0) 
		{
			k_bitmap.mask[i]=1;
			return i;
		}
	}
	return -1;
}

void free_sect(int index)
{
	k_bitmap.mask[index]=0;
}

struct File* new_file()
{
	if(free_file==NULL) return NULL;
	struct File* ans = free_file;
	free_file=free_file->next;
	ans->next=NULL;
	ans->next=file_list;
	file_list=ans;
	return ans;
}

void delete_file(struct File* f)
{
	struct File* head=file_list;
	if(head==f)
	{
		head=head->next;
		f->next=free_file;
		free_file=f;
	}
	else
	{
		while(head->next!=f) head=head->next;
		head->next=f->next;
		f->next=free_file;
		free_file=f;
	}
}

void init_disk()
{
	int i=0,j=0;
	for(;i<nr_entries;++i)
	{
		for(j=0;j<nr_blocks;++j)
			k_inode[i].data_block_offsets[j] = -1;
	}

	uint8_t buffer[512];
	readsect(buffer,DIR_START);
	k_dir.index=(buffer[0]<<24) + (buffer[1]<<16) + (buffer[2]<<8) + (buffer[3]);
	struct dirent* p=(struct dirent*)(buffer+4);
	for(i=0;i<=k_dir.index;++i)
	{
		strcpy(k_dir.entries[i].filename,p->filename);
		k_dir.entries[i].inode_offset=p->inode_offset;
		k_dir.entries[i].file_size=p->file_size;
		readsect(k_inode[p->inode_offset].data_block_offsets,INODE_START+p->inode_offset);
		p++;
	}

	for(i=0;i<MOST_FILE_OPEN-1;++i)
	{
		file[i].next=&file[i+1];
	}
	file[MOST_FILE_OPEN-1].next=NULL;
	
	for(i=BITMAP_START;i<DIR_START;++i)
		readsect(k_bitmap.mask+(i-BITMAP_START)*512,i);
}

struct File* k_open(char* filename)
{
	int index = k_dir.index;
	int i=0;
	for(i=0;i<=index;++i)
		if(strcmp(k_dir.entries[i].filename,filename)==0)
		{
			struct File* f=new_file();
			f->read_offset=0;
			f->write_offset=0;
		    f->inode_offset=k_dir.entries[i].inode_offset;
			f->filesz = k_dir.entries[i].file_size;
			return f;
		}
	return NULL;
}

int k_read(struct File* f,void* dst,int count)
{
	int read_offset = f->read_offset;
	int inode_offset = f->inode_offset;
	int sect_gl=-1;
	int i=0;
	uint8_t buffer[512];
	for(;i<count;++i)
	{
		int start = (read_offset+i)/512;
		if(start>f->filesz-1) 
		{
			f->read_offset+=(i+1);
			return i+1;
		}
		if(start!=sect_gl)
		{
			readsect(buffer,k_inode[inode_offset].data_block_offsets[start]);
			sect_gl=start;
		}
		((uint8_t*)dst)[i] = buffer[(read_offset+i)%512];
	}
	f->read_offset+=(count);
	return count;
}

int k_write(struct File* f,void* src,int count)
{
	int write_offset = f->write_offset;
	int inode_offset = f->inode_offset;
	uint8_t buffer[512];
	int sect_gl=-1;
	int i=0;
	int old=-1;
	int write_back=0;
	for(;i<count;++i)
	{
		int start=(write_offset+i)/512;
		if(start>f->filesz-1)
		{
			f->write_offset+=(i+1);
			return i+1;
		}
		if(start!=sect_gl)
		{
			if(write_back) 
			{
				writesect(k_inode[inode_offset].data_block_offsets[old],buffer,512);
			//	writesect(old,buffer+256,256);
			}
			readsect(buffer,k_inode[inode_offset].data_block_offsets[start]);
			sect_gl=start;
		}
		buffer[(write_offset+i)%512]=((uint8_t*)src)[i];
		old=start;
		write_back=1;
	}
	f->write_offset+=count;
	writesect(k_inode[inode_offset].data_block_offsets[old],buffer,512);
	//writesect(old,buffer+256,256);
	return count;
}

int k_close(struct File* f)
{
	if(f==NULL) return -1;
	delete_file(f);
	return 1;
}
	
struct File* creat(char* filename)
{
	k_dir.index++;
	strcpy(k_dir.entries[k_dir.index].filename,filename);
	int i=0;
	for(;i<nr_entries;++i)
		if(k_inode[i].data_block_offsets[0]==-1) 
		{
			k_dir.entries[k_dir.index].inode_offset = i;
			k_dir.entries[k_dir.index].file_size=1;
			k_inode[i].data_block_offsets[0]=alloc_free_sect();
			struct File* f=new_file();
			f->read_offset=0;
			f->write_offset=0;
			f->inode_offset=i;
			f->filesz=1;
			return f;
		}
	return NULL;
}
