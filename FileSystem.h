#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

#define MAXFNAMELEN 256
#define SUPERBLOCK_POSN 0
#define FREEMAP_POSN 100
#define FAT_POSN 200
#define TINYFS "TinyFS"

typedef struct
{
	char magic[sizeof(TINYFS)]; /* magic format identifier */
	uint32_t free_block;		/* first block of freemap block */
	uint32_t fat_block;			/* first block of file allocation table */
	/* fat_block[fat_idx_root] is the first block of the root directory */
	uint32_t fat_idx_root;
} SuperBlock;

typedef struct
{
	uint32_t start_blk; // first block of file
	uint32_t size;		// size of file in bytes
	bool directory;		// true:directory, false:file
	char fname[MAXFNAMELEN];
} DirEntry;

#endif //FILE_STSTEM_H
