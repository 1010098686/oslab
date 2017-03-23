#ifndef __X86_FS_H
#define __X86_FS_H

#define MAX_NAME_LEN 24
#define nr_entries 15
#define nr_blocks 128
#define BITMAP_SIZE 512*256

#define KERNEL_START 1
#define BITMAP_START 201
#define DIR_START 457
#define INODE_START 458
#define DATA_START 473

#define MOST_FILE_OPEN 20
struct inode
{
	uint32_t data_block_offsets[512/sizeof(uint32_t)];
};

struct dirent
{
	char filename[MAX_NAME_LEN];
	uint32_t inode_offset;
	uint32_t file_size;
};

struct dir
{
	int index;
	struct dirent entries[nr_entries];
	int useless[7];
};

struct bitmap
{
	uint8_t mask[512*256];
};

struct File
{
	int read_offset;
	int write_offset;
	int inode_offset;
	int filesz;
	struct File* next;
};

extern struct dir k_dir;
extern struct bitmap k_bitmap;
extern struct inode k_inode[nr_entries];
extern struct File file[MOST_FILE_OPEN];
extern struct File* free_file;
extern struct File* file_list;

void init_disk();

struct File* new_file();
void delete_file(struct File* f);

struct File*  k_open(char* filename);
int k_read(struct File* f,void* dst,int count);
int k_write(struct File* f,void* src,int count);
int k_close(struct File* f);
struct File* creat(char* filename);

#endif
