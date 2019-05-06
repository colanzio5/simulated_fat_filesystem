#include "FileSystem.h"

IDevice::IDevice(BlockDevice* device)
{
	this->device = device;
	this->number_blocks = device->getBlocks();
	this->block_size = device->getBlockSize();
}

/**
 * starting from the start block,
 * read from the device buffer_size / block_size
 * splice buffer from offset to offset + buffer_size
 * set the return buffer to the result
 * 
 * return -> -1 on error, 0 if successfull
 */
int IDevice::readFromDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size, char* buffer)
{
	// check those paramaters
	if (start_block < 0 || start_block > this->device->getBlocks())
	{
		printf("[_readFromDevice]: error, invalid start block...\n");
		return -1;
	}
	if (offset < 0 || offset >= this->device->getBlockSize())
	{
		printf("[_readFromDevice]: error, invalid offset value...\n");
		return -1;
	}
	if (buffer_size < 0 || buffer_size > this->device->getBlocks() * this->device->getBlockSize())
	{
		printf("[_readFromDevice]: error, invalid buffer size...\n");
		return -1;
	}

	uint32_t number_blocks_to_read = ceil((float)buffer_size / (float)this->device->getBlockSize());
	uint32_t read_buffer_size = number_blocks_to_read * this->device->getBlockSize();
	char *read_buffer = new char[read_buffer_size];
	for (uint32_t blk_idx = 0; blk_idx < number_blocks_to_read; blk_idx++)
	{
		char *block_buffer = new char[this->device->getBlockSize()];
		BlockDevice::result result = this->device->readBlock(start_block + blk_idx, block_buffer);
		if (result == 0)
			for (uint32_t j = 0; j < this->device->getBlockSize(); j++)
				read_buffer[blk_idx * this->device->getBlockSize() + j] = block_buffer[j];
		else
		{
			printf("[_readFromDevice]: error reading from the device...\n");
			return -1;
		}
	}
	// now we splice off all of the extra memory
	// buffer = read_buffer.splice(offset, offset + buffer_size)
	memcpy(buffer, (read_buffer + offset), buffer_size);
	return 0;
}

int IDevice::writeToDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size, char *buffer)
{
	// read from all the blocks the buffer is located in
	uint32_t block_size = this->device->getBlockSize();
	uint32_t number_blocks_to_read = ceil((float)buffer_size / (float)block_size);
	uint32_t read_buffer_size = number_blocks_to_read * block_size;
	char *read_buffer = new char[read_buffer_size];
	for (uint32_t blk_idx = 0; blk_idx < number_blocks_to_read; blk_idx++)
	{
		char *block_buffer = new char[block_size];
		BlockDevice::result result = this->device->readBlock(start_block + blk_idx, block_buffer);
		if (result == 0)
			for (uint32_t j = 0; j < block_size; j++)
				read_buffer[blk_idx * block_size + j] = block_buffer[j];
		else
		{
			printf("[_writeToDevice]: error reading from the device...\n");
			return -1;
		}
	}
	// splice the new values into the read buffer
	memcpy(read_buffer + offset, buffer, buffer_size);

	// now edit the write buffer with the updated buffer at the offset
	uint32_t number_blocks_to_write = number_blocks_to_read;
	for (uint32_t blk_idx = 0; blk_idx < number_blocks_to_write; blk_idx++)
	{
		char *block_buffer = new char[block_size];
		memcpy(block_buffer, read_buffer + (blk_idx * block_size), block_size);
		BlockDevice::result result = this->device->writeBlock(start_block + blk_idx, block_buffer);
		if (result != 0)
		{
			printf("[_writeToDevice]: error writing to the device...\n");
			return -1;
		}
	}
	return 0;
}
