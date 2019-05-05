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
 * return the buffer or null if error
 */
char *IDevice::readFromDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size)
{
	// check those paramaters
	if (start_block < 0 || start_block > this->device->getBlocks())
	{
		printf("[_readFromDevice]: error, invalid start block...\n");
		return NULL;
	}
	if (offset < 0 || offset >= this->device->getBlockSize())
	{
		printf("[_readFromDevice]: error, invalid offset value...\n");
		return NULL;
	}
	if (buffer_size < 0 || buffer_size > this->device->getBlocks() * this->device->getBlockSize())
	{
		printf("[_readFromDevice]: error, invalid buffer size...\n");
		return NULL;
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
			return NULL;
		}
	}
	// now we splice off all of the extra memory
	// buffer = read_buffer.splice(offset, offset + buffer_size)
	char *buffer = new char[buffer_size];
	memcpy(buffer, (read_buffer + offset), buffer_size);
	return buffer;
}

bool IDevice::writeToDevice(uint32_t start_block, uint32_t offset, uint32_t buffer_size, char *buffer)
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
		printf("writeBlock %d result %s\n", blk_idx, device->resultMessage(result));
		if (result == 0)
			for (uint32_t j = 0; j < block_size; j++)
				read_buffer[blk_idx * block_size + j] = block_buffer[j];
		else
		{
			printf("[_writeToDevice]: error reading from the device...\n");
			return NULL;
		}
	}
	// splice the new values into the read buffer
	memcpy(read_buffer + offset, buffer, buffer_size);

	// now edit the write buffer with the updated buffer at the offset
	uint32_t number_blocks_to_write = number_blocks_to_read;
	for (uint32_t blk_idx = 0; blk_idx < number_blocks_to_read; blk_idx++)
	{
		char *block_buffer = new char[block_size];
		memcpy(block_buffer, read_buffer + (blk_idx * block_size), block_size);
		hexDump(block_buffer, block_size);
		BlockDevice::result result = this->device->writeBlock(start_block + blk_idx, block_buffer);
		if (result != 0)
		{
			printf("[_readFromDevice]: error writing to the device...\n");
			return NULL;
		}
	}
	return true;
}
