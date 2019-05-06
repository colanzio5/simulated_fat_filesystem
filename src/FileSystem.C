#include "FileSystem.h"

// -----------
// CONSTRUCTOR
// -----------
FileSystem::FileSystem(BlockDevice *device)
{
	// set the device of the filesystem
	this->device = new IDevice(device);

	/**
	 * try and find an existing super block on the device
	 * if it exists (as identified by TINYFS header) use the super block
	 * if a super block is not found, create a new one
	 */
	char super_block_buffer[sizeof(SuperBlock)];
	int res = this->device->readFromDevice(0, 0, sizeof(SuperBlock), super_block_buffer);
	SuperBlock *super_block = reinterpret_cast<SuperBlock *>(super_block_buffer);
	if (strcmp(super_block->magic, TINYFS) == 0)
	{
		printf("[FileSystem]: super block exists, using existing super block...\n");
		this->super_block = super_block;
	}
	else
	{
		printf("[FileSystem] super block not found, creating a new one...\n");
		this->formatFileSystem();
	}
	this->debugFileSystem(true);
}

bool FileSystem::formatFileSystem()
{

	// erase all the contents of the device
	// by writing '\0' to all memory in all
	// block in the device
	uint32_t block_size = this->device->block_size;
	char empty_block[block_size] = {'\0'};
	for (int i = 0; i < this->device->number_blocks; i++) 	// write to all blocks in the device
		int res = this->device->writeToDevice(i, 0, this->device->number_blocks, empty_block);

	// now that the device is clean,
	// write all the data segments to the device
	// create and write superblock to device
	char *super_block_buffer = new char[sizeof(SuperBlock)];
	SuperBlock* super_block = new SuperBlock();
	memcpy(super_block_buffer, super_block, sizeof(SuperBlock));
	if(this->device->writeToDevice(SUPERBLOCK_POSN, 0, sizeof(SuperBlock), super_block_buffer) < 0)
		printf("[formatFileSystem]: error writing super block to device...\n");

	// create and write fat table to the device
	int fat_table_num_bytes = this->_getFatTableNumberBytes();
	
	// create and write free map to device
	int free_map_num_bytes  = this->_getFreeMapNumberBytes();
	char free_map_buffer[free_map_num_bytes] = { 0 };
	// mark super block free map blocks and fat table as reserved
	
	if(int res = this->device->writeToDevice(FREEMAP_POSN, 0, free_map_num_bytes, (char*)free_map_buffer) < 0)
		printf("[formatFileSystem]: error writing free map to device...\n");
	
	// create and write fat
	return true;
}

/**
 * @brief 
 * 		- shows all of the filesystem attributes
 * 		- if verbose all blocks are hexdumped
 * @param verbose 
 */
void FileSystem::debugFileSystem(bool verbose)
{
	std::cout << "[device]:\n";
	std::cout << "\tnumber_blocks:" << this->device->number_blocks << "\n"
			  << "\tblock_size:" << this->device->block_size << "\n";

	std::cout << "[superblock]:\n";
	std::cout << "\rroot_dir_block:" << this->super_block->root_dir_block << "\n"
			  << "\troot_dir_bytes:" << this->super_block->root_dir_bytes << "\n"
			  << "\tfat_block:" << this->super_block->fat_block << "\n"
			  << "\tfree_block:" << this->super_block->free_block << "\n";

	if (verbose == true)
	{
		// print out all the blocks
		char *block = new char[this->device->block_size];
		for (int blk_idx = 0; blk_idx < this->device->number_blocks; blk_idx++)
		{
			int res = this->device->readFromDevice(blk_idx, 0, this->device->block_size, block);
			hexDump(block, this->device->block_size);
		}
	}
}