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
	char *super_block_buffer = this->device->readFromDevice(0, 0, sizeof(SuperBlock));
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
}

bool FileSystem::formatFileSystem()
{
	/**
   * erase all the contents of the device
   * by writing '\0' to all memory in all
   * block in the device
   */
	uint32_t block_size = this->device->block_size;
	char *empty_block = new char[block_size];
	// create empty block buffer
	for (uint32_t didx = 0; didx < block_size; didx++)
		empty_block[didx] = '\0';
	// write to all blocks in the device
	for (int i = 0; i < this->device->number_blocks; i++)
		this->device->writeToDevice(i, 0, this->device->number_blocks, empty_block);

	/**
   * now that the device is clean,
   * write all the data segments to the device
   */
}

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
			block = this->device->readFromDevice(blk_idx, 0, this->device->block_size);
			hexDump(block, this->device->block_size);
		}
	}
}