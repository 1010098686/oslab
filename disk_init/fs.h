#define MAX_NAME_LEN 24
#define nr_entries 15
#define nr_blocks 128
#define BITMAP_SIZE 512*256

#define KERNEL_START 1
#define BITMAP_START 201
#define DIR_START 457
#define INODE_START 458
#define DATA_START 473

struct inode
{
	unsigned int data_block_offsets[512/sizeof(unsigned int)];
};

struct dirent
{
	char filename[MAX_NAME_LEN];
	unsigned int inode_offset;
	unsigned int file_size;
};

struct dir
{
	int index;
	struct dirent entries[nr_entries];
	int useless[7];
};

struct bitmap
{
	unsigned char mask[512*256];
};

